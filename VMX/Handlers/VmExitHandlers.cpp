#include "../../Globals.h"

static void adjust_rip(__gcpu_context_t* gcpu) {
	unsigned __int64 instruction_length;

	int status = __vmx_vmread(VMX_VMEXIT_INSTRUCTION_LENGTH, &instruction_length);
	if (status != 0) {
		log_error("FAILED TO ADJUST RIP");
	}

	gcpu->ext_registers.rip += instruction_length;
	__vmx_vmwrite(GUEST_RIP, gcpu->ext_registers.rip);
}

unsigned long long get_guest_rip(void) {
	unsigned long long rip = 0;
	int status = __vmx_vmread(GUEST_RIP, &rip);
	if (status != 0) {
		log_error("FAILED TO GET GUEST RIP");
	}
	return rip;
}

unsigned long long get_guest_rsp() {
	unsigned long long rsp = 0;
	int status = __vmx_vmread(GUEST_RSP, &rsp);
	if (status != 0) {
		log_error("FAILED TO GET GUEST RSP");
	}
	return rsp;
}

unsigned long long get_guest_rflags() {
	unsigned long long rflags = 0;
	int status = __vmx_vmread(GUEST_RFLAGS, &rflags);
	if (status != 0) {
		log_error("FAILED TO GET GUEST RLFAGS");
	}
	return rflags;
}


void capture_guest_state(struct __gcpu_context_t* gcpu) {
	gcpu->ext_registers.rip = get_guest_rip();
	gcpu->ext_registers.rsp = get_guest_rsp();
	gcpu->ext_registers.rflags.flags = get_guest_rflags();
}



/*
	Handlers
*/

static vmexit_status_code vmexit_vmx_instruction_executed(struct __gcpu_context_t* gcpu) {
	struct __vmentry_event_information_t ud_exception;

	ud_exception.instruction_length = 0;
	ud_exception.error_code = 0;

	ud_exception.interrupt_info.bits.valid = 1;
	ud_exception.interrupt_info.bits.vector = EXCEPTION_UNDEFINED_OPCODE;
	ud_exception.interrupt_info.bits.interrupt_type = INTERRUPT_TYPE_HARDWARE_EXCEPTION;
	ud_exception.interrupt_info.bits.deliver_code = 0;

	__vmx_vmwrite(VMX_VMENTRY_INTERRUPTION_INFO, ud_exception.interrupt_info.flags);
	__vmx_vmwrite(VMX_VMENTRY_INSTRUCTION_LENGTH, ud_exception.instruction_length);

	gcpu->guest_registers.rflags.bits.rf = 1;
	__vmx_vmwrite(GUEST_RFLAGS, gcpu->guest_registers.rflags.flags);
	return VMEXIT_HANDLED;
}

static vmexit_status_code vmexit_cpuid_handler(struct __gcpu_context_t* gcpu) {
	struct __cpuid_params_t cpuid_reg;
	cpuid_reg.rax = gcpu->guest_registers.rax;
	cpuid_reg.rbx = gcpu->guest_registers.rbx;
	cpuid_reg.rcx = gcpu->guest_registers.rcx;
	cpuid_reg.rdx = gcpu->guest_registers.rdx;

	__cpuidex((int*)&cpuid_reg, gcpu->guest_registers.rax, gcpu->guest_registers.rcx);

	switch (cpuid_reg.rax) {
		case CPUID_HYPERVISOR_LEAF:
			if (QUERY_CPUID_BIT(cpuid_reg.rcx, 1)) {
				CLR_CPUID_BIT(cpuid_reg.rcx, 31);
			}
			break;
		default:
			break;
	}
	gcpu->guest_registers.rax = cpuid_reg.rax;
	gcpu->guest_registers.rbx = cpuid_reg.rbx;
	gcpu->guest_registers.rcx = cpuid_reg.rcx;
	gcpu->guest_registers.rdx = cpuid_reg.rdx;

	return VMEXIT_HANDLED;
}

static vmexit_status_code vmentry_inject_gp(struct __gcpu_context_t* gcpu, unsigned __int32 error_code) {
	struct __vmentry_event_information_t gp_exception{};
	__vmx_vmread(VMX_VMEXIT_INSTRUCTION_LENGTH, (size_t*)gp_exception.instruction_length);
	gp_exception.error_code = error_code;

	gp_exception.interrupt_info.bits.valid = 1;
	gp_exception.interrupt_info.bits.vector = EXCEPTION_GENERAL_PROTECTION_FAULT;
	gp_exception.interrupt_info.bits.interrupt_type = INTERRUPT_TYPE_HARDWARE_EXCEPTION;
	gp_exception.interrupt_info.bits.deliver_code = 1;

	__vmx_vmwrite(VMX_VMENTRY_EXCEPTION_ERROR_CODE, gp_exception.error_code);
	__vmx_vmwrite(VMX_VMENTRY_INTERRUPTION_INFO, gp_exception.interrupt_info.flags);
	__vmx_vmwrite(VMX_VMENTRY_INSTRUCTION_LENGTH, gp_exception.instruction_length);
	return VMEXIT_HANDLED;
}

static vmexit_status_code vmexit_msr_access(struct __gcpu_context_t* gcpu, bool access_type) {
	unsigned __int64 msr_value;
	unsigned __int64 msr_id;

	msr_id = gcpu->guest_registers.rcx;
	msr_value = 0;

	if (msr_id >= RESERVED_MSR_RANGE_LOW && (msr_id <= RESERVED_MSR_RANGE_HI)) {
		vmentry_inject_gp(gcpu, 0);
		return VMEXIT_HANDLED;
	}

	if (access_type == MSR_READ) {
		msr_value = __readmsr(msr_id);
		gcpu->guest_registers.rdx = (msr_value >> 32);
		gcpu->guest_registers.rax = (msr_value & MSR_MASK_LOW);
	}
	else {
		msr_value = (gcpu->guest_registers.rdx << 32);
		msr_value |= (gcpu->guest_registers.rax) & MSR_MASK_LOW;
		__writemsr(msr_id, msr_value);
	}

	adjust_rip(gcpu);

	return VMEXIT_HANDLED;
}

static void ap_hard_reset(void) {
	union __reset_control_register reset_register;
	reset_register.flags = __inbyte(RST_CNT_IO_PORT);

	reset_register.reset_cpu = 1;
	reset_register.system_reset = 1;

	__outbyte(RST_CNT_IO_PORT, reset_register.flags);
}

static void vmexit_triple_fault_handler() {
	log_error("VMEXIT: Triple fault detected resetting cpu");
	ap_hard_reset();
}

// Function to dump all GCPU state information in a readable format
void dump_gcpu_state_information(struct __guest_registers_t* gcpu) {
	if (!gcpu) {
		log_error("Dump: GCPU context is NULL.\n");
		return;
	}

	log_debug("====== GCPU REGISTER STATE ======\n");
	log_debug("RAX: %016llX\n", gcpu->rax);
	log_debug("RBX: %016llX\n", gcpu->rbx);
	log_debug("RCX: %016llX\n", gcpu->rcx);
	log_debug("RDX: %016llX\n", gcpu->rdx);
	log_debug("RBP: %016llX\n", gcpu->rbp);
	log_debug("RSI: %016llX\n", gcpu->rsi);
	log_debug("RDI: %016llX\n", gcpu->rdi);
	log_debug("R8:  %016llX\n", gcpu->r8);
	log_debug("R9:  %016llX\n", gcpu->r9);
	log_debug("R10: %016llX\n", gcpu->r10);
	log_debug("R11: %016llX\n", gcpu->r11);
	log_debug("R12: %016llX\n", gcpu->r12);
	log_debug("R13: %016llX\n", gcpu->r13);
	log_debug("R14: %016llX\n", gcpu->r14);
	log_debug("R15: %016llX\n", gcpu->r15);


	// You can add code to dump SIMD/XMM registers, control registers, etc.
}

vmexit_status_t vmexit_handler(struct __vmm_stack_t* stack) {
	struct vmexit_status_t vmexit_reason;
	struct __gcpu_context_t gcpu;

	vmexit_status_code exit_handling_status = VMEXIT_UNHANDLED;

	__vmx_vmread(VMX_VMEXIT_REASON, &vmexit_reason.exit_reason.flags);

	capture_guest_state(&gcpu);

	gcpu.guest_registers = stack->vmm_context.guest_registers;

	dump_gcpu_state_information(&gcpu.guest_registers);
	log_debug("Handling vmexit...");

	switch (vmexit_reason.exit_reason.bits.basic_exit_reason) {
		case vmexit_vmcall:
			exit_handling_status = vmentry_inject_gp(&gcpu, 0);
		case vmexit_vmclear:
		case vmexit_vmlaunch:
		case vmexit_vmptrld:
		case vmexit_vmptrst:
		case vmexit_vmread:
		case vmexit_vmresume:
		case vmexit_vmwrite:
		case vmexit_vmxoff:
		case vmexit_vmxon:
		case vmexit_invept:
		case vmexit_vmfunc:
		case vmexit_invvpid:
			exit_handling_status = vmexit_vmx_instruction_executed(&gcpu);
			// ADD THESE
			break;
		case vmexit_cpuid:
			exit_handling_status = vmexit_cpuid_handler(&gcpu);
			break;
		case vmexit_rdmsr:
			exit_handling_status = vmexit_msr_access(&gcpu, false);
			break;
		case vmexit_wrmsr:
			exit_handling_status = vmexit_msr_access(&gcpu, true);
			break;
		case vmexit_triple_fault:
			vmexit_triple_fault_handler();
			break;
		default:
			exit_handling_status = VMEXIT_UNHANDLED;
			break;
	}

	if (exit_handling_status == VMEXIT_UNHANDLED) {
		log_error("Vmexit: Failed to handle vmexit");
		KeBugCheck((ULONG)"Failed to handle vmexit");
	}

	return vmexit_reason;
}

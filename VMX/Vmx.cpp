#include "../Globals.h"

// Enable VMX Operation by configuring CR4 and MSR
BOOLEAN Enable_VMX_Operation() {
    union __cr4_t cr4 = { 0 };
    union __ia32_feature_control_msr_t feature_msr = { 0 };

    // Enable VMX in CR4
    cr4.control = __readcr4();
    cr4.bits.vmx_enable = 1;
    __writecr4(cr4.control);

    feature_msr.control = __readmsr(MSR_IA32_FEATURE_CONTROL);

    if (feature_msr.bits.lock == 0) {
        feature_msr.bits.lock = 1;
        feature_msr.bits.vmxon_outside_smx = 1;
        __writemsr(MSR_IA32_FEATURE_CONTROL, feature_msr.control);
        log_debug("Set vmx_enable to 1");
        return TRUE;
    }
    else if (feature_msr.bits.vmxon_outside_smx == 0) {
        log_debug("VMX locked in bios.");
        return FALSE;
    }

    return TRUE;
}

// Check if the CPU supports VMX by checking CPUID
BOOLEAN VMX::CheckSupport() {
    log_debug("Detecting VMX support");
    CPUID cpuid = { 0 };
    __cpuid((int*)&cpuid, 1);

    // Check if the VMX bit (bit 5) is set in ECX
    if ((cpuid.ecx & (1 << 5)) == 0)
        return FALSE;

    return TRUE;
}

// Allocate and Initialize VMM Context
struct __vmm_context_t* allocate_vmm_context() {
    struct __vmm_context_t* vmm_context = NULL;
    vmm_context = (struct __vmm_context_t*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vmm_context_t), POOLTAG);

    if (vmm_context == NULL) {
        log_error("Failed to allocate vmm_context");
        return NULL;
    }

    // Get the number of active processors
    vmm_context->ProcessorCount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

    // Allocate the vCPU table
    vmm_context->vcpu_table = (__vcpu_t**)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t*) * vmm_context->ProcessorCount, POOLTAG);
    if (vmm_context->vcpu_table == NULL) {
        log_error("Failed to allocate vcpu_table");
        ExFreePoolWithTag(vmm_context, POOLTAG);
        return NULL;
    }

    // Allocate the VMM stack
    vmm_context->stack = ExAllocatePoolWithTag(NonPagedPool, VMM_STACK_SIZE, POOLTAG);
    if (vmm_context->stack == NULL) {
        log_error("Failed to allocate vmm_context stack");
        ExFreePoolWithTag(vmm_context->vcpu_table, POOLTAG);
        ExFreePoolWithTag(vmm_context, POOLTAG);
        return NULL;
    }

    RtlSecureZeroMemory(vmm_context->stack, VMM_STACK_SIZE);

    log_debug("vmm_context allocated at %llX", vmm_context);
    log_debug("vcpu_table allocated at %llX", vmm_context->vcpu_table);
    log_debug("vmm stack allocated at %llX", vmm_context->stack);

    return vmm_context;
}

// Initialize and Allocate vCPU
struct __vcpu_t *init_vcpu() {
    struct __vcpu_t *vcpu = NULL;

    vcpu = (struct __vcpu_t*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t), POOLTAG);
    if (!vcpu) {
        log_error("Oops! vcpu could not be allocated.");
        return NULL;
    }

    RtlSecureZeroMemory(vcpu, sizeof(struct __vcpu_t));

    // Allocate and initialize the vmm_stack
    vcpu->vmm_stack = (struct __vmm_stack_t*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vmm_stack_t), POOLTAG);
    if (!vcpu->vmm_stack) {
        log_error("Failed to allocate vmm_stack for vcpu.");
        ExFreePoolWithTag(vcpu, POOLTAG);
        return NULL;
    }

    RtlSecureZeroMemory(vcpu->vmm_stack, sizeof(struct __vmm_stack_t));

    // Allocate and set up the MSR bitmap
    vcpu->vmm_stack->vmm_context.msr_bitmap = ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, POOLTAG);
    RtlSecureZeroMemory(vcpu->vmm_stack->vmm_context.msr_bitmap, PAGE_SIZE);
    vcpu->vmm_stack->vmm_context.msr_bitmap_physical = MmGetPhysicalAddress(vcpu->vmm_stack->vmm_context.msr_bitmap).QuadPart;

    log_debug("vcpu entry allocated successfully at %llX", vcpu);
    return vcpu;
}

// Initialize VMXON Region
int init_vmxon(struct __vcpu_t* vcpu) {
    union __vmx_basic_msr_t vmx_basic = { 0 };
    PHYSICAL_ADDRESS physical_max;

    if (!vcpu) {
        log_error("VMXON region could not be initialized. vcpu was null.");
        return FALSE;
    }

    // Read the VMX_BASIC MSR and allocate the VMXON region
    vmx_basic.control = __readmsr(MSR_IA32_VMX_BASIC);
    physical_max.QuadPart = ~0ULL;
    vcpu->vmxon = (struct __vmcs_t*)MmAllocateContiguousMemory(PAGE_SIZE, physical_max);

    if (!vcpu->vmxon) {
        log_error("Failed to allocate VMXON region.");
        return FALSE;
    }

    vcpu->vmxon_physical = MmGetPhysicalAddress(vcpu->vmxon).QuadPart;
    RtlSecureZeroMemory(vcpu->vmxon, PAGE_SIZE);
    vcpu->vmxon->header.all = vmx_basic.bits.vmcs_revision_identifier;

    log_debug("VMXON for vcpu %d initialized:\n\t-> VA: %llX\n\t-> PA: %llX\n\t-> REV: %X",
        KeGetCurrentProcessorNumber(),
        vcpu->vmxon,
        vcpu->vmxon_physical,
        vcpu->vmxon->header.all);

    return TRUE;
}

// Adjust CR0 and CR4 to match VMX requirements
void adjust_cr() {
    union __cr4_t cr4 = { 0 };
    union __cr0_t cr0 = { 0 };
    union __cr_fixed_t cr_fixed;

    // Adjust CR0 based on the MSRs
    cr_fixed.all = __readmsr(MSR_IA32_VMX_CR0_FIXED0);
    cr0.control = __readcr0();
    cr0.control |= cr_fixed.split.low;
    cr_fixed.all = __readmsr(MSR_IA32_VMX_CR0_FIXED1);
    cr0.control &= cr_fixed.split.low;
    __writecr0(cr0.control);

    // Adjust CR4 based on the MSRs
    cr_fixed.all = __readmsr(MSR_IA32_VMX_CR4_FIXED0);
    cr4.control = __readcr4();
    cr4.control |= cr_fixed.split.low;
    cr_fixed.all = __readmsr(MSR_IA32_VMX_CR4_FIXED1);
    cr4.control &= cr_fixed.split.low;
    __writecr4(cr4.control);
}

// Free memory allocated for a vCPU
int free_vcpu(struct __vcpu_t* vcpu) {
    ExFreePoolWithTag(vcpu->vmm_stack->vmm_context.msr_bitmap, POOLTAG);
    ExFreePoolWithTag(vcpu->vmm_stack, POOLTAG);
    ExFreePoolWithTag(vcpu, POOLTAG);
    log_debug("Successfully freed vcpu: %llX", vcpu);
    return TRUE;
}

// Free VMM context memory
int free_vmm_context(struct __vmm_context_t* context) {
    ExFreePoolWithTag(context->vcpu_table, POOLTAG);
    ExFreePoolWithTag(context->stack, POOLTAG);
    ExFreePoolWithTag(context, POOLTAG);
    return TRUE;
}

// Disable VMX operation
int VMX::disable_vmx() {
    __vmx_off();
    return TRUE;
}

// Initialize a logical processor for VMX operation, including VM launch
void init_logical_processor(struct __vmm_context_t* context, void* guest_rsp) {
    struct __vmm_context_t* vmm_context = context;
    struct __vcpu_t* vcpu;
    unsigned long processor_number = KeGetCurrentProcessorNumber();

    vcpu = vmm_context->vcpu_table[processor_number];
    log_debug("vcpu %d guest_rsp = %llX", processor_number, guest_rsp);

    // Adjust CR0 and CR4
    adjust_cr();

    // Initialize VMXON region
    if (!init_vmxon(vcpu)) {
        log_error("VMXON failed to initialize for vcpu %d.", processor_number);
        free_vcpu(vcpu);
        VMX::disable_vmx();
        return;
    }

    // Enter VMX operation
    if (__vmx_on(&vcpu->vmxon_physical) != 0) {
        log_error("Failed to put vcpu %d into VMX operation.", processor_number);
        free_vcpu(vcpu);
        VMX::disable_vmx();
        free_vmm_context(vmm_context);
        return;
    }

    log_debug("vcpu %d is now in VMX operation.", processor_number);

    // Initialize VMCS
    init_vmcs(vcpu, vmm_context);

    // Attempt to launch the virtual machine
    unsigned long long error_code;
    int status = vm_launch();
    if (status != 0) {
        __vmx_vmread(0x4400, &error_code);
        log_error("vmlaunch failed: %llu\n", error_code);
    }
    else {
        log_debug("Successfully launched guest!");
    }
    size_t guest_rip, Guest_rsp, host_rip, host_rsp;
    __vmx_vmread(VMX_GUEST_RIP, &guest_rip);
    __vmx_vmread(VMX_GUEST_RSP, &Guest_rsp);
    __vmx_vmread(VMX_HOST_RIP, &host_rip);
    __vmx_vmread(VMX_HOST_RSP, &host_rsp);

    log_debug("Guest RIP: %lx, Guest RSP: %lx, Host RIP: %lx, Host RSP: %lx\n", guest_rip, Guest_rsp, host_rip, host_rsp);
}

// Initialize VMM
void VMX::vmm_init(PVOID) {
    struct __vmm_context_t* vmm_context = allocate_vmm_context();
    if (!vmm_context) return;

    PROCESSOR_NUMBER processor_number;
    GROUP_AFFINITY affinity, old_affinity;

    // Initialize vCPUs
    for (unsigned i = 0; i < vmm_context->ProcessorCount; i++) {
        vmm_context->vcpu_table[i] = init_vcpu();
        if (!vmm_context->vcpu_table[i]) {
            log_error("Failed to initialize vCPU %d.", i);
            free_vmm_context(vmm_context);
            return;
        }
        vmm_context->vcpu_table[i]->vmm_stack->vmm_context = *vmm_context;
    }

    // Initialize each logical processor
    for (unsigned inter = 0; inter < vmm_context->ProcessorCount; inter++) {
        KeGetProcessorNumberFromIndex(inter, &processor_number);
        RtlSecureZeroMemory(&affinity, sizeof(GROUP_AFFINITY));
        affinity.Mask = (KAFFINITY)1 << processor_number.Number;
        KeSetSystemGroupAffinityThread(&affinity, &old_affinity);

        init_logical_processor(vmm_context, 0);

        KeRevertToUserGroupAffinityThread(&old_affinity);
    }
}

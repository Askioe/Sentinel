#include "../globals.h"


static void vmx_adjust_pinbased_controls(union __vmx_pinbased_control_msr_t* pinbased_controls) {
    union __vmx_true_control_settings_t cap;
    cap.control = __readmsr(MSR_IA32_VMX_TRUE_PINBASED_CTLS);

    pinbased_controls->control |= cap.allowed_0_settings;
    pinbased_controls->control &= cap.allowed_1_settings;
};

static void vmx_adjust_processor_based_controls(union __vmx_primary_processor_based_control_t* primary_controls) {
    union __vmx_true_control_settings_t cap;

    // Read the MSR for the processor-based controls
    cap.control = __readmsr(MSR_IA32_VMX_TRUE_PROCBASED_CTLS);

    // Adjust the control based on the allowed settings
    primary_controls->control |= cap.allowed_0_settings;  // Set required bits
    primary_controls->control &= cap.allowed_1_settings;
}
static void vmx_adjust_secondary_controls(union __vmx_secondary_processor_based_control_t* secondary_controls) {
    union __vmx_true_control_settings_t cap;

    // Read the MSR for the secondary processor-based controls
    cap.control = __readmsr(MSR_IA32_VMX_PROCBASED_CTLS2);

    // Adjust the control based on the allowed settings
    secondary_controls->control |= cap.allowed_0_settings;  // Set mandatory bits to 1
    secondary_controls->control &= cap.allowed_1_settings;
}

static uintptr_t vmx_adjust_cv(unsigned int capability_msr, unsigned int value)
{
    union __vmx_true_control_settings_t cap;
    unsigned int actual;

    cap.control = __readmsr(capability_msr);
    actual = value;

    actual |= cap.allowed_0_settings;
    actual &= cap.allowed_1_settings;
    return cap.control;
}

static void vmx_adjust_entry_controls(union __vmx_entry_control_t* entry_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;

    basic.control = __readmsr(MSR_IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? MSR_IA32_VMX_TRUE_ENTRY_CTLS : MSR_IA32_VMX_ENTRY_CTLS;

    entry_controls->control = vmx_adjust_cv(capability_msr, entry_controls->control);
}

static void vmx_adjust_exit_controls(union __vmx_exit_control_t* exit_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;
    basic.control = __readmsr(MSR_IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? MSR_IA32_VMX_TRUE_EXIT_CTLS : MSR_IA32_VMX_EXIT_CTLS;
    exit_controls->control = vmx_adjust_cv(capability_msr, exit_controls->control);
}

static unsigned __int32 read_segment_access_rights(unsigned __int16 segment_selector) {
    union __segment_selector_t selector;
    union __segment_access_rights_t vmx_access_rights;

    selector.flags = segment_selector;

    if (selector.table == 0
        && selector.index == 0)
    {
        vmx_access_rights.flags = 0;
        vmx_access_rights.unusable = TRUE;
        return vmx_access_rights.flags;
    }

    vmx_access_rights.flags = (__load_ar(segment_selector) >> 8);
    vmx_access_rights.unusable = 0;
    vmx_access_rights.reserved0 = 0;
    vmx_access_rights.reserved1 = 0;

    return vmx_access_rights.flags;
}

static unsigned __int64 get_segment_base(unsigned __int64 gdt_base, unsigned __int16 segment_selector) {
    unsigned __int64 segment_base;
    union __segment_selector_t selector;
    struct __segment_descriptor_32_t* descriptor;
    struct __segment_descriptor_32_t* descriptor_table;

    selector.flags = segment_selector;

    if (selector.table == 0 && selector.index == 0) {
        segment_base = 0;
        return segment_base;
    }

    descriptor_table = (struct __segment_descriptor_32_t*)gdt_base;
    descriptor = &descriptor_table[selector.index];

    segment_base = (unsigned __int64)((descriptor->base_high & 0xFF000000) |
        ((descriptor->base_middle << 16) & 0x00FF0000) |
        ((descriptor->base_low >> 16) & 0x0000FFFF));

    if ((descriptor->system == 0) &&
        ((descriptor->type == SEGMENT_DESCRIPTOR_TYPE_TSS_AVAILABLE) ||
            (descriptor->type == SEGMENT_DESCRIPTOR_TYPE_TSS_BUSY)))
    {
        struct __segment_descriptor_64_t* expanded_descriptor;
        expanded_descriptor = (struct __segment_descriptor_64_t*)descriptor;
        segment_base |= ((unsigned __int64)expanded_descriptor->base_upper << 32);
    }

    return segment_base;
}

void write_guest() {
    __vmx_vmwrite(GUEST_CR0, __readcr0());
    __vmx_vmwrite(GUEST_CR3, __readcr3());
    __vmx_vmwrite(GUEST_CR4, __readcr4());

    __vmx_vmwrite(GUEST_DR7, __readdr(7));

    __vmx_vmwrite(GUEST_RSP, 0);
    __vmx_vmwrite(GUEST_RIP, 0);

    __vmx_vmwrite(GUEST_RFLAGS, __readeflags());
    __vmx_vmwrite(GUEST_DEBUG_CONTROL, __readmsr(MSR_IA32_DEBUGCTL));
    __vmx_vmwrite(GUEST_SYSENTER_ESP, __readmsr(MSR_IA32_SYSENTER_ESP));
    __vmx_vmwrite(GUEST_SYSENTER_EIP, __readmsr(MSR_IA32_SYSENTER_EIP));
    __vmx_vmwrite(GUEST_SYSENTER_CS, __readmsr(MSR_IA32_SYSENTER_CS));
    __vmx_vmwrite(GUEST_VMCS_LINK_POINTER, MAXUINT64);
    __vmx_vmwrite(GUEST_FS_BASE, __readmsr(MSR_IA32_FS_BASE));
    __vmx_vmwrite(GUEST_GS_BASE, __readmsr(MSR_IA32_GS_BASE));
}

// Initialize all VMCS guest states, base, etc...
int init_vmcs(struct __vcpu_t* vcpu, struct __vmm_context_t* vmm_context) {
    struct __vmcs_t* vmcs;
    union __vmx_basic_msr_t vmx_basic = { 0 };
    PHYSICAL_ADDRESS physical_max;

    vmx_basic.control = __readmsr(MSR_IA32_VMX_BASIC);

    physical_max.QuadPart = ~0ULL;
    vcpu->vmcs = (__vmcs_t*)MmAllocateContiguousMemory(PAGE_SIZE, physical_max);
    vcpu->vmcs_physical = MmGetPhysicalAddress(vcpu->vmcs).QuadPart;
    RtlSecureZeroMemory(vcpu->vmcs, PAGE_SIZE);
    vmcs = vcpu->vmcs;
    vmcs->header.all = vmx_basic.bits.vmcs_revision_identifier;
    vmcs->header.bits.shadow_vmcs_indicator = 0;

    if ((__vmx_vmclear(&vcpu->vmcs_physical) != VMX_OK) || (__vmx_vmptrld(&vcpu->vmcs_physical) != VMX_OK)) {
        log_debug("Failed to clear vmcs_physical");
        return FALSE;
    }
    write_guest();


    __vmx_vmwrite(CR0_READ_SHADOW, __readcr0());
    __vmx_vmwrite(CR4_READ_SHADOW, __readcr4());

    union __vmx_entry_control_t entry_controls;
    entry_controls.bits.ia32e_mode_guest = TRUE;

    vmx_adjust_entry_controls(&entry_controls);

    union __vmx_exit_control_t exit_controls;

    exit_controls.control = 0;
    exit_controls.bits.host_address_space_size = TRUE;
    vmx_adjust_exit_controls(&exit_controls);

    union __vmx_pinbased_control_msr_t pinbased_controls;
    pinbased_controls.control = 0;
    vmx_adjust_pinbased_controls(&pinbased_controls);

    union __vmx_primary_processor_based_control_t primary_controls;
    primary_controls.control = 0;
    primary_controls.bits.use_msr_bitmaps = TRUE;
    primary_controls.bits.active_secondary_controls = TRUE;
    vmx_adjust_processor_based_controls(&primary_controls);

    union __vmx_secondary_processor_based_control_t secondary_controls;
    secondary_controls.control = 0;
    secondary_controls.bits.enable_rdtscp = TRUE;
    secondary_controls.bits.enable_xsave_xrstor = TRUE;
    secondary_controls.bits.enable_invpcid = TRUE;
    vmx_adjust_secondary_controls(&secondary_controls);



    // I should probably document this but ill do it later
    __vmx_vmwrite(VMX_PIN_BASED_VM_EXECUTION_CONTROLS, pinbased_controls.control);
    __vmx_vmwrite(VMX_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, primary_controls.control);
    __vmx_vmwrite(VMX_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, secondary_controls.control);
    __vmx_vmwrite(VMX_VMEXIT_CONTROLS, exit_controls.control);
    __vmx_vmwrite(VMX_VMENTRY_CONTROLS, entry_controls.control);

    __vmx_vmwrite(GUEST_CS_SELECTOR, __read_cs());
    __vmx_vmwrite(GUEST_SS_SELECTOR, __read_ss());
    __vmx_vmwrite(GUEST_DS_SELECTOR, __read_ds());
    __vmx_vmwrite(GUEST_ES_SELECTOR, __read_es());
    __vmx_vmwrite(GUEST_FS_SELECTOR, __read_fs());
    __vmx_vmwrite(GUEST_GS_SELECTOR, __read_fs());
    __vmx_vmwrite(GUEST_LDTR_SELECTOR, __read_ldtr());
    __vmx_vmwrite(GUEST_TR_SELECTOR, __read_tr());

    __vmx_vmwrite(GUEST_CS_LIMIT, __segmentlimit(__read_cs()));
    __vmx_vmwrite(GUEST_SS_LIMIT, __segmentlimit(__read_ss()));
    __vmx_vmwrite(GUEST_DS_LIMIT, __segmentlimit(__read_ds()));
    __vmx_vmwrite(GUEST_ES_LIMIT, __segmentlimit(__read_es()));
    __vmx_vmwrite(GUEST_FS_LIMIT, __segmentlimit(__read_fs()));
    __vmx_vmwrite(GUEST_GS_LIMIT, __segmentlimit(__read_fs()));
    __vmx_vmwrite(GUEST_LDTR_LIMIT, __segmentlimit(__read_ldtr()));
    __vmx_vmwrite(GUEST_TR_LIMIT, __segmentlimit(__read_tr()));

    struct __pseudo_descriptor_64_t gdtr;
    struct __pseudo_descriptor_64_t idtr;

    _sgdt(&gdtr);
    __sidt(&idtr);

    __vmx_vmwrite(GUEST_GDTR_LIMIT, gdtr.limit);
    __vmx_vmwrite(GUEST_IDTR_LIMIT, idtr.limit);
    __vmx_vmwrite(GUEST_GDTR_BASE, gdtr.base_address);
    __vmx_vmwrite(GUEST_IDTR_BASE, idtr.base_address);

    __vmx_vmwrite(GUEST_CS_ACCESS_RIGHTS, read_segment_access_rights(__read_cs()));
    __vmx_vmwrite(GUEST_SS_ACCESS_RIGHTS, read_segment_access_rights(__read_ss()));
    __vmx_vmwrite(GUEST_DS_ACCESS_RIGHTS, read_segment_access_rights(__read_ds()));
    __vmx_vmwrite(GUEST_ES_ACCESS_RIGHTS, read_segment_access_rights(__read_es()));
    __vmx_vmwrite(GUEST_FS_ACCESS_RIGHTS, read_segment_access_rights(__read_fs()));
    __vmx_vmwrite(GUEST_GS_ACCESS_RIGHTS, read_segment_access_rights(__read_gs()));
    __vmx_vmwrite(GUEST_LDTR_ACCESS_RIGHTS, read_segment_access_rights(__read_ldtr()));
    __vmx_vmwrite(GUEST_TR_ACCESS_RIGHTS, read_segment_access_rights(__read_tr()));

    __vmx_vmwrite(GUEST_LDTR_BASE, get_segment_base(gdtr.base_address, __read_ldtr()));
    __vmx_vmwrite(GUEST_TR_BASE, get_segment_base(gdtr.base_address, __read_tr()));


    __vmx_vmwrite(HOST_CS_SELECTOR, __read_cs() & ~SELECTOR_MASK);
    __vmx_vmwrite(HOST_SS_SELECTOR, __read_ss() & ~SELECTOR_MASK);
    __vmx_vmwrite(HOST_DS_SELECTOR, __read_ds() & ~SELECTOR_MASK);
    __vmx_vmwrite(HOST_ES_SELECTOR, __read_es() & ~SELECTOR_MASK);
    __vmx_vmwrite(HOST_FS_SELECTOR, __read_fs() & ~SELECTOR_MASK);
    __vmx_vmwrite(HOST_GS_SELECTOR, __read_gs() & ~SELECTOR_MASK);
    __vmx_vmwrite(HOST_TR_SELECTOR, __read_tr() & ~SELECTOR_MASK);

    __vmx_vmwrite(HOST_TR_BASE, get_segment_base(gdtr.base_address, __read_tr()));
    __vmx_vmwrite(HOST_GDTR_BASE, gdtr.base_address);
    __vmx_vmwrite(HOST_IDTR_BASE, idtr.base_address);

    unsigned __int64 vmm_stack = (unsigned __int64)&vcpu->vmm_stack->vmm_context;

    __vmx_vmwrite(HOST_RIP, (UINT64)vmm_entrypoint);
    __vmx_vmwrite(HOST_RSP, vmm_stack);

    vcpu->vmm_stack->vmm_context.msr_bitmap = vmm_context->msr_bitmap;
    vcpu->vmm_stack->vmm_context.ProcessorCount = vmm_context->ProcessorCount;
    vcpu->vmm_stack->vmm_context.vcpu_table = vmm_context->vcpu_table;
    return TRUE;
}


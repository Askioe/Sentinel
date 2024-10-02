#include "../Common.h"

bool VMX::CheckSupport() {
    log_debug("Detecting VMX support");
    int cpu_info[4];

    __cpuid(cpu_info, 1);

    log_debug("EAX: 0x%x\n", cpu_info[0]);
    log_debug("EBX: 0x%x\n", cpu_info[1]);
    log_debug("ECX: 0x%x\n", cpu_info[2]);
    log_debug("EDX: 0x%x\n", cpu_info[3]);

    if ((cpu_info[2] & (1 << 5)) == 0)
        return FALSE;

    return TRUE;

}

bool VMX::CheckAndSetInstalled() {
    union ia32_feature_control_msr_t feature_msr = { 0 };
    feature_msr.control = __readmsr(IA32_FEATURE_CONTROL_MSR);
    if (feature_msr.bits.lock == 0) {
        feature_msr.bits.lock = 1;               
        feature_msr.bits.vmxon_outside_smx = 1;  

        __writemsr(IA32_FEATURE_CONTROL_MSR, feature_msr.control);

        log_debug("VMX enabled outside SMX and MSR locked.");
    }
    else if (feature_msr.bits.vmxon_outside_smx == 0) {
        log_debug("VMX locked in BIOS.");
        return FALSE;
    }

    return TRUE;
}

bool VMX::Init_Vcpu(struct __VCPU_DATA_T* vcpu) {
    return true;
}

bool VMX::PrepareForVirtualization() {

    struct __VMM_CONTEXT_T* g_vmm_context;
    g_vmm_context = (struct __VMM_CONTEXT_T*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __VMM_CONTEXT_T), 'OHCA');
    if (g_vmm_context == NULL) {
        log_error("Failed to allocate guest vmm context");
        return FALSE;
    }
    RtlSecureZeroMemory(g_vmm_context, sizeof(__VMM_CONTEXT_T));


    g_vmm_context->processor_count = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    g_vmm_context->Vcpu_table = (struct __VCPU_DATA_T**)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __VCPU_DATA_T), 'OHCA');

    if (g_vmm_context->Vcpu_table == NULL) {
        log_error("Failed to allocate vcpu table");
        return FALSE;
    }
    RtlSecureZeroMemory(g_vmm_context->Vcpu_table, sizeof(__VCPU_DATA_T));
    
    for (unsigned int iter = 0; iter < g_vmm_context->processor_count; iter++) {
        if (!Init_Vcpu(g_vmm_context->Vcpu_table[iter])) {
            log_error("Failed to initialize vcpu");
        }
    }
    return TRUE;
}

bool VMX::Start_HV() {
    if (!VMX::CheckAndSetInstalled()) {
	log_debug("Failed to set VMX operation.");
	return FALSE;
    }

    if (!VMX::PrepareForVirtualization()) {
        log_debug("Failed to Virtualize system");
        return FALSE;
    }
    log_debug("VMX setup complete, initializing...");


}
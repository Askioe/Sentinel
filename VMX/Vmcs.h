#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	extern void __cdecl _sgdt(void* destination);

#ifdef __cplusplus
}
#endif


static uintptr_t vmx_adjust_cv(unsigned int capability_msr, unsigned int value);
static void vmx_adjust_entry_controls(union __vmx_entry_control_t* entry_controls);
int init_vmcs(struct __vcpu_t* vcpu, struct __vmm_context_t* vmm_context);
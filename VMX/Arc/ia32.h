#pragma once


namespace VMX {
	union ia32_feature_control_msr_t
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int64 lock : 1;
			unsigned __int64 vmxon_inside_smx : 1;
			unsigned __int64 vmxon_outside_smx : 1;
			unsigned __int64 reserved_0 : 5;
			unsigned __int64 senter_local : 6;
			unsigned __int64 senter_global : 1;
			unsigned __int64 reserved_1 : 1;
			unsigned __int64 sgx_launch_control_enable : 1;
			unsigned __int64 sgx_global_enable : 1;
			unsigned __int64 reserved_2 : 1;
			unsigned __int64 lmce : 1;
			unsigned __int64 system_reserved : 42;
		} bits;
	};
}


//DEFINES
// MSR address for IA32_FEATURE_CONTROL
#define IA32_FEATURE_CONTROL_MSR 0x3A

// Bit definitions for IA32_FEATURE_CONTROL MSR
#define IA32_FEATURE_CONTROL_LOCK_BIT          (1 << 0)  // Lock bit (bit 0)
#define IA32_FEATURE_CONTROL_VMX_ENABLE_BIT    (1 << 2)  // VMX enable outside SMX (bit 2)

// Macros to check if specific bits are set
#define IA32_FEATURE_CONTROL_IS_LOCKED(msr_value)  ((msr_value) & IA32_FEATURE_CONTROL_LOCK_BIT)
#define IA32_FEATURE_CONTROL_VMX_ENABLED(msr_value) ((msr_value) & IA32_FEATURE_CONTROL_VMX_ENABLE_BIT)

// Macro to enable VMX and lock the MSR
#define IA32_FEATURE_CONTROL_ENABLE_VMX(msr_value) \
    ((msr_value) | IA32_FEATURE_CONTROL_VMX_ENABLE_BIT | IA32_FEATURE_CONTROL_LOCK_BIT)

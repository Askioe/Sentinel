#pragma once

union __ia32_feature_control_msr_t
{
    unsigned __int64 control;
    ULONG64 All;
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
union __ia32_efer_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 syscall_enable : 1;
        unsigned __int64 reserved_0 : 7;
        unsigned __int64 long_mode_enable : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 long_mode_active : 1;
        unsigned __int64 execute_disable : 1;
        unsigned __int64 reserved_2 : 52;
    } bits;
};

union __vmx_misc_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 vmx_preemption_tsc_rate : 5;
        unsigned __int64 store_lma_in_vmentry_control : 1;
        unsigned __int64 activate_state_bitmap : 3;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 pt_in_vmx : 1;
        unsigned __int64 rdmsr_in_smm : 1;
        unsigned __int64 cr3_target_value_count : 9;
        unsigned __int64 max_msr_vmexit : 3;
        unsigned __int64 allow_smi_blocking : 1;
        unsigned __int64 vmwrite_to_any : 1;
        unsigned __int64 interrupt_mod : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 mseg_revision_identifier : 32;
    } bits;
};

typedef union _IA32_VMX_BASIC_MSR
{
    ULONG64 All;
    struct
    {
        ULONG32 RevisionIdentifier : 31;  // [0-30]
        ULONG32 Reserved1 : 1;            // [31]
        ULONG32 RegionSize : 12;          // [32-43]
        ULONG32 RegionClear : 1;          // [44]
        ULONG32 Reserved2 : 3;            // [45-47]
        ULONG32 SupportedIA64 : 1;        // [48]
        ULONG32 SupportedDualMoniter : 1; // [49]
        ULONG32 MemoryType : 4;           // [50-53]
        ULONG32 VmExitReport : 1;         // [54]
        ULONG32 VmxCapabilityHint : 1;    // [55]
        ULONG32 Reserved3 : 8;            // [56-63]
    } Fields;
} IA32_VMX_BASIC_MSR, * PIA32_VMX_BASIC_MSR;

union __vmx_basic_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 vmcs_revision_identifier : 31;
        unsigned __int64 always_0 : 1;
        unsigned __int64 vmxon_region_size : 13;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 vmxon_physical_address_width : 1;
        unsigned __int64 dual_monitor_smi : 1;
        unsigned __int64 memory_type : 4;
        unsigned __int64 io_instruction_reporting : 1;
        unsigned __int64 true_controls : 1;
    } bits;
};



#define MSR_APIC_BASE            0x01B
#define MSR_IA32_FEATURE_CONTROL 0x03A

#define MSR_IA32_VMX_BASIC               0x480
#define MSR_IA32_VMX_PINBASED_CTLS       0x481
#define MSR_IA32_VMX_PROCBASED_CTLS      0x482
#define MSR_IA32_VMX_EXIT_CTLS           0x483
#define MSR_IA32_VMX_ENTRY_CTLS          0x484
#define MSR_IA32_VMX_MISC                0x485
#define MSR_IA32_VMX_CR0_FIXED0          0x486
#define MSR_IA32_VMX_CR0_FIXED1          0x487
#define MSR_IA32_VMX_CR4_FIXED0          0x488
#define MSR_IA32_VMX_CR4_FIXED1          0x489
#define MSR_IA32_VMX_VMCS_ENUM           0x48A
#define MSR_IA32_VMX_PROCBASED_CTLS2     0x48B
#define MSR_IA32_VMX_EPT_VPID_CAP        0x48C
#define MSR_IA32_VMX_TRUE_PINBASED_CTLS  0x48D
#define MSR_IA32_VMX_TRUE_PROCBASED_CTLS 0x48E
#define MSR_IA32_VMX_TRUE_EXIT_CTLS      0x48F
#define MSR_IA32_VMX_TRUE_ENTRY_CTLS     0x490
#define MSR_IA32_VMX_VMFUNC              0x491

#define MSR_IA32_SYSENTER_CS  0x174
#define MSR_IA32_SYSENTER_ESP 0x175
#define MSR_IA32_SYSENTER_EIP 0x176
#define MSR_IA32_DEBUGCTL     0x1D9

#define MSR_LSTAR 0xC0000082

#define MSR_FS_BASE        0xC0000100
#define MSR_GS_BASE        0xC0000101
#define MSR_SHADOW_GS_BASE 0xC0000102 // SwapGS GS shadow
#define MSR_IA32_FS_BASE 0xC0000100
#define MSR_IA32_GS_BASE 0xC0000101

#define CR0_READ_SHADOW 0x6006  // VMCS field for CR0 shadow
#define CR4_READ_SHADOW 0x6008  // VMCS field for CR4 shadow
#define HOST_RSP 0x6C14  // VMCS field for host RSP
#define HOST_RIP 0x6C16  // VMCS field for host RIP

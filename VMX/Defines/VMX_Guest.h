#pragma once
enum __vmcs_access_e
{
    full = 0,
    high = 1
};

enum __vmcs_type_e
{
    control = 0,
    vmexit,
    guest,
    host
};

enum __vmcs_width_e
{
    word = 0,
    quadword,
    doubleword,
    natural
};

#define VMCS_ENCODE_COMPONENT( access, type, width, index )    ( unsigned )( ( unsigned short )( access ) | \
                                                                        ( ( unsigned short )( index ) << 1 ) | \
                                                                        ( ( unsigned short )( type ) << 10 ) | \
                                                                        ( ( unsigned short )( width ) << 13 ) )
#define VMCS_ENCODE_COMPONENT_FULL( type, width, index )    VMCS_ENCODE_COMPONENT( full, type, width, index )

#define VMCS_ENCODE_COMPONENT_FULL_16( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, word, index )
#define VMCS_ENCODE_COMPONENT_FULL_32( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, doubleword, index )
#define VMCS_ENCODE_COMPONENT_FULL_64( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, quadword, index )

#define VMX_VMEXIT_REASON 0x4402


union __vmx_true_control_settings_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int32 allowed_0_settings;
        unsigned __int32 allowed_1_settings;
    };
};

enum __vmcs_fields_e
{
    GUEST_CR0 = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 0),
    GUEST_CR3 = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 1),
    GUEST_CR4 = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 2),
    GUEST_ES_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 3),
    GUEST_CS_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 4),
    GUEST_SS_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 5),
    GUEST_DS_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 6),
    GUEST_FS_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 7),
    GUEST_GS_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 8),
    GUEST_LDTR_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 9),
    GUEST_TR_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 10),
    GUEST_GDTR_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 11),
    GUEST_IDTR_BASE = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 12),
    GUEST_DR7 = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 13),
    GUEST_RSP = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 14),
    GUEST_RIP = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 15),
    GUEST_RFLAGS = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 16),
    GUEST_SYSENTER_ESP = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 18),
    GUEST_SYSENTER_EIP = VMCS_ENCODE_COMPONENT_FULL(guest, natural, 19),

    GUEST_VMCS_LINK_POINTER = VMCS_ENCODE_COMPONENT_FULL_64(guest, 0),
    GUEST_DEBUG_CONTROL = VMCS_ENCODE_COMPONENT_FULL_64(guest, 1),


    GUEST_PAT = VMCS_ENCODE_COMPONENT_FULL_64(guest, 2),
    GUEST_EFER = VMCS_ENCODE_COMPONENT_FULL_64(guest, 3),
    GUEST_PERF_GLOBAL_CONTROL = VMCS_ENCODE_COMPONENT_FULL_64(guest, 4),
    GUEST_BNDCFGS = VMCS_ENCODE_COMPONENT_FULL_64(guest, 9),

    GUEST_ES_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 0),
    GUEST_CS_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 1),
    GUEST_SS_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 2),
    GUEST_DS_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 3),
    GUEST_FS_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 4),
    GUEST_GS_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 5),
    GUEST_LDTR_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 6),
    GUEST_TR_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 7),
    GUEST_GDTR_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 8),
    GUEST_IDTR_LIMIT = VMCS_ENCODE_COMPONENT_FULL_32(guest, 9),
    GUEST_ES_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 10),
    GUEST_CS_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 11),
    GUEST_SS_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 12),
    GUEST_DS_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 13),
    GUEST_FS_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 14),
    GUEST_GS_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 15),
    GUEST_LDTR_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 16),
    GUEST_TR_ACCESS_RIGHTS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 17),
    GUEST_SMBASE = VMCS_ENCODE_COMPONENT_FULL_32(guest, 20),
    GUEST_SYSENTER_CS = VMCS_ENCODE_COMPONENT_FULL_32(guest, 21),

    GUEST_ES_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 0),
    GUEST_CS_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 1),
    GUEST_SS_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 2),
    GUEST_DS_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 3),
    GUEST_FS_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 4),
    GUEST_GS_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 5),
    GUEST_LDTR_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 6),
    GUEST_TR_SELECTOR = VMCS_ENCODE_COMPONENT_FULL_16(guest, 7),

};

union __vmx_pinbased_control_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 external_interrupt_exiting : 1;
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 nmi_exiting : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 virtual_nmis : 1;
        unsigned __int64 vmx_preemption_timer : 1;
        unsigned __int64 process_posted_interrupts : 1;
    } bits;
};

union __vmx_primary_processor_based_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 interrupt_window_exiting : 1;
        unsigned __int64 use_tsc_offsetting : 1;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 hlt_exiting : 1;
        unsigned __int64 reserved_2 : 1;
        unsigned __int64 invldpg_exiting : 1;
        unsigned __int64 mwait_exiting : 1;
        unsigned __int64 rdpmc_exiting : 1;
        unsigned __int64 rdtsc_exiting : 1;
        unsigned __int64 reserved_3 : 2;
        unsigned __int64 cr3_load_exiting : 1;
        unsigned __int64 cr3_store_exiting : 1;
        unsigned __int64 reserved_4 : 2;
        unsigned __int64 cr8_load_exiting : 1;
        unsigned __int64 cr8_store_exiting : 1;
        unsigned __int64 use_tpr_shadow : 1;
        unsigned __int64 nmi_window_exiting : 1;
        unsigned __int64 mov_dr_exiting : 1;
        unsigned __int64 unconditional_io_exiting : 1;
        unsigned __int64 use_io_bitmaps : 1;
        unsigned __int64 reserved_5 : 1;
        unsigned __int64 monitor_trap_flag : 1;
        unsigned __int64 use_msr_bitmaps : 1;
        unsigned __int64 monitor_exiting : 1;
        unsigned __int64 pause_exiting : 1;
        unsigned __int64 active_secondary_controls : 1;
    } bits;
};

union __vmx_secondary_processor_based_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 virtualize_apic_accesses : 1;
        unsigned __int64 enable_ept : 1;
        unsigned __int64 descriptor_table_exiting : 1;
        unsigned __int64 enable_rdtscp : 1;
        unsigned __int64 virtualize_x2apic : 1;
        unsigned __int64 enable_vpid : 1;
        unsigned __int64 wbinvd_exiting : 1;
        unsigned __int64 unrestricted_guest : 1;
        unsigned __int64 apic_register_virtualization : 1;
        unsigned __int64 virtual_interrupt_delivery : 1;
        unsigned __int64 pause_loop_exiting : 1;
        unsigned __int64 rdrand_exiting : 1;
        unsigned __int64 enable_invpcid : 1;
        unsigned __int64 enable_vmfunc : 1;
        unsigned __int64 vmcs_shadowing : 1;
        unsigned __int64 enable_encls_exiting : 1;
        unsigned __int64 rdseed_exiting : 1;
        unsigned __int64 enable_pml : 1;
        unsigned __int64 use_virtualization_exception : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
        unsigned __int64 enable_xsave_xrstor : 1;
        unsigned __int64 reserved_0 : 1;
        unsigned __int64 mode_based_execute_control_ept : 1;
        unsigned __int64 reserved_1 : 2;
        unsigned __int64 use_tsc_scaling : 1;
    } bits;
};

union __vmx_exit_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 save_dbg_controls : 1;
        unsigned __int64 reserved_1 : 6;
        unsigned __int64 host_address_space_size : 1;
        unsigned __int64 reserved_2 : 2;
        unsigned __int64 load_ia32_perf_global_control : 1;
        unsigned __int64 reserved_3 : 2;
        unsigned __int64 ack_interrupt_on_exit : 1;
        unsigned __int64 reserved_4 : 2;
        unsigned __int64 save_ia32_pat : 1;
        unsigned __int64 load_ia32_pat : 1;
        unsigned __int64 save_ia32_efer : 1;
        unsigned __int64 load_ia32_efer : 1;
        unsigned __int64 save_vmx_preemption_timer_value : 1;
        unsigned __int64 clear_ia32_bndcfgs : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
    } bits;
};

union __vmx_exit_reason_field_t
{
    unsigned __int64 flags;
    struct
    {
        unsigned __int64 basic_exit_reason : 16;
        unsigned __int64 must_be_zero_1 : 11;
        unsigned __int64 was_in_enclave_mode : 1;
        unsigned __int64 pending_mtf_exit : 1;
        unsigned __int64 exit_from_vmx_root : 1;
        unsigned __int64 must_be_zero_2 : 1;
        unsigned __int64 vm_entry_failure : 1;
    } bits;
};

union __vmx_entry_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 load_dbg_controls : 1;
        unsigned __int64 reserved_1 : 6;
        unsigned __int64 ia32e_mode_guest : 1;
        unsigned __int64 entry_to_smm : 1;
        unsigned __int64 deactivate_dual_monitor_treament : 1;
        unsigned __int64 reserved_3 : 1;
        unsigned __int64 load_ia32_perf_global_control : 1;
        unsigned __int64 load_ia32_pat : 1;
        unsigned __int64 load_ia32_efer : 1;
        unsigned __int64 load_ia32_bndcfgs : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
    } bits;
};


struct __guest_registers_t
{
    __m128 xmm[6];
    void* padding;
    unsigned __int64 r15;
    unsigned __int64 r14;
    unsigned __int64 r13;
    unsigned __int64 r12;
    unsigned __int64 r11;
    unsigned __int64 r10;
    unsigned __int64 r9;
    unsigned __int64 r8;
    unsigned __int64 rdi;
    unsigned __int64 rsi;
    unsigned __int64 rbp;
    unsigned __int64 rbx;
    unsigned __int64 rdx;
    unsigned __int64 rcx;
    unsigned __int64 rax;
};
struct __vmexit_stack_t
{
    struct __guest_registers_t guest_registers;
    struct __vmm_context_t vmm_context;
};

struct __vmexit_guest_registers_t
{
    unsigned __int64 rax;
    unsigned __int64 rcx;
    unsigned __int64 rdx;
    unsigned __int64 rbx;
    unsigned __int64 rsp;
    unsigned __int64 rbp;
    unsigned __int64 rsi;
    unsigned __int64 rdi;
    unsigned __int64 r8;
    unsigned __int64 r9;
    unsigned __int64 r10;
    unsigned __int64 r11;
    unsigned __int64 r12;
    unsigned __int64 r13;
    unsigned __int64 r14;        // 70h
    unsigned __int64 r15;
};

struct vmexit_status_t
{
    union __vmx_exit_reason_field_t exit_reason;     // VM exit reason (already defined)
    unsigned __int64 exit_qualification;            // Additional information on the exit reason (specific to certain exits)
    unsigned __int64 guest_rip;                     // Guest instruction pointer at the time of the VM exit
    unsigned __int64 guest_rsp;                     // Guest stack pointer at the time of the VM exit
    unsigned __int64 guest_cr0;                     // Guest CR0 value at the time of the exit
    unsigned __int64 guest_cr3;                     // Guest CR3 value at the time of the exit
    unsigned __int64 guest_cr4;                     // Guest CR4 value at the time of the exit
    union __rflags_t guest_rflags;                  // Guest RFLAGS (defined in rflags.h)

    struct __vmexit_guest_registers_t guest_registers; // General-purpose registers (already defined)

    unsigned __int64 guest_linear_address;          // Guest linear address (for certain exits, like page faults)
    unsigned __int64 guest_physical_address;        // Guest physical address (for EPT violations)
    unsigned int instruction_length;                // Length of the instruction causing the exit
};

struct __ext_registers_t
{
    unsigned __int64 rip;
    unsigned __int64 rsp;
    union __rflags_t rflags;
};

struct __gcpu_context_t
{
    void* vcpu;
    struct __ext_registers_t ext_registers;
    struct __guest_registers_t guest_registers;
};

enum __vmexit_reason_e
{
    vmexit_nmi = 0,
    vmexit_ext_int,
    vmexit_triple_fault,
    vmexit_init_signal,
    vmexit_sipi,
    vmexit_smi,
    vmexit_other_smi,
    vmexit_interrupt_window,
    vmexit_nmi_window,
    vmexit_task_switch,
    vmexit_cpuid,
    vmexit_getsec,
    vmexit_hlt,
    vmexit_invd,
    vmexit_invlpg,
    vmexit_rdpmc,
    vmexit_rdtsc,
    vmexit_rsm,
    vmexit_vmcall,
    vmexit_vmclear,
    vmexit_vmlaunch,
    vmexit_vmptrld,
    vmexit_vmptrst,
    vmexit_vmread,
    vmexit_vmresume,
    vmexit_vmwrite,
    vmexit_vmxoff,
    vmexit_vmxon,
    vmexit_control_register_access,
    vmexit_mov_dr,
    vmexit_io_instruction,
    vmexit_rdmsr,
    vmexit_wrmsr,
    vmexit_vmentry_failure_due_to_guest_state,
    vmexit_vmentry_failure_due_to_msr_loading,
    vmexit_mwait = 36,
    vmexit_monitor_trap_flag,
    vmexit_monitor = 39,
    vmexit_pause,
    vmexit_vmentry_failure_due_to_machine_check_event,
    vmexit_tpr_below_threshold = 43,
    vmexit_apic_access,
    vmexit_virtualized_eoi,
    vmexit_access_to_gdtr_or_idtr,
    vmexit_access_to_ldtr_or_tr,
    vmexit_ept_violation,
    vmexit_ept_misconfiguration,
    vmexit_invept,
    vmexit_rdtscp,
    vmexit_vmx_preemption_timer_expired,
    vmexit_invvpid,
    vmexit_wbinvd,
    vmexit_xsetbv,
    vmexit_apic_write,
    vmexit_rdrand,
    vmexit_invpcid,
    vmexit_vmfunc,
    vmexit_encls,
    vmexit_rdseed,
    vmexit_pml_full,
    vmexit_xsaves,
    vmexit_xrstors,
};


// Pin-Based VM Execution Controls
#define VMX_PIN_BASED_VM_EXECUTION_CONTROLS         0x4000

// Primary Processor-Based VM Execution Controls
#define VMX_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS  0x4002

// Secondary Processor-Based VM Execution Controls
#define VMX_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS  0x401E

// VM-Exit Controls
#define VMX_VMEXIT_CONTROLS                         0x400C

// VM-Entry Controls
#define VMX_VMENTRY_CONTROLS                        0x4012


/*
Yoinked this structure from:
https://github.com/jonomango/hv/blob/main/hv/guest-context.h
Credits to jonomango!
*/

struct alignas(16) guest_context {
    union {
        uint64_t gpr[16];

        // aliases for general-purpose registers
        struct {
            union {
                uint64_t rax;
                uint32_t eax;
                uint16_t ax;
                uint8_t  al;
            };
            union {
                uint64_t rcx;
                uint32_t ecx;
                uint16_t cx;
                uint8_t  cl;
            };
            union {
                uint64_t rdx;
                uint32_t edx;
                uint16_t dx;
                uint8_t  dl;
            };
            union {
                uint64_t rbx;
                uint32_t ebx;
                uint16_t bx;
                uint8_t  bl;
            };

            // this is where RSP would be if it wasn't saved in the vmcs
            uint64_t _padding;

            union {
                uint64_t rbp;
                uint32_t ebp;
                uint16_t bp;
                uint8_t  bpl;
            };
            union {
                uint64_t rsi;
                uint32_t esi;
                uint16_t si;
                uint8_t  sil;
            };
            union {
                uint64_t rdi;
                uint32_t edi;
                uint16_t di;
                uint8_t  dil;
            };
            union {
                uint64_t r8;
                uint32_t r8d;
                uint16_t r8w;
                uint8_t  r8b;
            };
            union {
                uint64_t r9;
                uint32_t r9d;
                uint16_t r9w;
                uint8_t  r9b;
            };
            union {
                uint64_t r10;
                uint32_t r10d;
                uint16_t r10w;
                uint8_t  r10b;
            };
            union {
                uint64_t r11;
                uint32_t r11d;
                uint16_t r11w;
                uint8_t  r11b;
            };
            union {
                uint64_t r12;
                uint32_t r12d;
                uint16_t r12w;
                uint8_t  r12b;
            };
            union {
                uint64_t r13;
                uint32_t r13d;
                uint16_t r13w;
                uint8_t  r13b;
            };
            union {
                uint64_t r14;
                uint32_t r14d;
                uint16_t r14w;
                uint8_t  r14b;
            };
            union {
                uint64_t r15;
                uint32_t r15d;
                uint16_t r15w;
                uint8_t  r15b;
            };
        };
    };

    // control registers
    uint64_t cr2;
    uint64_t cr8;

    // debug registers
    uint64_t dr0;
    uint64_t dr1;
    uint64_t dr2;
    uint64_t dr3;
    uint64_t dr6;

    // SSE registers
    M128A xmm0;
    M128A xmm1;
    M128A xmm2;
    M128A xmm3;
    M128A xmm4;
    M128A xmm5;
    M128A xmm6;
    M128A xmm7;
    M128A xmm8;
    M128A xmm9;
    M128A xmm10;
    M128A xmm11;
    M128A xmm12;
    M128A xmm13;
    M128A xmm14;
    M128A xmm15;
};

// remember to update this value in vm-exit.asm
static_assert(sizeof(guest_context) == 0x1C0, "guest_context size must be 0x1C0");

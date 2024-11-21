#pragma once
#define POOLTAG 0x48564653
#define VMM_STACK_SIZE 600016
#define VMX_OK 0
#define VMX_FAIL 1

struct CPUID {
    int eax;
    int ebx;
    int ecx;
    int edx;
};

struct __cpuid_params_t {
    unsigned __int64 rax;
    unsigned __int64 rbx;
    unsigned __int64 rcx;
    unsigned __int64 rdx;
};

struct __vmcs_t
{
    union
    {
        unsigned int all;
        struct
        {
            unsigned int revision_identifier : 31;
            unsigned int shadow_vmcs_indicator : 1;
        } bits;
    } header;

    unsigned int abort_indicator;
    char data[0x1000 - 2 * sizeof(unsigned)];
};

struct __vmm_stack_t;
struct __vcpu_t;

struct __guest_registers_t
{
    __rflags_t rflags;
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


struct __vmm_context_t
{
    size_t ProcessorCount;
    __declspec(align(4096)) struct __vcpu_t** vcpu_table;
    __declspec(align(4096)) void* msr_bitmap;
    unsigned __int64 msr_bitmap_physical;
    void* stack;
    size_t stack_size;
    struct __guest_registers_t guest_registers;
};

struct __vcpu_t
{
    struct vmexit_status_t* status;
    unsigned __int64 guest_rsp;
    unsigned __int64 guest_rip;

    struct __vmcs_t* vmcs;
    unsigned __int64 vmcs_physical;

    struct __vmcs_t* vmxon;
    unsigned __int64 vmxon_physical;

    __declspec(align(4096)) struct __vmm_stack_t* vmm_stack;
};

struct __vmm_stack_t
{
    unsigned char stackLimit[VMM_STACK_SIZE - sizeof(struct __vmm_context_t)];
    struct __vmm_context_t vmm_context;
};

#define GUEST_STACK_SIZE  (16 * 1024)



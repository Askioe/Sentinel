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




struct __vmm_context_t
{
    size_t ProcessorCount;
    __declspec(align(4096)) struct __vcpu_t** vcpu_table;
    __declspec(align(4096)) void* msr_bitmap;
    unsigned __int64 __msr_bitmap_physical;
    void* stack;
    size_t stack_size;
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
    unsigned char limit[VMM_STACK_SIZE - sizeof(struct __vmm_context_t)];
    struct __vmm_context_t vmm_context;
};




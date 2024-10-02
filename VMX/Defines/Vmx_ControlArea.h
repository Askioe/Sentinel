#pragma once

namespace VMX {
    typedef struct __VMM_CONTEXT_T {
        __declspec(align(4096)) struct __VCPU_DATA_T** Vcpu_table;
        __declspec(align(4096)) void* msr_bitmap;
        unsigned __int64 __msr_bitmap_physical;
        void* stack;
        size_t stack_size;
        unsigned __int32 processor_count;
    };

}
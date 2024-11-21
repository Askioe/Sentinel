#pragma once

//
// Segment Selector Intrinsics
//
extern "C" unsigned short __read_ldtr(void);
extern "C" unsigned short __read_tr(void);
extern "C" unsigned short __read_cs(void);
extern "C" unsigned short __read_ss(void);
extern "C" unsigned short __read_ds(void);
extern "C" unsigned short __read_es(void);
extern "C" unsigned short __read_fs(void);
extern "C" unsigned short __read_gs(void);
extern "C" unsigned short __load_ar(unsigned __int16 segment_selector);
extern "C" void vmm_entrypoint(void);
extern "C" unsigned long long __read_rip(void);
extern "C" unsigned long long __read_rsp(void);



#define SEGMENT_DESCRIPTOR_TYPE_TSS_AVAILABLE 0x9  // Available TSS (1001b)
#define SEGMENT_DESCRIPTOR_TYPE_TSS_BUSY      0xB  // Busy TSS (1011b)
#define HOST_CS_SELECTOR  0x00000C02  // VMCS field for Host CS Selector
#define HOST_SS_SELECTOR  0x00000C04  // VMCS field for Host SS Selector
#define HOST_DS_SELECTOR  0x00000C06  // VMCS field for Host DS Selector
#define HOST_ES_SELECTOR  0x00000C00  // VMCS field for Host ES Selector
#define HOST_FS_SELECTOR  0x00000C08  // VMCS field for Host FS Selector
#define HOST_GS_SELECTOR  0x00000C0A  // VMCS field for Host GS Selector
#define HOST_TR_SELECTOR  0x00000C0C  // VMCS field for Host Task Register (TR) Selector
#define SELECTOR_MASK 0xFFF8
#define HOST_TR_BASE   0x00006C0C  // VMCS field for Host Task Register (TR) Base
#define HOST_GDTR_BASE 0x00006C10  // VMCS field for Host GDTR Base
#define HOST_IDTR_BASE 0x00006C12  // VMCS field for Host IDTR Base

#define VMX_VMEXIT_INSTRUCTION_LENGTH 0x440C   // Field encoding for instruction length on exit

#define VMX_VMENTRY_INTERRUPTION_INFO   0x00004016
#define VMX_VMENTRY_INSTRUCTION_LENGTH  0x0000401A

#pragma pack(push, 1)
struct __pseudo_descriptor_64_t
{
    unsigned __int16 limit;
    unsigned __int64 base_address;
};
#pragma pack(pop)

struct __segment_descriptor_64_t
{
    unsigned __int16 segment_limit_low;
    unsigned __int16 base_low;
    union
    {
        struct
        {
            unsigned __int32 base_middle : 8;
            unsigned __int32 type : 4;
            unsigned __int32 descriptor_type : 1;
            unsigned __int32 dpl : 2;
            unsigned __int32 present : 1;
            unsigned __int32 segment_limit_high : 4;
            unsigned __int32 system : 1;
            unsigned __int32 long_mode : 1;
            unsigned __int32 default_big : 1;
            unsigned __int32 granularity : 1;
            unsigned __int32 base_high : 8;
        };

        unsigned __int32 flags;
    };
    unsigned __int32 base_upper;
    unsigned __int32 reserved;
};

struct __segment_descriptor_32_t
{
    unsigned __int16 segment_limit_low;
    unsigned __int16 base_low;
    union
    {
        struct
        {
            unsigned __int32 base_middle : 8;
            unsigned __int32 type : 4;
            unsigned __int32 descriptor_type : 1;
            unsigned __int32 dpl : 2;
            unsigned __int32 present : 1;
            unsigned __int32 segment_limit_high : 4;
            unsigned __int32 system : 1;
            unsigned __int32 long_mode : 1;
            unsigned __int32 default_big : 1;
            unsigned __int32 granularity : 1;
            unsigned __int32 base_high : 8;
        };

        unsigned __int32 flags;
    };
};

union __segment_selector_t
{
    struct
    {
        unsigned __int16 rpl : 2;
        unsigned __int16 table : 1;
        unsigned __int16 index : 13;
    };

    unsigned __int16 flags;
};

union __segment_access_rights_t
{
    struct
    {
        unsigned __int32 type : 4;
        unsigned __int32 descriptor_type : 1;
        unsigned __int32 dpl : 2;
        unsigned __int32 present : 1;
        unsigned __int32 reserved0 : 4;
        unsigned __int32 available : 1;
        unsigned __int32 long_mode : 1;
        unsigned __int32 default_big : 1;
        unsigned __int32 granularity : 1;
        unsigned __int32 unusable : 1;
        unsigned __int32 reserved1 : 15;
    };

    unsigned __int32 flags;
};
#pragma once

namespace VMX {

	typedef struct __VMCS_T
	{
		union
		{
			unsigned int all;
			struct
			{
				unsigned int revision_identifier : 31;
				unsigned int shadow_vmcs_indicator : 1;
			};
		} header;
		unsigned int abort_indicator;
		char data[0x1000 - 2 * sizeof(unsigned)];
	};

	typedef struct __VCPU_DATA_T {

		void* vmm_stack;

		unsigned __int64 guest_rsp;
		unsigned __int64 guest_rip;

		struct __VMCS_T* vmcs;
		unsigned __int64 vmcs_physical;
		
		struct __VMCS_T* vmxon;
		unsigned __int64 vmxon_physical;
	};

}
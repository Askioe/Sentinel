#pragma once

namespace VMX {
	bool CheckSupport();
	bool CheckAndSetInstalled();

	bool Init_Vcpu(struct __VCPU_DATA_T* vcpu);
	bool PrepareForVirtualization();

	bool Start_HV();
}
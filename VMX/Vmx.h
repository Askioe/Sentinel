#pragma once

bool vm_launch();


namespace VMX {
	BOOLEAN CheckSupport();
	void vmm_init(PVOID);
	int disable_vmx();
}
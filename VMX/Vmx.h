#pragma once

bool vm_launch();


namespace VMX {
	BOOLEAN CheckSupport();
	int vmm_init();
	int disable_vmx();
}
#include "globals.h"

NTSTATUS Entry() {

	log_debug("Initializing Driver");

	if (!VMX::CheckSupport()) {
		log_debug("Failed VMX support check");
		return STATUS_ABANDONED;
	}
	log_debug("VMX supported!");

	HANDLE threadHandle;
	const NTSTATUS status = PsCreateSystemThread(&threadHandle, THREAD_ALL_ACCESS, nullptr, nullptr, nullptr, VMX::vmm_init, nullptr);
	
	return STATUS_SUCCESS;
}


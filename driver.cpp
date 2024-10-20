#include "globals.h"

auto driver_unload(PDRIVER_OBJECT DriverObject) -> void {

	log_debug("Driver unload called.");
	VMX::disable_vmx();
	log_debug("Disabled VMX");

}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry_Path) {
	UNREFERENCED_PARAMETER(Registry_Path);


	log_debug("Initializing Driver");

	if (!VMX::CheckSupport()) {
		log_debug("Failed VMX support check");
		return STATUS_ABANDONED;
	}
	log_debug("VMX supported!");

	VMX::vmm_init();


	DriverObject->DriverUnload = &driver_unload;

	return STATUS_SUCCESS;
}


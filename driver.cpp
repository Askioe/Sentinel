#include "Common.h"

auto driver_unload(PDRIVER_OBJECT DriverObject) -> void {

	log_debug("Driver unload called.");
	log_debug("Disabled VMX");

}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry_Path) {
	UNREFERENCED_PARAMETER(Registry_Path);


	log_debug("Initializing Driver");

	DriverObject->DriverUnload = &driver_unload;


	if (!VMX::CheckSupport()) {
		log_debug("Failed VMX support check");
		return STATUS_ABANDONED;
	}
	log_debug("VMX supported!");

	if (!VMX::Start_HV()) {
		log_error("Failed to start HV");
		return STATUS_ABANDONED;
	}
	return STATUS_SUCCESS;
}

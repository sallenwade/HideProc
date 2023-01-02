#include <wdm.h>
extern NTSTATUS HidProcByName(CHAR* ProcName);
extern NTSTATUS HidProcByInit();
VOID DriverUnload(PDRIVER_OBJECT pDriver) {
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("DriverUnloading...\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING RegPath) {
	UNREFERENCED_PARAMETER(RegPath);
	pDriver->DriverUnload = DriverUnload;
	_try{
	HidProcByName("dbgview.exe");
	}
		_except (EXCEPTION_EXECUTE_HANDLER) {
		DbgPrint("EXCEPTION_EXECUTE_HANDLER\n");
	}
	return STATUS_SUCCESS;
}
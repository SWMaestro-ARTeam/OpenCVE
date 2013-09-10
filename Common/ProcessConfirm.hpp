#ifndef _ProcessConfirm_hpp_
#define _ProcessConfirm_hpp_

#include "Common.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Process Confirm용 Windows Library.
#include <windows.h>
#include <TlHelp32.h>

class ProcessConfirm {
private:
#ifdef WINDOWS
	HANDLE _HProcess;
	PROCESSENTRY32 _PE32;
#endif

	bool FindProcess(char *szNameOfProcess);
#ifdef WINDOWS
	bool GetProcess(DWORD PID, char *ProcessName); // windows용 함수.
#endif
public:
	// Constructors
	ProcessConfirm();

	bool CheckAnotherEngineProcess(char *ProcessName);
};

#endif
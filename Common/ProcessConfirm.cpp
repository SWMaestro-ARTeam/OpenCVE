//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright © 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
//	{invi.dh.kim, munsp9103, aiaipming} at gmail.com
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of
//	this software and associated documentation files (the “Software”), to deal
//	in the Software without restriction, including without limitation the rights to
//	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
//	the Software, and to permit persons to whom the Software is furnished to do so,
//	subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
//	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
//	OR OTHER DEALINGS IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////////////////////

#include "ProcessConfirm.hpp"

ProcessConfirm *PC;

ProcessConfirm::ProcessConfirm() {
#if WINDOWS
	_HProcess = NULL;
	//_PE32 = {0};
#endif
	PC = this;
}

bool ProcessConfirm::GetProcess(DWORD PID, char *ProcessName) {
#if WINDOWS
	HANDLE _HModuleSnap = NULL; 
	MODULEENTRY32 _ME32 = {0};

	_HModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

	if (_HModuleSnap == (HANDLE)-1)
		return false;

	_ME32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(_HModuleSnap, &_ME32)) {
		do {
			// 똑같은 이름이 존재 할 때 True.
			if (strcmp((char *)_ME32.szModule, ProcessName) == 0) {
				CloseHandle(_HModuleSnap);
				return true;
			}
		} while (Module32Next(_HModuleSnap, &_ME32));
	}

	CloseHandle(_HModuleSnap);
#endif
	return false;
}

bool ProcessConfirm::CheckProcess(char *ProcessName){
#if WINDOWS
	_HProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	_PE32.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(_HProcess, &_PE32)) {
		bool _BCurrentStatus = false;
		MODULEENTRY32 _ME32 = {0};
		
		do {
			if (GetProcess(_PE32.th32ProcessID, ProcessName)) {
				if (OpenProcess(PROCESS_ALL_ACCESS, FALSE, _PE32.th32ProcessID)) {
					CloseHandle(_HProcess);
					return true;
				}
			}
		} while(Process32Next(_HProcess, &_PE32));
	}
	
	CloseHandle(_HProcess);
#endif
	return false;
}

UINT ExecProcessLoopThread(LPVOID Param) {
	char *_Str = (char *)Param;
	// Proess Active.
	PC->IsProcessActive = true;

	//while (1) {
	STARTUPINFO _StartUpInfo;
	PROCESS_INFORMATION _ProcessInfo;

	ZeroMemory(&_StartUpInfo, sizeof(_StartUpInfo));
	_StartUpInfo.cb = sizeof(_StartUpInfo);
	ZeroMemory(&_ProcessInfo, sizeof(_ProcessInfo));

	// Start the child process. 
	if (!CreateProcess(NULL, // No module name (use command line). 
		(LPWSTR)_Str, // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&_StartUpInfo,              // Pointer to STARTUPINFO structure.
		&_ProcessInfo)             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		PC->IsProcessActive = false;
		return 0;
	}

	// Wait until child process exits.
	// 기다리다가 프로세스가 죽으면 이 이후로 통과할 것이다.
	WaitForSingleObject(_ProcessInfo.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(_ProcessInfo.hProcess);
	CloseHandle(_ProcessInfo.hThread);
	//}
	// Process가 종료 되어 Handle이 없어졌다면, 최종적으로 Thread가 끝난다.
	PC->IsProcessActive = false;
	return 0;
}

void ProcessConfirm::CreateProcessOnThread(char *ProcessName) {
	AfxBeginThread(ExecProcessLoopThread, (char *)ProcessName);
}
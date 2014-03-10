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

#include "Process.hpp"

// only for Process Modules.
#define STRSAFE_LIB
#include <strsafe.h>

#if defined(WINDOWS_SYS)
#pragma region Windows Process Module
HMODULE WindowsProcess::NtDLLOpen() {
	CodeConverter _TCodeConverter;
	HMODULE _TNtDll = LoadLibrary(
		//#if MINGW_USING
		_TCodeConverter.CharToWChar("ntdll.dll")
		//#else
		//	_T("ntdll.dll")
		//#endif
		);
	if (_TNtDll == NULL)
		return NULL;

	gNtQueryInformationProcess =
		(fnNtQueryInformationProcess)GetProcAddress(_TNtDll, "NtQueryInformationProcess");

	if (gNtQueryInformationProcess == NULL) {
		FreeLibrary(_TNtDll);
		return NULL;
	}

	return _TNtDll;
}

void WindowsProcess::NtDLLRelease(HMODULE NtDllModuleHandle) {
	if (NtDllModuleHandle)
		FreeLibrary(NtDllModuleHandle);

	gNtQueryInformationProcess = (WindowsProcess::fnNtQueryInformationProcess)NULL;
}

bool WindowsProcess::CheckAbleProcessAccess(LPCTSTR PrivilegeName) {
	HANDLE _TToken = 0;
	TOKEN_PRIVILEGES _TTokenPrivileges = {0};
	//SE_DEBUG_NAME
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &_TToken))
		return false;

	if (LookupPrivilegeValue(NULL, PrivilegeName, &_TTokenPrivileges.Privileges[0].Luid)) {
		// Privilege Set.
		_TTokenPrivileges.PrivilegeCount = 1;
		// 특권 활성.
		_TTokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(_TToken, false, &_TTokenPrivileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
			return false;

		return true;
	}

	return false;
}

bool WindowsProcess::GetProcessInformations(const DWORD PID, SProcessInformations *ProcessInfo) {
	BOOL _TReturnStatus = TRUE;
	DWORD _TSize = 0;
	DWORD _TSizeNeeded = 0;
	DWORD _TBytesRead = 0;
	DWORD _TBufferSize = 0;
	HANDLE _THeap = 0;
	WCHAR *_TBuffer = NULL;

	SProcessInformations _TSPI = {0};
	SPProcessBasicInformations _TPBI = NULL;

	SPEB _TPEB = {0};
	SPEB_LDRData _TPEB_LDRData = {0};
	SRTLUserProcessParameters _TPEB_UPP	= {0};

	ZeroMemory(&_TSPI, sizeof(_TSPI));
	ZeroMemory(&_TPEB, sizeof(_TPEB));
	ZeroMemory(&_TPEB_LDRData, sizeof(_TPEB_LDRData));
	ZeroMemory(&_TPEB_UPP, sizeof(_TPEB_UPP));

	_TSPI.PID = PID;

	// Attempt to access process
	HANDLE _TProcess = OpenProcess(PROCESS_QUERY_INFORMATION | 
		PROCESS_VM_READ, FALSE, PID);
	if (_TProcess == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// Try to allocate buffer 
	_THeap = GetProcessHeap();

	_TSize = sizeof(SProcessBasicInformations);

	_TPBI = (SPProcessBasicInformations)HeapAlloc(_THeap,
		HEAP_ZERO_MEMORY,
		_TSize);
	// Did we successfully allocate memory
	if (!_TPBI) {
		CloseHandle(_TProcess);
		return FALSE;
	}

	// Attempt to get basic info on process
	NTSTATUS _TStatus = gNtQueryInformationProcess(_TProcess,
		ProcessBasicInformation,
		_TPBI,
		_TSize,
		&_TSizeNeeded);

	// If we had error and buffer was too small, try again
	// with larger buffer size (dwSizeNeeded)
	if (_TStatus >= 0 && _TSize < _TSizeNeeded) {
		if (_TPBI)
			HeapFree(_THeap, 0, _TPBI);
		_TPBI = (SPProcessBasicInformations)HeapAlloc(_THeap,
			HEAP_ZERO_MEMORY,
			_TSizeNeeded);
		if (!_TPBI) {
			CloseHandle(_TProcess);
			return FALSE;
		}

		_TStatus = gNtQueryInformationProcess(_TProcess,
			ProcessBasicInformation,
			_TPBI, _TSizeNeeded, &_TSizeNeeded);
	}

	// Did we successfully get basic info on process
	if (_TStatus >= 0)	{
		// Basic Info
		// spi.dwPID			 = (DWORD)pbi->UniqueProcessId;
		_TSPI.ParentPID		 = (DWORD)_TPBI->InheritedFromUniqueProcessId;
		_TSPI.BasePriority	 = (LONG)_TPBI->BasePriority;
		_TSPI.ExitStatus	 = (NTSTATUS)_TPBI->ExitStatus;
		_TSPI.PEBBaseAddress = (DWORD)_TPBI->PebBaseAddress;
		_TSPI.AffinityMask	 = (DWORD)_TPBI->AffinityMask;

		// Read Process Environment Block (PEB)
		if (_TPBI->PebBaseAddress) {
			if (ReadProcessMemory(_TProcess, _TPBI->PebBaseAddress, &_TPEB, sizeof(_TPEB), &_TBytesRead)) {
				_TSPI.SessionID	   = (DWORD)_TPEB.SessionId;
				_TSPI.BeingDebugged = (BYTE)_TPEB.BeingDebugged;

				// Here we could access PEB_LDR_DATA, i.e., module list for process
				//				dwBytesRead = 0;
				//				if (ReadProcessMemory(hProcess,
				//									 pbi->PebBaseAddress->Ldr,
				//									 &peb_ldr,
				//									 sizeof(peb_ldr),
				//									 &dwBytesRead))
				//				{
				// get ldr
				//				}

				// if PEB read, try to read Process Parameters
				_TBytesRead = 0;
				if (ReadProcessMemory(_TProcess,
					_TPEB.ProcessParameters,
					&_TPEB_UPP,
					sizeof(SRTLUserProcessParameters),
					&_TBytesRead)) {
						// We got Process Parameters, is CommandLine filled in
						if (_TPEB_UPP.CommandLine.Length > 0) {
							// Yes, try to read CommandLine
							_TBuffer = (WCHAR *)HeapAlloc(_THeap,
								HEAP_ZERO_MEMORY,
								_TPEB_UPP.CommandLine.Length);
							// If memory was allocated, continue
							if (_TBuffer)	{
								if (ReadProcessMemory(_TProcess,
									_TPEB_UPP.CommandLine.Buffer,
									_TBuffer,
									_TPEB_UPP.CommandLine.Length,
									&_TBytesRead)) {
										// if commandline is larger than our variable, truncate
										if (_TPEB_UPP.CommandLine.Length >= sizeof(_TSPI.CmdLine)) 
											_TBufferSize = sizeof(_TSPI.CmdLine) - sizeof(TCHAR);
										else
											_TBufferSize = _TPEB_UPP.CommandLine.Length;

										// Copy CommandLine to our structure variable
#if defined(UNICODE) || defined(_UNICODE)
										// Since core NT functions operate in Unicode
										// there is no conversion if application is
										// compiled for Unicode
										StringCbCopyN(_TSPI.CmdLine, sizeof(_TSPI.CmdLine),
											_TBuffer, _TBufferSize);
#else
										// Since core NT functions operate in Unicode
										// we must convert to Ansi since our application
										// is not compiled for Unicode
										WideCharToMultiByte(CP_ACP, 0, _TBuffer,
											(int)(_TBufferSize / sizeof(WCHAR)),
											_TSPI.CmdLine, sizeof(_TSPI.CmdLine),
											NULL, NULL);
#endif
								}
								if (!HeapFree(_THeap, 0, _TBuffer)) {
									// failed to free memory
									_TReturnStatus = FALSE;
									goto gnpiFreeMemFailed;
								}
							}
						}	// Read CommandLine in Process Parameters

						// We got Process Parameters, is ImagePath filled in
						if (_TPEB_UPP.ImagePathName.Length > 0) {
							// Yes, try to read ImagePath
							_TBytesRead = 0;
							_TBuffer = (WCHAR *)HeapAlloc(_THeap,
								HEAP_ZERO_MEMORY,
								_TPEB_UPP.ImagePathName.Length);
							if (_TBuffer) {
								if (ReadProcessMemory(_TProcess,
									_TPEB_UPP.ImagePathName.Buffer,
									_TBuffer,
									_TPEB_UPP.ImagePathName.Length,
									&_TBytesRead)) {
										// if ImagePath is larger than our variable, truncate
										if (_TPEB_UPP.ImagePathName.Length >= sizeof(_TSPI.ImgPath)) 
											_TBufferSize = sizeof(_TSPI.ImgPath) - sizeof(TCHAR);
										else
											_TBufferSize = _TPEB_UPP.ImagePathName.Length;

										// Copy ImagePath to our structure
#if defined(UNICODE) || defined(_UNICODE)
										StringCbCopyN(_TSPI.ImgPath, sizeof(_TSPI.ImgPath),
											_TBuffer, _TBufferSize);
#else
										WideCharToMultiByte(CP_ACP, 0, _TBuffer,
											(int)(_TBufferSize / sizeof(WCHAR)),
											_TSPI.ImgPath, sizeof(_TSPI.ImgPath),
											NULL, NULL);
#endif
								}
								if (!HeapFree(_THeap, 0, _TBuffer)) {
									// failed to free memory
									_TReturnStatus = FALSE;
									goto gnpiFreeMemFailed;
								}
							}
						}	// Read ImagePath in Process Parameters
				}	// Read Process Parameters
			}	// Read PEB 
		}	// Check for PEB

		// System process for WinXP and later is PID 4 and we cannot access
		// PEB, but we know it is aka ntoskrnl.exe so we will manually define it.
		// ntkrnlpa.exe if Physical Address Extension (PAE)
		// ntkrnlmp.exe if Symmetric MultiProcessing (SMP)
		// Actual filename is ntoskrnl.exe, but other name will be in
		// Original Filename field of version block.
		if (_TSPI.PID == 4) {
			CodeConverter _TCodeConverter;
			ExpandEnvironmentStrings(
				//#if MINGW_USING
				_TCodeConverter.CharToWChar("%SystemRoot%\\System32\\ntoskrnl.exe")
				//#else
				//			_T("%SystemRoot%\\System32\\ntoskrnl.exe")
				//#endif
				,
				_TSPI.ImgPath, sizeof(_TSPI.ImgPath));
		}
	}	// Read Basic Info

gnpiFreeMemFailed:

	// Free memory if allocated
	if (_TPBI != NULL)
		if (!HeapFree(_THeap, 0, _TPBI)) {
			// failed to free memory
		}

	CloseHandle(_TProcess);

	// Return filled in structure to caller
	*ProcessInfo = _TSPI;

	return _TReturnStatus;
}

DWORD WindowsProcess::GetProcessStatus(HANDLE ProcessPID) {
	DWORD _TExitCode = 0;
	::GetExitCodeProcess(ProcessPID, &_TExitCode);
	return _TExitCode;
}

void WindowsProcess::TerminateProcess(HANDLE ProcessPID) {
	::TerminateProcess(ProcessPID, 0);
}


#pragma endregion Windows Process Module
#elif defined(POSIX_SYS)
#pragma region POSIX Process Module
bool POSIXProcess::GetProcessInformations() {

}
#pragma endregion POSIX Process Module
#endif

#pragma region Process Module
Process *G_Process;

Process::Process() {
#if defined(WINDOWS_SYS)
	_ProcessHandle = NULL;
	_ModuleHandle = NULL;
	_PBI = NULL;
	_Size = 0;
	_Status = 0;
	//_PE32 = {0};
#elif defined(POSIX_SYS)
#endif
	G_Process = this;
}

Process::~Process() {
#if defined(WINDOWS_SYS)
	_ProcessHandle = NULL;
	_ModuleHandle = NULL;
	_PBI = NULL;
	_Size = 0;
	_Status = 0;
	//_PE32 = {0};
#elif defined(POSIX_SYS)

#endif
	G_Process = NULL;
}

bool Process::FindProcess(
#if defined(WINDOWS_SYS)
	DWORD
#elif defined(POSIX_SYS)
	unsigned long
#endif
	PID, char *ProcessName) {
#if defined(WINDOWS_SYS)
		HANDLE _TModuleSnap = NULL; 
		MODULEENTRY32 _TME32 = {0};

		_TModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

		if (_TModuleSnap == (HANDLE) - 1)
			return false;

		_TME32.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(_TModuleSnap, &_TME32)) {
			do {
				// 똑같은 이름이 존재 할 때 True.
				if (strcmp((char *)_TME32.szModule, ProcessName) == 0) {
					CloseHandle(_TModuleSnap);
					return true;
				}
			} while (Module32Next(_TModuleSnap, &_TME32));
		}

		CloseHandle(_TModuleSnap);
#elif defined(POSIX_SYS)

#endif
		return false;
}

#if defined(WINDOWS_SYS)
HANDLE
#elif defined(POSIX_SYS)
unsigned long
#endif
	Process::FindHandleGetOneProcess(
#if defined(WINDOWS_SYS)
	DWORD
#elif defined(POSIX_SYS)
	unsigned long
#endif
	PID) {
#if defined(WINDOWS_SYS)
		HANDLE _TModuleSnap = NULL; 
		MODULEENTRY32 _TME32 = {0};

		_TModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

		if (_TModuleSnap == (HANDLE) - 1)
			return false;

		_TME32.dwSize = sizeof(MODULEENTRY32);

		if (Process32First(_TModuleSnap, &_ProcessEntry32)) {
			MODULEENTRY32 _ME32 = {0};

			do {
				if (PID == _TME32.th32ModuleID) {
					HANDLE _TProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ProcessEntry32.th32ProcessID);
					CloseHandle(_TModuleSnap);
					return _TProcessHandle;
				}
			} while (Process32Next(_TModuleSnap, &_ProcessEntry32));
		}

		CloseHandle(_TModuleSnap);
#elif defined(POSIX_SYS)

#endif
		return false;
}

bool Process::CheckProcessExistByFileName(char *ProcessName) {
#if defined(WINDOWS_SYS)
	_ProcessHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	_ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(_ProcessHandle, &_ProcessEntry32)) {
		MODULEENTRY32 _ME32 = {0};

		do {
			if (FindProcess(_ProcessEntry32.th32ProcessID, ProcessName)) {
				if (OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ProcessEntry32.th32ProcessID)) {
					CloseHandle(_ProcessHandle);
					return true;
				}
			}
		} while (Process32Next(_ProcessHandle, &_ProcessEntry32));
	}

	CloseHandle(_ProcessHandle);
#elif defined(POSIX_SYS)

#endif
	return false;
}

int Process::CheckProcessExistByNumberOfExists(char *ProcessName) {
	int _TProcessNumber = 0;
#if defined(WINDOWS_SYS)
	_ProcessHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	_ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(_ProcessHandle, &_ProcessEntry32)) {
		MODULEENTRY32 _ME32 = {0};

		do {
			if (FindProcess(_ProcessEntry32.th32ProcessID, ProcessName)) {
				if (OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ProcessEntry32.th32ProcessID)) {
					CloseHandle(_ProcessHandle);
					_TProcessNumber++;
				}
			}
		} while (Process32Next(_ProcessHandle, &_ProcessEntry32));
	}

	CloseHandle(_ProcessHandle);
#elif defined(POSIX_SYS)

#endif
	return _TProcessNumber;
}

list<
#if defined(WINDOWS_SYS)
	SProcessInformations
#elif defined(POSIX_SYS)

#endif
> Process::GetProcessInformations() {
#if defined(WINDOWS_SYS)
	WindowsProcess 
#elif defined(POSIX_SYS)
	POSIXProcess
#endif
		_TProcess;
	list<
#if defined(WINDOWS_SYS)
		SProcessInformations
#elif defined(POSIX_SYS)

#endif
	> _TSProcessInformationsList;
	DWORD _TPIDs[PROCESS_MAX] = {0};
	DWORD _TArraySize = PROCESS_MAX * sizeof(DWORD);
	DWORD _TSizeNeeded = 0;
	DWORD _TPIDCount = 0;

	if (!_TProcess.CheckAbleProcessAccess(SE_DEBUG_NAME)) ;

	if (EnumProcesses((DWORD *)&_TPIDs, _TArraySize, &_TSizeNeeded)) {
		HMODULE _TNtDll = _TProcess.NtDLLOpen();
		if (_TNtDll) {
			_TPIDCount = _TSizeNeeded / sizeof(DWORD);
			for (DWORD _ProcessCount = 0;
				_ProcessCount < PROCESS_MAX && _ProcessCount < _TPIDCount; _ProcessCount++) {
					_TProcess.GetProcessInformations(_TPIDs[_ProcessCount], &__SProcessInformation[_ProcessCount]);
					_TSProcessInformationsList.push_back(__SProcessInformation[_ProcessCount]);
			}
			_TProcess.NtDLLRelease(_TNtDll);
		}
	}

	return _TSProcessInformationsList;
}

#pragma region Exec Process thread
// Exec Process thread
#if defined(WINDOWS_SYS)
UINT WINAPI
	//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	Process::ExecProcessLoopThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
		char *_TStr = (char *)Param;
		CodeConverter _TCodeConverter;

		//while (1) {
#if defined(WINDOWS_SYS)
		STARTUPINFO _TStartUpInfo;
		PROCESS_INFORMATION _TProcessInfo;

		ZeroMemory(&_TStartUpInfo, sizeof(_TStartUpInfo));
		_TStartUpInfo.cb = sizeof(_TStartUpInfo);
		ZeroMemory(&_TProcessInfo, sizeof(_TProcessInfo));
#elif defined(POSIX_SYS)

#endif
		// Start the child process. 
		if (
#if defined(WINDOWS_SYS)
			!CreateProcess(
			NULL, // No module name (use command line).
#if defined(UNICODE) || defined(_UNICODE)
			(LPWSTR)_TCodeConverter.CharToWChar(_TStr), // Command line.
#else
			(LPCSTR)_TStr
#endif
			NULL, // Process handle not inheritable. 
			NULL, // Thread handle not inheritable. 
			FALSE, // Set handle inheritance to FALSE. 
			CREATE_NO_WINDOW, // No creation flags. Default 0.
			NULL, // Use parent's environment block. 
			NULL, // Use parent's starting directory. 
			&_TStartUpInfo, // Pointer to STARTUPINFO structure.
			&_TProcessInfo // Pointer to PROCESS_INFORMATION structure.
			)
#elif defined(POSIX_SYS)

#endif
			) {
				G_Process->IsProcessActive = false;
				return 0;
		}
		// Proess Active.
		G_Process->IsProcessActive = true;

		// Wait until child process exits.
		// 무한정 기다리다가 프로세스가 죽으면 이 이후로 통과할 것이다.
#if defined(WINDOWS_SYS)
		// Process가 죽을때까지 기다린다.
		WaitForSingleObject(_TProcessInfo.hProcess, INFINITE);
#elif defined(POSIX_SYS)
		// pthread_join(_Thread, NULL);
#endif

#if defined(WINDOWS_SYS)
		// Close process and thread handles. 
		CloseHandle(_TProcessInfo.hProcess);
		CloseHandle(_TProcessInfo.hThread);
#elif defined(POSIX_SYS)
		// No handle in POSIX Thread.
#endif
		//}
		// Process가 종료 되어 Handle이 없어졌다면, 최종적으로 Thread가 끝난다.
		G_Process->IsProcessActive = false;

#if defined(WINDOWS_SYS)
		_endthread();
#elif defined(POSIX_SYS)

#endif
		return 0;
}
#pragma endregion Exec Process thread

void Process::CreateProcessOnThread(char *ProcessName) {
	_Thread.StartThread(ExecProcessLoopThread, (LPVOID)ProcessName);
	/*
#if defined(WINDOWS_SYS)
	//#ifdef _AFXDLL
	//DWORD _TThreadID = 0;
	//CreateThread(NULL, 0, ExecProcessLoopThread, (LPVOID)ProcessName, 0, &_TThreadID);
	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ExecProcessLoopThread, (LPVOID)ProcessName, 0, NULL);
	//#endif
#elif defined(POSIX_SYS)
	pthread_t _TThread;
	pthread_attr_t _TThreadAttr;
	// pthread attribute initialize.
	pthread_attr_init(&_TThreadAttr);
	// Detached thread.
	pthread_attr_setdetachstate(&_TThreadAttr, PTHREAD_CREATE_DETACHED);
	// User space thread.
	pthread_attr_setscope(&_TThreadAttr, PTHREAD_SCOPE_SYSTEM);
	// Create thread.
	pthread_create(&_TThread, NULL, ExecProcessLoopThread, (void *)ProcessName);
#endif
	*/
}

#if defined(WINDOWS_SYS)
DWORD
#elif defined(POSIX_SYS)
unsigned long
#endif
	Process::GetProcessStatus(
#if	defined(WINDOWS_SYS)
	HANDLE
#elif defined(POSIX_SYS)

#endif
	ProcessHandle) {
#if defined(WINDOWS_SYS)
		WindowsProcess 
#elif defined(POSIX_SYS)
		POSIXProcess
#endif
			_TProcess;
		return _TProcess.GetProcessStatus(ProcessHandle);
}

void Process::TerminateProcess(
#if defined(WINDOWS_SYS)
	HANDLE
#elif defined(POSIX_SYS)

#endif
	ProcessHandle) {
#if defined(WINDOWS_SYS)
		WindowsProcess 
#elif defined(POSIX_SYS)
		POSIXProcess
#endif
			_TProcess;
		_TProcess.TerminateProcess(ProcessHandle);
}

HANDLE Process::FindProcessByPID(DWORD ProcessPID) {
	return FindHandleGetOneProcess(ProcessPID);
}

#pragma endregion Process Module

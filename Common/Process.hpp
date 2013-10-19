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

#ifndef _Process_hpp_
#define _Process_hpp_

#include "Common.hpp"
//#include "SystemDependency.hpp"
#include "CodeConverter.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Process Confirm용 Windows Library.
#if WINDOWS_SYS
#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <windows.h>
#endif
// using WindowsProcess class.
#include <winternl.h>
#include <Psapi.h>
#define STRSAFE_LIB
#include <strsafe.h>

#pragma comment(lib, "strsafe.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "psapi.lib")

#include <TlHelp32.h>
#elif POSIX_SYS

#include <pthread.h>
#include <unistd.h>
#endif

#if WINDOWS_SYS
// using PEB Structure.
typedef struct _PEB_LDRData {
	BYTE Reserved1[8];
	PVOID Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} SPEB_LDRData, *SPPEB_LDRData;

typedef struct _RTLUserProcessParameters {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} SRTLUserProcessParameters, *SPRTLUserProcessParameters;

typedef struct _PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
	SPRTLUserProcessParameters ProcessParameters;
	BYTE Reserved4[104];
	PVOID Reserved5[52];
	ULONG PostProcessInitRoutine;
	BYTE Reserved6[128];
	PVOID Reserved7[1];
	ULONG SessionId;
} SPEB, *SPPEB;

typedef struct _ProcessBasicInformations {
	LONG ExitStatus;
	SPPEB PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} SProcessBasicInformations, *SPProcessBasicInformations;

typedef struct _ProcessInformations {
	DWORD	PID;
	DWORD	ParentPID;
	DWORD	dwessionID;
	DWORD	PEBBaseAddress;
	DWORD	AffinityMask;
	LONG	BasePriority;
	LONG	ExitStatus;
	BYTE	BeingDebugged;
	TCHAR	ImgPath[BUFFER_MAX_32767];
	TCHAR	CmdLine[BUFFER_MAX_32767];
} SProcessInformations;
#elif POSIX_SYS

#endif

#if WINDOWS_SYS
class WindowsProcess {
private:

public:
	//NTSTATUS(NTAPI* fnNtQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
	typedef NTSTATUS(NTAPI* fnNtQueryInformationProcess)(
		IN HANDLE ProcessHandle,
		IN PROCESSINFOCLASS ProcessInfomationClass,
		OUT PVOID ProcessInformation,
		IN ULONG ProcessInformationLength,
		OUT PULONG ReturnLength OPTIONAL
		);

	fnNtQueryInformationProcess gNtQueryInformationProcess;

	// Open & Release
	HMODULE NtDLLOpen();
	void NtDLLRelease(HMODULE NtDllModuleHandle);

	// check Enabled Access Token.
	bool CheckAbleProcessAccess(LPCTSTR PrivilegeName);

	// Get Process Information
	bool GetProcessInformations(const DWORD PID, SProcessInformations *ProcessInfo);
};
#elif POSIX_SYS
class POSIXProcess {
private:
public:
};
#endif

class Process :
#if WINDOWS_SYS
	WindowsProcess
#elif POSIX_SYS
	POSIXProcess
#endif
{
private:
#if WINDOWS_SYS
	// Windows에서의 Process를 위한 Handle, Process Entry.
	HANDLE _ProcessHandle;
	HMODULE _ModuleHandle;
	PROCESSENTRY32 _ProcessEntry32;
	PROCESS_BASIC_INFORMATION *_PBI;
	DWORD _Size;
	NTSTATUS _Status;
#elif POSIX_SYS

#endif

	//bool FindProcess(char *NameOfProcess);

	bool FindProcess(
#if WINDOWS_SYS
		DWORD 
#elif POSIX_SYS
		unsigned long
#endif
		PID, char *ProcessName); // windows용 함수.
public:
	// Constructor
	Process();
	// Destructor
	~Process();

	bool IsProcessActive;

	bool CheckFileExist(char *ProcessName);

	bool CheckProcessExist(char *ProcessName);
	int CheckProcessExistByNumberOfExists(char *ProcessName);

	void GetProcessInformations(char *ProcessName, DWORD PID);
	void CreateProcessOnThread(char *ProcessName);
};

#endif
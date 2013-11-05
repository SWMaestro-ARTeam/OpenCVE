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

#include "CodeConverter.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <list>

using namespace std;

// Process Confirm용 Windows Library.
#if WINDOWS_SYS
//#		ifdef _AFXDLL
//#		include <afxwin.h>
//#		else
#		include <windows.h>
#		include <process.h>
#		include <tchar.h>
//#		endif
// using WindowsProcess class.
#include <winternl.h>
#include <Psapi.h>
//#include <processthreadsapi.h>
//#include <securitybaseapi.h>

#ifndef NTSTATUS
#define LONG NTSTATUS
#endif

#pragma comment(lib, "strsafe.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "psapi.lib")

#include <TlHelp32.h>
#elif POSIX_SYS

#include <pthread.h>
#include <unistd.h>
#endif

#if WINDOWS_SYS
typedef ULONG PPS_PostProcessInitRoutine;
// using PEB Structure.
// Used in PEB struct
typedef struct _SPEB_LDRData {
	BYTE Reserved1[8];
	PVOID Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} SPEB_LDRData, *SPPEB_LDRData;

// Used in PEB struct
typedef struct _SRTLUserProcessParameters {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} SRTLUserProcessParameters, *SPRTLUserProcessParameters;

// Used in PROCESS_BASIC_INFORMATION struct
typedef struct _SPEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	SPPEB_LDRData Ldr;
	SPRTLUserProcessParameters ProcessParameters;
	BYTE Reserved4[104];
	PVOID Reserved5[52];
	PPS_PostProcessInitRoutine PostProcessInitRoutine;
	BYTE Reserved6[128];
	PVOID Reserved7[1];
	ULONG SessionId;
} SPEB, *SPPEB;

typedef struct _SProcessBasicInformations {
	LONG ExitStatus;
	SPPEB PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} SProcessBasicInformations, *SPProcessBasicInformations;

typedef struct _SProcessInformations {
	DWORD	PID;
	DWORD	ParentPID;
	DWORD	SessionID;
	DWORD	PEBBaseAddress;
	DWORD	AffinityMask;
	LONG	BasePriority;
	LONG	ExitStatus;
	BYTE	BeingDebugged;
	TCHAR	ImgPath[BUFFER_MAX_32767];
	TCHAR	CmdLine[BUFFER_MAX_32767];
} SProcessInformations;

static SProcessInformations __SProcessInformation[1024] = {0};
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

	DWORD GetProcessStatus(HANDLE ProcessPID);
	void TerminateProcess(HANDLE ProcessPID);
};
#elif POSIX_SYS
class POSIXProcess {
private:
public:
	bool GetProcessInformations();
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

	//
	bool FindProcess(
#if WINDOWS_SYS
		DWORD 
#elif POSIX_SYS
		unsigned long
#endif
		PID, char *ProcessName);

#if WINDOWS_SYS
	HANDLE
#elif POSIX_SYS
	unsigned long
#endif
		FindHandleGetOneProcess(
#if WINDOWS_SYS
		DWORD 
#elif POSIX_SYS
		unsigned long
#endif
		PID);

	// ExecProcessLoopThread
	static
#if WINDOWS_SYS
		UINT WINAPI
		//DWORD WINAPI
#elif POSIX_SYS
// using pthread
		void *
#endif
		ExecProcessLoopThread(
#if WINDOWS_SYS
		LPVOID
#elif POSIX_SYS
		void *
#endif
		Param);
public:
	// Constructor
	Process();
	// Destructor
	~Process();

	bool IsProcessActive;

	bool CheckFileExist(char *ProcessName);

	bool CheckProcessExistByFileName(char *ProcessName);
	int CheckProcessExistByNumberOfExists(char *ProcessName);

	list<SProcessInformations> GetProcessInformations();

	void CreateProcessOnThread(char *ProcessName);
	HANDLE FindProcessByPID(DWORD ProcessPID);

#if WINDOWS_SYS
	DWORD
#elif POSIX_SYS
	unsigned long
#endif
		GetProcessStatus(
#if WINDOWS_SYS
		HANDLE
#elif POSIX_SYS
#endif
		ProcessHandle);

	void TerminateProcess(
#if WINDOWS_SYS
		HANDLE
#elif POSIX_SYS
#endif
		ProcessHandle);
};

#endif

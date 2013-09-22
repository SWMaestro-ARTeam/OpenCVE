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

#ifndef _ProcessConfirm_hpp_
#define _ProcessConfirm_hpp_

#include "Common.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Process Confirm용 Windows Library.
#if WINDOWS
#include <windows.h>
#include <TlHelp32.h>
#elif OTHER
#endif

class ProcessConfirm {
private:
#if WINDOWS
	// Windows에서의 Process를 위한 Handle, Process Entry.
	HANDLE _HProcess;
	PROCESSENTRY32 _PE32;
#elif OTHER
#endif

	bool FindProcess(char *szNameOfProcess);
#if WINDOWS
	bool GetProcess(DWORD PID, char *ProcessName); // windows용 함수.
#elif OTHER
#endif
public:
	// Constructors
	ProcessConfirm();

	bool IsProcessActive;

	bool CheckProcess(char *ProcessName);
	void CreateProcessOnThread(char *ProcessName);
};

#endif
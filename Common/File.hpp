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

#ifndef _File_hpp_
#define _File_hpp_

#include "SystemDependency.hpp"
//#define WINDOWS 1
//#include "Common.hpp"
#include "CodeConverter.hpp"

#include <stdio.h>
#include <string.h>
#if WINDOWS_SYS
//	#ifdef _AFXDLL
//#include <afxwin.h>
//	#else
#include <windows.h>
	//#endif
#else
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#endif

class File {
private:
public:
	char *GetCurrentPath();
	char *GetCurrentFullPath();
	char *GetCurrentProcessName();

	bool CheckFileExist(char *FileName);
};
#endif
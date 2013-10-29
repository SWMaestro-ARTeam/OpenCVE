//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright © 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
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

#ifndef _SystemDependency_hpp_
#define _SystemDependency_hpp_

#if defined(_WIN32) || defined(_WIN64)
// using windows.
#define WINDOWS_SYS 1
	#if defined(__MINGW32__)
	// using MinGW(for windows).
	#define MINGW_USING 1
	#endif
#else
// using Linux, FreeBSD, Mac OSX.
#define POSIX_SYS 1
#endif

// If defined Debug mode by compiler(M$ Visual Studio & Qt Creator).
#if defined(_DEBUG) || !defined(QT_NO_DEBUG)
//#define DEBUG_MODE
#endif

#define ZERO_ 0
//#undef NULL
//#define NULL ((void *)0)
#if MINGW_USING
#define NULL 0
#define NULL_ __null
#define NOTNULL_ __notnull
#define MAYBENULL_ __maybenull
#endif

// Buffer Size Defines.
#define BUFFER_MAX_16 16
#define BUFFER_MAX_32 32
#define BUFFER_MAX_128 128
#define BUFFER_MAX_1024 1024
#define BUFFER_MAX_2048 (BUFFER_MAX_1024 * 2)
#define BUFFER_MAX_4096 (BUFFER_MAX_1024 * 4)
#define BUFFER_MAX_32767 ((BUFFER_MAX_1024 * 32) - 1)
#define BUFFER_MAX_65535 ((BUFFER_MAX_1024 * 64) - 1)
 
#if WINDOWS_SYS
// using MFC.
#if !MINGW_USING
#define _AFXDLL
#endif
// or no MFC.

// Maximum Process Limit.
#define PROCESS_MAX 32767
#endif

#endif

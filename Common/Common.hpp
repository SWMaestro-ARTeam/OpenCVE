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

#ifndef _Common_hpp_
#define _Common_hpp_

#include "SystemDependency.hpp"

// File 처리를 위한 값.
#if WINDOWS_SYS
#define SERVER_ENGINE_EXEC_FILENAME "CVES.exe"
#define CLIENT_ENGINE_EXEC_FILENAME "CVEC.exe"
#define CELESTIALS_EXEC_FILENAME "Celestials.exe"
#elif POSIX_SYS
#define SERVER_ENGINE_EXEC_FILENAME "CVES"
#define CLIENT_ENGINE_EXEC_FILENAME "CVEC"
#define CELESTIALS_EXEC_FILENAME "Celestials"
#endif

#define ENGINE_EXEC_VER "0.1.0"

// 통신을 위한 값
#define CVE_PORT 10080
#define LISTEN_QUEUE 10
#define IP_ADDR_LOCAL "127.0.0.1"

// 영상을 위한 값
#define SERVER_VIEW_DEFAULT_WIDTH 640
#define SERVER_VIEW_DEFAULT_HEIGHT 480
#define FLIP_MODE -1

// Algorithm 값.
#define RECOGNITION_MODE 1 // 1 : hough Line, 2 : 규홍 recognition

#define MAX_CORNER 5000

#define MOP_NUM 2
#define T_SKIN_NUM 10
#define SUB_THRESHOLD 10

#define _DEF_MAX_BLOBS 10000
#define _DEF_MAX_LABEL 100

// Thread를 위한 값
#define H_THREAD 12

#endif
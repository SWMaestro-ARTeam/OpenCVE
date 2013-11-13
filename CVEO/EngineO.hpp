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

#ifndef _EngineO_hpp_
#define _EngineO_hpp_

#include <cstdio>
#include <cstdlib>

#include <list>
#include <queue>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 공통 상수 정의
#include "Common.hpp"
// 공용 변수 정의
#include "GlobalVariables.hpp"
// Internal Protocol Seeker
#include "InternalProtocolSeeker.hpp"
// Process Confirm Module
#include "Process.hpp"
// String Tokenizer
#include "StringTokenizer.hpp"
// Telepathy Module
#include "Telepathy.hpp"
// String Tools
#include "StringTools.hpp"

#include "Time.hpp"

class EngineO {
private:
	Telepathy::Client *_TelepathyClient; // CVES 통신용.

	InternalProtocolSeeker _InternalProtocolSeeker;
	StringTools _StringTools;

	bool Initialize_TClient();
	void Deinitialize_TClient();

	void Engine_Initializing();
	void Engine_DeInitializing();

	void Clear_ClientSocket();

	// for Server Connect.
	bool Connect_Server();
	void Disconnect_Server();
	bool Reconnect_Server();

	// ClientReceivedCallback
	static void ObserverReceivedCallback(char *Buffer);
	static void ObserverDisconnectedCallback();

	static
#if defined(WINDOWS_SYS)
		UINT WINAPI
		//DWORD WINAPI
#elif defined(POSIX_SYS)
		// using pthread
		void *
#endif
		ObserverCommandQueueProcessingThread(
#if defined(WINDOWS_SYS)
		LPVOID
#elif defined(POSIX_SYS)
		void *
#endif
		Param);

	static
#if defined(WINDOWS_SYS)
		UINT WINAPI
		//DWORD WINAPI
#elif defined(POSIX_SYS)
		// using pthread
		void *
#endif
		EngineO::CVEOProcessingThread(
#if defined(WINDOWS_SYS)
		LPVOID
#elif defined(POSIX_SYS)
		void *
#endif
		Param);

public:
	// Constructor
	EngineO();
	~EngineO();

	bool EngineEnable;

	queue<char *> *CommandQueue;

	void EngineO_Start();
};

#endif // _EngineO_hpp_

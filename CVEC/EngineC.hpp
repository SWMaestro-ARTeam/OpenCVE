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

#ifndef _EngineC_hpp_
#define _EngineC_hpp_

#include <cstdio>
#include <cstdlib>

#include <list>
#include <mutex>
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
// File Utility
#include "File.hpp"
// UCI Command Seeker
#include "UCICommandSeeker.hpp"
// String Tokenizer
#include "StringTokenizer.hpp"
// Telepathy Module
#include "Telepathy.hpp"
// Option Module
#include "Option.hpp"
// String Tools
#include "StringTools.hpp"

#include "Time.hpp"

class EngineC {
private:
	Option *_Option;
	Telepathy::Client *_TelepathyClient;
	Process *_ProcessConfirm;
	UCICommandSeeker _UCICommandSeeker;
	File _File;

	// Variables
	bool _IsGetCVESProcess;
	bool _IsNoCVESProcess;
	bool _IsCVESReady;

	DWORD _ServerPID;

	mutex _QueueProtectMutex;
	mutex _VarProtectMutex;
	
	char *_Command;
	// 내가 흑색인지 백색인지 구분.
	bool _IsWhite;
	bool _IsInAI;

	// Functions
	void Initialize_CommandStr();
	void Deinitialize_CommandStr();
	
	void Initialize_CVEOption();
	void Deinitialize_CVEOption();

	bool Initialize_TClient();
	void Deinitialize_TClient();

	void Initialize_ProcessConfirm();
	void Deinitialize_ProcessConfirm();

	void Put_Author();

	void Engine_Initializing();
	void Engine_DeInitializing();

	void Get_Command_Str();
	void Clear_Str();

	void Clear_ClientSocket();

	// for Server Connect.
	bool Connect_Server();
	void Disconnect_Server();
	bool Reconnect_Server();

	// Command Functions
	void Command_UCI();
	void Command_Debug();
	void Command_Isready();
	void Command_Setoption(CommandString *_UCICS); //
	void Command_Ucinewgame();
	void Command_Register();
	void Command_Position(CommandString *_UCICS); //
	void Command_Go(CommandString *_UCICS); //
	void Command_Stop();
	void Command_Ponderhit();
	void Command_Quit();

	void Parsing_Command();

	void SendToGUI(const char *Str, ...);
	bool CheckingCVESProcess();

	// ClientReceivedCallback
	static void ClientReceivedCallback(char *Buffer);
	static void ClientDisconnectedCallback();

	// CVEC_CVESCheckingThread
	static
#if WINDOWS_SYS
		UINT WINAPI
		//DWORD WINAPI
#elif POSIX_SYS
		void *
#endif
		CVEC_CVESCheckingThread(
#if WINDOWS_SYS
		LPVOID
#elif POSIX_SYS
		void *
#endif
		Param);

	// ClientCommandQueueProcessingThread
	static
#if WINDOWS_SYS
		UINT WINAPI
		//DWORD WINAPI
#elif POSIX_SYS
		// using pthread
		void *
#endif
		ClientCommandQueueProcessingThread(
#if WINDOWS_SYS
		LPVOID
#elif POSIX_SYS
		void *
#endif
		Param);
	
public:
	// Constructor
	EngineC();
	~EngineC();

	bool EngineEnable;
	bool EnginePause;

	queue<char *> *CommandQueue;

	void EngineC_Start();
};
#endif
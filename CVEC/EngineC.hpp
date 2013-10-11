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

#include <string.h>
#include <stdlib.h>

// 공통 상수 정의
#include "Common.hpp"
// 공용 변수 정의
#include "GlobalVariables.hpp"
// Internal Protocol Seeker
#include "InternalProtocolSeeker.hpp"
// Process Confirm Module
#include "ProcessConfirm.hpp"
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

class EngineC {
private:
	// Variables
	char *Command_Str;

	Option *_Option;
	Telepathy::Client *_TelepathyClient;
	ProcessConfirm *_ProcessConfirm;
	UCICommandSeeker _UCICommandSeeker;
	File _File;

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

	void Command_UCI();
	void Command_Debug();
	void Command_Isready();
	void Command_Setoption(CS *_UCICS); //
	//void Command_Setoption(); //
	void Command_Ucinewgame();
	void Command_Register();
	void Command_Position(CS *_UCICS); //
	//void Command_Position(); //
	void Command_Go(CS *_UCICS); //
	//void Command_Go(); //
	void Command_Stop();
	void Command_Ponderhit();
	void Command_Quit();

	void Parsing_Command();

public:
	// Constructor
	EngineC();
	~EngineC();

	bool CVEC_CVESControlInitial;
	//bool IsCVESProcessAlive;
	//bool IsSocketInitialize;
	//bool IsSocketConnented;
	//bool IsGetCVESProcess;
	//bool isServerOrClient;
	bool EngineEnable;
	bool EnginePause;
	// Functions
	bool Connect_Server();
	void Disconnect_Server();

	void SendToGUI(const char *Str, ...);

	bool Get_CVESProcessStatus();
	bool Get_CVESConnectionStatus();
	Telepathy::Client *Get_TelepathyClient();
	bool CheckingCVESProcess();
	void EngineC_Start();
};

void ClientReceivedCallback(char *Buffer);

#endif
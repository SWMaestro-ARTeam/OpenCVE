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

// 공통 상수 정의
#include "Common.hpp"

// 기본 Header
#include "UCIParser.hpp"

// Callback을 위한 Menber.
UCIParser G_UCIParser;

UCIParser::UCIParser() {
	_IsSocketConnented = false;
	initializing();
}

UCIParser::~UCIParser() {
	_IsSocketConnented = false;
	Clear_Str();
}

void UCIParser::init_CommandStr() {
	// Command는 BUFFER_MAX값에 의해서 결정
	Command_Str = (char *)calloc(BUFFER_MAX_32767, sizeof(char));
	Clear_Str();
}

bool UCIParser::Init_ClientSocket() {
	_TClient = new Telepathy::Client();
	SendToGUI("Wait for Server Request.");
	// Inititalizing Client.
	if (_TClient->ClientInitialize() == true) {
		// Server 연결 성공시에, ClientReceivedCallback을 묶어
		// Receive 할 때 Server에서 전송된 내용을 받아야 한다.
		_TClient->TClientReceivedCallback = ClientReceivedCallback;
	}	
	else {
		// Server 연결 실패.
		SendToGUI("Server connection Failed.");
		return false;
	}

	// 연결 성립.
	SendToGUI("Connected.\n");
	return true;
}

void UCIParser::Put_Author() {
	// 4 Parser Engine Start.
	SendToGUI("OpenCVE Connector Ver %s Start.",  ENGINE_EXEC_VER);
	SendToGUI("Engine Copyright by ARTeam.");
	/*
	printf("OpenCVE Connector Ver %s Start.\n",  ENGINE_EXEC_VER);
	printf("Engine Copyright by ARTeam.\n");
	*/
}

void UCIParser::Get_Command_Str() {
	if (fgets(Command_Str, BUFFER_MAX_32767, stdin) == NULL) {
		if (feof(stdin)) {
			return ;
		}
	}

	char *_StrPtr = strchr(Command_Str,'\n');
	if (_StrPtr != NULL) *_StrPtr = '\0';
}

void UCIParser::Clear_Str() {
	memset(Command_Str, NULL, sizeof(Command_Str));
}

void UCIParser::Clear_ClientSocket() {
	if (_TClient != NULL)
		delete _TClient;
}

void UCIParser::SendToGUI(const char *Str, ...) {
	va_list _Argument_List;
	char _Str[4096];

	va_start(_Argument_List, Str);
	vsprintf(_Str, Str, _Argument_List);
	va_end(_Argument_List);

	fprintf(stdout, "%s\n", _Str);
}

void UCIParser::Command_UCI() {
	// 현재 Name, 저작자 보냄.
	SendToGUI("id name OpenCVE " ENGINE_EXEC_VER);
	SendToGUI("id author SW Maestro AR Team");

	// Print Options(no option)

	// send "uciok"
	SendToGUI("uciok");
}

void UCIParser::Command_Debug() {
	// No Implementation
}

void UCIParser::Command_Isready() {
	SendToGUI("readyok");
}

void UCIParser::Command_Setoption() {
	printf("4\n");
}

void UCIParser::Command_Ucinewgame() {
	printf("5\n");
}

void UCIParser::Command_Register() {
	// No Implementation
}

//
void UCIParser::Command_Position() {
	printf("7\n");
}

//
void UCIParser::Command_Go() {
	printf("8\n");
}

void UCIParser::Command_Stop() {
	// 정보를 굳이 저장하지 않는다면, 이걸 할 이유가 있을까?
	printf("9\n");
}

void UCIParser::Command_Ponderhit() {
	printf("10\n");
}

void UCIParser::Command_Quit() {
	exit(EXIT_SUCCESS);
}

void UCIParser::Parsing_Command() {
	UCICommandParser _UCICommandParser;

	// Get UCI String.
	Get_Command_Str();
	int _NSeek_GUIToEngine = _UCICommandParser.UCIString_Seeker(Command_Str);

	switch (_NSeek_GUIToEngine) {
		case VALUE_UCI :
			Command_UCI();
			break;
		case VALUE_DEBUG :
			Command_Debug();
			break;
		case VALUE_ISREADY :
			Command_Isready();
			break;
		case VALUE_SETOPTION :
			Command_Setoption();
			break;
		case VALUE_UCINEWGAME :
			Command_Ucinewgame();
			break;
		case VALUE_REGISTER :
			Command_Register();
			break;
		case VALUE_POSITION :
			Command_Position();
			break;
		case VALUE_GO :
			Command_Go();
			break;
		case VALUE_STOP :
			Command_Stop();
			break;
		case VALUE_PONDERHIT :
			Command_Ponderhit();
			break;
		case VALUE_QUIT :
			Command_Quit();
			break;
	}
	// Clear UCI String.
	Clear_Str();
}

void UCIParser::initializing() {
	// Put Author string.
	Put_Author();
	
	// Initialize string Buffer.
	init_CommandStr();
	
	// Initialize Client Socket.
	if (Init_ClientSocket() != true)
		SendToGUI("Socket Initialize Failed.");
}


void UCIParser::Parsing_Engine_Start() {
	while (1) Parsing_Command();
}

void ClientReceivedCallback(char *Buffer){
	
}
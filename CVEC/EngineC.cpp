﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

// 기본 Header
#include "EngineC.hpp"

// Callback을 위한 Menber.
EngineC *G_EngineC;

#pragma region Constructor & Destructor
// Constructor
EngineC::EngineC() {
	//IsSocketInitialize = false;
	//IsSocketConnented = false;
	IsGetCVESProcess = false;
	IsNoCVESProcess = false;
	EngineEnable = false;
	EnginePause = false;
	CVEC_CVESControlInitial = true;
	G_EngineC = this;
}

// Destructor
EngineC::~EngineC() {
	//IsSocketInitialize = false;
	//IsSocketConnented = false;
	IsGetCVESProcess = false;
	IsNoCVESProcess = false;
	EngineEnable = false;
	EnginePause = false;
	CVEC_CVESControlInitial = false;
	G_EngineC = NULL;
}
#pragma endregion Constructor & Destructor

#pragma region Initialize & Deinitialize Functions
void EngineC::Initialize_CommandStr() {
	// Command는 BUFFER_MAX값에 의해서 결정
	Command_Str = (char *)calloc(BUFFER_MAX_32767, sizeof(char));
	Clear_Str();
}

void EngineC::Deinitialize_CommandStr() {
	Clear_Str();
	free(Command_Str);
}

void EngineC::Initialize_CVEOption() {
	_Option = new Option();
}

void EngineC::Deinitialize_CVEOption() {
	if (_Option != NULL)
		delete _Option;
}

bool EngineC::Initialize_TClient() {
	_TelepathyClient = new Telepathy::Client();

	if (_TelepathyClient->ClientInitialize() != true) {
		// Server 연결 실패.
		SendToGUI("Client Initialize Failed.");
		return false;
	}

	return true;
}

void EngineC::Deinitialize_TClient() {
	if (_TelepathyClient != NULL)
		delete _TelepathyClient;
}

void EngineC::Initialize_ProcessConfirm() {
	_ProcessConfirm = new Process();
}

void EngineC::Deinitialize_ProcessConfirm() {
	if (_ProcessConfirm != NULL)
		delete _ProcessConfirm;
}

void EngineC::Put_Author() {
	// 4 Parser Engine Start.
	SendToGUI("OpenCVE Client Engine Version %s.", ENGINE_EXEC_VER);
	SendToGUI("{Doohoon Kim, Sungpil Moon, Kyuhong Choi} Copyright All right reserved.");
	/*
	// in ARTeam of SW Maestro 4th.
	printf("OpenCVE Connector Ver %s Start.\n",  ENGINE_EXEC_VER);
	printf("Engine Copyright by ARTeam.\n");
	*/
}

void EngineC::Engine_Initializing() {
	// Defect #24
	// _IOLBF가 Windows에서는 깨지는 난감한 현상이 생김.
	// 이 구문은 본 Engine의 Command Line에서 반드시 써야 하는 중요한 구문.
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	// Put Author string.
	Put_Author();

	// Initialize string Buffer.
	Initialize_CommandStr();
	// Initialize CVEOptions.
	Initialize_CVEOption();
	// Initialize ProcessConfirm.
	Initialize_ProcessConfirm();
	
	// Initialize Client Socket.
	if (Initialize_TClient() != true)
		SendToGUI("Socket Initialize Failed.");
	
}

void EngineC::Engine_DeInitializing() {
	Deinitialize_CommandStr();
	Deinitialize_CVEOption();
	Deinitialize_ProcessConfirm();
}
#pragma endregion Initialize & Deinitialize Functions

void EngineC::Get_Command_Str() {
	if (fgets(Command_Str, BUFFER_MAX_32767, stdin) == NULL) {
		if (feof(stdin)) {
			return ;
		}
	}

	char *_StrPtr = strchr(Command_Str,'\n');

	if (_StrPtr != NULL)
		*_StrPtr = '\0';
}

void EngineC::Clear_Str() {
	memset(Command_Str, NULL, sizeof(Command_Str));
}

bool EngineC::Connect_Server() {
	bool _TIsConnected = false;

	if (_TelepathyClient->IsInitializeClient == true) {
		if (_TelepathyClient->IsConnectedClient == true)
			return true;

		// Server 연결 성공시에, ClientReceivedCallback을 묶어
		// Receive 할 때 Server에서 전송된 내용을 받아야 한다.
		if (_TelepathyClient->ClientConnect()) {
			_TelepathyClient->TClientReceivedCallback = ClientReceivedCallback;
			_TelepathyClient->ClientReceiveStart();
			_TIsConnected = true;
		}
	}

	return _TIsConnected;
}

void EngineC::Disconnect_Server() {
	_TelepathyClient->ClientDisconnect();
}

void EngineC::Clear_ClientSocket() {
	if (_TelepathyClient != NULL)
		delete _TelepathyClient;
}

void EngineC::SendToGUI(const char *Str, ...) {
	va_list _TArgument_List;
	char _Str[BUFFER_MAX_4096];

	va_start(_TArgument_List, Str);
	vsprintf(_Str, Str, _TArgument_List);
	va_end(_TArgument_List);

	fprintf(stdout, "%s\n", _Str);
}

void EngineC::Command_UCI() {
	// 1. 현재 Name, 저작자 보냄.
	SendToGUI("id name OpenCVE " ENGINE_EXEC_VER);
	SendToGUI("id author {Doohoon Kim, Sungpil Moon, Kyuhong Choi} in ARTeam of SW Maestro 4th");

	// Print Options.
	list<EO> _TEngineOption = _Option->GetEngineValues();
	
	// Test "for_TypeToEnd", not Standard.
	for_IterToEnd(list, EO, _TEngineOption) {
		if (_TVal->_OptionEnable == true) {
			if (_TVal->_VariableOptionString != NULL && _TVal->_VariableOptionString != '\0')
				SendToGUI("option name %s type %s default %s %s",
				_TVal->_VariableName, _TVal->_ControlerType, _TVal->_CurrentVariable, _TVal->_VariableOptionString);
			else
				SendToGUI("option name %s type %s default %s",
				_TVal->_VariableName, _TVal->_ControlerType, _TVal->_CurrentVariable);
		}
	}
	
	// 2. Send UCI String is "uciok".
	SendToGUI("uciok");
}

void EngineC::Command_Debug() {
	// Debug용이긴 하나 실제로 UCI에서 무엇을 Debugging하는지 의미 자체를 알 수 없으므로,
	// 이 Option은 구현하지 않는다.

	// No Implementation.
}

void EngineC::Command_Isready() {
	// CVES, CVEC가 Ready 되어 있느냐, 그렇지 않느냐를 판단하여 정리한다.

	// 1. CVES의 준비상태를 물어본다.

	// 2. CVEC를 잠시 Thread로 돌려 대기하게 한다.

	// 3. CVEC는 이미 준비가 되어 있으므로, "readyok"를 날린다.
	SendToGUI("readyok");
}

void EngineC::Command_Setoption(CommandString *_UCICS) {
	bool _TIsName = false;
	bool _TIsValue = false;

	string _TStringSetoption_Name;
	string _TStringSetoption_Value;

	while (_UCICS->NextCharArrayIter()){
		int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			case VALUE_SETOPTION_NAME :
				_TIsName = true;
				break;
			case VALUE_SETOPTION_VALUE :
				_TIsValue = true;
				break;
			case VALUE_ANYVALUES :
				if (_TIsName) {
					_TStringSetoption_Name = string((const char *)*_UCICS->CharArrayListIter);
					_TIsName = false;
				}
				else if (_TIsValue) {
					_TStringSetoption_Value = string((const char *)*_UCICS->CharArrayListIter);
					_TIsValue = false;
				}
				break;
		}
	}
}

void EngineC::Command_Ucinewgame() {
	// Game의 모든 변수를 Setting 한다.
	// 
}

void EngineC::Command_Register() {
	// Register를 쓸 일이 없다.

	// No Implementation
}

//
void EngineC::Command_Position(CommandString *_UCICS) {
	bool _IsFen = false;
	bool _IsStartpos = false;
	bool _IsMoves = false;

	while (_UCICS->NextCharArrayIter()){
		int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			case VALUE_POSITION_FEN :
				// 당장에 필요가 없을 것 같다.
				_IsFen = true;
				break;
			case VALUE_POSITION_STARTPOS :
				_IsStartpos = true;
				break;
			case VALUE_POSITION_MOVES :
				_IsMoves = true;
				break;
			case VALUE_ANYVALUES :
				break;
		}
	}

	// while이 종료되면 해야 할 것들.
	// 1. while에서 받아놓은 Move String을 모아서 저장한다.
	// 2. 
}

//
void EngineC::Command_Go(CommandString *_UCICS) {
	bool _IsBinc = false;
	bool _IsBtime = false;
	bool _IsWinc = false;
	bool _IsWtime = false;
	bool _IsMovestogo = false;

	// Fetch the next at while.
	while (_UCICS->NextCharArrayIter()) {
		int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			case VALUE_GO_BINC :
				_IsBinc = true;
				break;
			case VALUE_GO_BTIME :
				_IsBtime = true;
				break;
			case VALUE_GO_DEPTH :
				// No Implementation.
				break;
			case VALUE_GO_INFINITE :
				// No Implementation.
				break;
			case VALUE_GO_MATE :
				// No Implementation.
				break;
			case VALUE_GO_MOVESTOGO :
				_IsMovestogo = true;
				break;
			case VALUE_GO_MOVETIME :
				// No Implementation.
				break;
			case VALUE_GO_NODES :
				// No Implementation.
				break;
			case VALUE_GO_PONDER :
				// Implement me.
				break;
			case VALUE_GO_SEARCHMOVES :
				// No Implementation.
				break;
			case VALUE_GO_WINC :
				_IsWinc = true;
				break;
			case VALUE_GO_WTIME :
				_IsWtime = true;
				break;
			// 어떤 명령어 다음에 어떤 수.
			// 검출하지 못한 모든 것은 여기에 온다.
			case VALUE_ANYVALUES :
				if (_IsBinc) {	_IsBinc = false; }
				else if (_IsBtime) {

					_IsBtime = false;
				}
				else if (_IsWinc) { _IsWinc = false; }
				else if (_IsWtime) {

					_IsWtime = false;
				}
				else if (_IsMovestogo) {

					_IsMovestogo = false;
				}
				break;
		}
	}
}

void EngineC::Command_Stop() {
	// CVES의 인식을 멈춘다.
	_TelepathyClient->SendData("Stop");
	// 
}

void EngineC::Command_Ponderhit() {
	// AI가 아니기 때문에 ponderhit은 구현 하지 않는다.
  // 이 명령어는 AI가 예측 했던게 맞았냐, 틀렸냐에 따라 오는 명령어이기 때문이다.

	// No Implementation
}

void EngineC::Command_Quit() {
	// 모든 Process를 종료한다.
	// Server Process를 가지고 있다면 같이 종료 시킨다.

	// 1. 내가 Server Process를 가지고 있는지 확인.
	if (_ProcessConfirm->IsProcessActive == true) {
		// 2. 가지고 있다면 Server Process 종료 명령.
		if (_TelepathyClient->IsConnectedClient == true)
			_TelepathyClient->SendData("ServerKill");
	}

	// 3. Server의 완전 종료를 위해서 Server가 살아있는지, 아닌지를 판별하고,
	// 대기시간을 두어(10초) 종료 시킨다.
	// 그러기 위해서는 Thread를 쓴다.

	// 3. 완전 종료(Loop 정상 종료).
	EngineEnable = false;
}

void EngineC::Parsing_Command() {
	StringTokenizer *_StringTokenizer = new StringTokenizer();
	
	// Get UCI String.
	Get_Command_Str();

	_StringTokenizer->SetInputCharString((const char *)Command_Str);
	_StringTokenizer->SetSingleToken(" ");
	if (_StringTokenizer->StringTokenGo() == false)
		return ;
	
	CommandString *_UCICommandString = new CommandString(_StringTokenizer->GetTokenedCharListArrays());

	// 단지 명령어 Matching 하여 값만 비교.
	int _TSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICommandString->CharArrayListIter);

	switch (_TSeek_GUIToEngine) {
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
			Command_Setoption(_UCICommandString); //
			break;
		case VALUE_UCINEWGAME :
			Command_Ucinewgame();
			break;
		case VALUE_REGISTER :
			Command_Register();
			break;
		case VALUE_POSITION :
			Command_Position(_UCICommandString); //
			break;
		case VALUE_GO :
			Command_Go(_UCICommandString); //
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

	delete _UCICommandString;
	delete _StringTokenizer;
}

#pragma region CVEC_CVESCheckingThread
// CVEC와는 별개로 CVES 및 CVES의 통신이 죽으면 안 되므로,
// 이를 방지하기 위해 별도의 Thread를 두어 이들을 Checking 한다.
#if WINDOWS_SYS
//UINT
DWORD WINAPI
#elif POSIX_SYS
void *
#endif
 CVEC_CVESCheckingThread(
#if WINDOWS_SYS
	 LPVOID
#elif POSIX_SYS
	 void *
#endif
	 Param) {
	// 맨 처음에 해야 할 일.
	// 1. Process Checking.
	G_EngineC->CheckingCVESProcess();
	/*
	if (G_EngineC->IsNoCVESProcess)
		return 0;
	*/
	// Process가 시작될 때까지 기다린다.
	// 만약 G_EngineC->IsNoCVESProcess가 true면 0(이는, Server를 가지고 있지 않으므로 while을 실행하지 않는다는 이야기)
	// 만약 G_EngineC->IsNoCVESProcess가 false면 G_EngineC->Get_CVESProcessStatus()를 검사.
	while ((G_EngineC->IsNoCVESProcess) ? 0 : !G_EngineC->Get_CVESProcessStatus()) ;
	// 2. Process Enable 뒤 Server 접속.
	G_EngineC->Connect_Server();
	// 3. Parser가 살아있을 때 까지 무조건 계속 while 돌며 Process가 살아있는지, 통신이 살아있는지 Check 함.
	// 간혹 Loop에 걸렸을때 갑자기 종료가 될 수 있기 때문에, 항상 전제를 Engine이 Enable일 때만 돌게 끔 작업.
	while (G_EngineC->EngineEnable) {
		bool _Urgency = false;

		// 만약 여기서 끊기면, 게임이 끊겼다고 생각하고 재실행 및 재접속 작업에 돌입한다.
		// 일단 될 때까지 무한 반복.
		// Server를 가지고 있지 않아도 통신이 끊긴건 일단 비상 상황이므로, 통신을 재개할 수단을 마련해야 한다.
		if (((G_EngineC->IsNoCVESProcess) ?
			1 : (G_EngineC->Get_CVESProcessStatus() != true))
			&& G_EngineC->EngineEnable == true) {
			// CVES 없어지고 다시 실행할 때까지 계속 돈다.
			// Server를 가지고 있지 않다면, 굳이 실행 할 필요가 없다.
			while (((G_EngineC->IsNoCVESProcess) ? 0 : !G_EngineC->CheckingCVESProcess()) && G_EngineC->EngineEnable == true) ;
			
			// Process가 시작될 때까지 기다린다.
			// 역시 Server를 가지고 있지 않다면, 굳이 실행 할 필요가 없다.
			while ((G_EngineC->IsNoCVESProcess) ? 0 : !G_EngineC->Get_CVESProcessStatus()) ;
			
			// Server와 통신해야 하는 긴급한 상황(죽은 경우기 때문에).
			_Urgency = true;
		}

		if (G_EngineC->Get_TelepathyClient()->IsConnectedClient != true && G_EngineC->EngineEnable == true) {
			// CVES에 접속할 때까지 계속 돈다.
			while (!G_EngineC->Connect_Server() && G_EngineC->EngineEnable == true) ;

			if (_Urgency == true) {
				// 긴급한 상황.
				// Socket이 유효할 때, 복구가 가능한지를 CVES에 물어본다.
				G_EngineC->Get_TelepathyClient()->SendData("IsRestorePossible");
			}
		}
	}
	return 0;
}
#pragma endregion CVEC_CVESCheckingThread

bool EngineC::Get_CVESProcessStatus() {
	return _ProcessConfirm->IsProcessActive;
}

bool EngineC::Get_CVESConnectionStatus() {
	return _TelepathyClient->IsConnectedClient;
}

Telepathy::Client *EngineC::Get_TelepathyClient() {
	return _TelepathyClient;
}

bool EngineC::CheckingCVESProcess() {
	/*
		CheckingCVESProcess 주의 할 점 정리.
		(1) 초반에 실행될 때는 CVES Process가 없고, 내가 Process를 가지고 있지 않을 때.
		(2) 내게 CVES Process가 없고, 다른 CVES Process가 떠 있을 때(다른 Client가 가지고 있을 때).
		(3) 내가 Process를 가지고 있어서 이미 실행 중일 때.

		최종 정리.
		1. 다른 CVEC, CVES Process가 없고, 내가 Process를 가지고 있지 않을 때(가장 먼저 실행된 Client) - 무조건 CVES 실행(곧, 해당 CVEC가 CVES를 갖는다).
		(CVES 없음, CVEC 없음 - 해당 Process에 CVES 실행권 모름)
		2. 다른 CVEC, CVES Process가 있고, 내가 Process를 가지고 있지 않을 때(이미 다른 Client가 가지고 있을 때) - CVES 실행 없음.
		(CVES 있음, CVEC 있음 - 해당 Process에 CVES 실행권 없음)
		3. 다른 CVEC, CVES Process가 있었으나, 본 CVEC가 가지고 있던 CVES Process가 종료 되었을 때 - 무조건 CVES 실행.
		(CVES 없음, CVEC 있음 - 해당 Process에 CVES 실행권 있음)
		4. 다른 CVEC, CVES Process가 있었으나, 다른 CVEC가 가지고 있던 CVES Process가 종료 되었을 때 - CVES 실행 없음.
		(CVES 없음, CVEC 있음 - 해당 Process에 CVES 실행권 없음)

		이외의 경우(절대 있을 수 없는 경우 포함).
		1. CVEC Process가 전부 없고, 이미 CVES가 있는 경우 - CVES 종료 후 재실행(예외 상황으로, 이전 CVES만 종료 되지 않아 게임이 불가할 경우이다)
		1의 경우 강제로 Process를 종료 시켜야 하는데, 2가지의 시나리오로 가능하다.
		(1) Process 접근 권한을 얻어 Process를 '강제' 종료한다.
		(2) 이미 열려 있는 CVES에 접속하여 'Kill'을 날린다.
		2. CVES Process가 죽고, 이후에 파일이 갑자기 사라진 경우.
		구제 불가. WTF!!
	*/

	if (_ProcessConfirm->IsProcessActive == true) {
		// 내가 Process를 가지고 있고, CVES Process가 살아 있다면?
		// 이미 잘 실행되고 있으므로 true.
		return true;
	}
	else {
		// 최종정리 1, 2, 3, 4의 경우 처리.
		bool _TIsCVESProcessActive = false;
		bool _TIsAnotherCVECProcessActive = false;
		list<SProcessInformations> _TSProcessInformationsList;
		// 자신의 PID를 구한다.
		DWORD _TMyPID = GetCurrentProcessId();

		/*
			1. CVES Process 확인.
			나와 다른 Process를 Check 한다.
			왜냐하면 실행의 기준을 나누어야 한다.
			여기서 받아야 하는 값은..
			(1) 나 이외의 다른 Client(CVEC)의 존재 여부와 갯수(즉, 자기 자신 밖에 없는지 아닌지).
			(2) Server(CVES)의 존재 여부.
		*/
		// 이젠 이름으로 체크 하지 않음.
		//_IsCVESProcessActive = _ProcessConfirm->CheckProcessExistByFileName(SERVER_ENGINE_EXEC_FILENAME);
		//_IsAnotherCVECProcessActive = _ProcessConfirm->CheckProcessExistByFileName(CLIENT_ENGINE_EXEC_FILENAME);
		_TSProcessInformationsList = _ProcessConfirm->GetProcessInformations();

		// CVEC 검사(나 이외의 다른 Process 검사).
		for_IterToEnd(list, SProcessInformations, _TSProcessInformationsList) {
			// Image Path 검사.
			CodeConverter _TCodeConverter;
			char *_TStr = _TCodeConverter.WCharToChar(_TVal->ImgPath);
			char _TStrBuff[MAX_PATH];

			// 들어오는 String이 Null인 경우는 다음으로 넘어간다.
			if (_TStr == NULL)
				continue;

			memset(_TStrBuff, NULL, sizeof(_TStrBuff));
			// 가장 마지막에 있는 '\' 뒤에는 반드시 파일이 있기 때문이다.
			// 고로 맨 앞은 현재 파일의 Directory Path.
			strcpy(_TStrBuff, strrchr(_TStr, '\\') + 1);
			_TStrBuff[strlen(_TStrBuff)] = '\0';

			// 이름은 같은데 다른 Client Process가 이미 존재하는 경우.
			if (strcmp(_TStrBuff, CLIENT_ENGINE_EXEC_FILENAME) == 0 && _TVal->PID != _TMyPID)
				_TIsAnotherCVECProcessActive = true;

			// Server Process가 존재하는 경우.
			if (strcmp(_TStrBuff, SERVER_ENGINE_EXEC_FILENAME) == 0)
				_TIsCVESProcessActive = true;
		}
		
		// 2. CVES Process가 없다면 Process 실행.
		// 무조건 실행의 경우는 최종정리 1, 3의 경우이다.
		if (_TIsCVESProcessActive == false) {
			// Check File Exists.
			// 파일이 없으면 실행을 할 수 없기 때문에 처리.
			// 이외의 경우 2번에도 적용 가능.
			if (!_File.CheckFileExist(SERVER_ENGINE_EXEC_FILENAME))
				return false;

			// CVES 실행.
			if (_TIsAnotherCVECProcessActive != true) {
				// CVES Process를 이 Process가 갖는다.
				IsGetCVESProcess = true;
				_ProcessConfirm->CreateProcessOnThread(SERVER_ENGINE_EXEC_FILENAME);
			}
			else {
				if (IsGetCVESProcess == true){
					// 다른 Client가 있다고 해도, 내가 Server의 실행권한을 가지고 있다면 Server 실햄.
					_ProcessConfirm->CreateProcessOnThread(SERVER_ENGINE_EXEC_FILENAME);
				}
			}
			return true;
		}
		else {
			// 먼저 실행된 다른 Client(CVEC)가 있을 경우에는 해당 Client가 Server를 Watchdog 할 가능성이 높으므로,
			// 혹은 이미 Server를 실행해서 가지고 있으므로 Server를 실행하지 않는다.
			IsNoCVESProcess = true;
		}
		
		return false;
	}
}

void EngineC::EngineC_Start() {
	// 1. EngineC 초기화.
	Engine_Initializing();
	
	// 2. CVES의 Process와 CVES <-> CVEC 간의 통신이 끊기지 않도록 Check 하는 Thread.
#if WINDOWS_SYS
	DWORD _TThreadID = 0;
	#ifdef _AFXDLL
	CreateThread(NULL, 0, CVEC_CVESCheckingThread, 0, 0, &_TThreadID);
	#endif
#elif POSIX_SYS

#endif
	// 3. EngineC go Parsing.
	while (EngineEnable) {
		// Parser Engine Pause.
		//while (EnginePause) ;
		Parsing_Command();
	}

	// 4. EngineC Deinitializing.
	Engine_DeInitializing();
}

void ClientReceivedCallback(char *Buffer) {
	// 내부 Protocol 송신(CVEC -> CVES).
	StringTokenizer *_StringTokenizer = new StringTokenizer();
	InternalProtocolSeeker _InternalProtocolSeeker;

	_StringTokenizer->SetInputCharString((const char *)Buffer);
	_StringTokenizer->SetSingleToken(" ");
	if (_StringTokenizer->StringTokenGo() == false)
		return ;

	CommandString *_InternalProtocolCS = new CommandString(_StringTokenizer->GetTokenedCharListArrays());
	int _NSeek_CVESToCVEC = _InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_InternalProtocolCS->CharArrayListIter);
	
	switch (_NSeek_CVESToCVEC) {
		case VALUE_I_ALIVE :
			// 게임 재개.
			//G_EngineC->EnginePause = true;
			G_EngineC->Get_TelepathyClient()->SendData("Start");
			break;
		case VALUE_I_BUSY :
			// No Implement.
			break;
		case VALUE_I_MOVE :
			// UCI로 좌표 송신.
			_InternalProtocolCS->NextCharArrayIter();
			G_EngineC->SendToGUI("bestmove %s", _InternalProtocolCS->CharArrayListIter);
			break;
		case VALUE_I_RESTOREOK :
			// 복구 완료.
			// 게임 재개.
			G_EngineC->Get_TelepathyClient()->SendData("Start");
			break;
		case VALUE_I_RESTORENOT :
			// 복구 미완료.
			// All Stop the game.
			// CVES Process를 죽인다.
			G_EngineC->Get_TelepathyClient()->SendData("Stop");
			G_EngineC->Get_TelepathyClient()->SendData("ServerKill");
			G_EngineC->EngineEnable = false;
			break;
	}

	delete _InternalProtocolCS;
	delete _StringTokenizer;
}
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

// 기본 Header
#include "Engine.hpp"

// Callback을 위한 Menber.
Engine *G_Engine;

#pragma region Constructor & Destructor
// Constructor
Engine::Engine() {
	//IsSocketInitialize = false;
	//IsSocketConnented = false;
	//IsGetCVESProcess = false;
	EngineEnable = false;
	CVEC_CVESControlInitial = true;
	G_Engine = this;
}

// Destructor
Engine::~Engine() {
	//IsSocketInitialize = false;
	//IsSocketConnented = false;
	//IsGetCVESProcess = false;
	EngineEnable = false;
	CVEC_CVESControlInitial = false;
	G_Engine = NULL;
}
#pragma endregion Constructor & Destructor

#pragma region Initialize & Deinitialize Functions
void Engine::Initialize_CommandStr() {
	// Command는 BUFFER_MAX값에 의해서 결정
	Command_Str = (char *)calloc(BUFFER_MAX_32767, sizeof(char));
	Clear_Str();
}

void Engine::Deinitialize_CommandStr() {
	Clear_Str();
	free(Command_Str);
}

void Engine::Initialize_CVEOption() {
	_Option = new Option();
}

void Engine::Deinitialize_CVEOption() {
	if (_Option != NULL)
		delete _Option;
}

bool Engine::Initialize_TClient() {
	_TelepathyClient = new Telepathy::Client();
	// SendToGUI("Wait for Server Request.");
	// Inititalizing Client.
	/*
	if (_TClient->ClientInitialize() == true) {
		//IsSocketInitialize = true;
	}	
	else {
	*/
	if (_TelepathyClient->ClientInitialize() != true) {
		// Server 연결 실패.
		SendToGUI("Client Initialize Failed.");
		return false;
	}

	// 연결 성립.
	//SendToGUI("Connected.");
	return true;
}

void Engine::Deinitialize_TClient() {
	if (_TelepathyClient != NULL)
		delete _TelepathyClient;
}

void Engine::Initialize_ProcessConfirm() {
	_ProcessConfirm = new ProcessConfirm();
}

void Engine::Deinitialize_ProcessConfirm() {
	if (_ProcessConfirm != NULL)
		delete _ProcessConfirm;
}

void Engine::Put_Author() {
	// 4 Parser Engine Start.
	SendToGUI("OpenCVE Client Engine Version %s.", ENGINE_EXEC_VER);
	SendToGUI("{Doohoon Kim, Sungpil Moon, Kyuhong Choi} Copyright All right reserved.");
	/*
	// in ARTeam of SW Maestro 4th.
	printf("OpenCVE Connector Ver %s Start.\n",  ENGINE_EXEC_VER);
	printf("Engine Copyright by ARTeam.\n");
	*/
}

void Engine::Engine_Initializing() {
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

void Engine::Engine_DeInitializing() {
	Deinitialize_CommandStr();
	Deinitialize_CVEOption();
	Deinitialize_ProcessConfirm();
}
#pragma endregion Initialize & Deinitialize Functions

void Engine::Get_Command_Str() {
	if (fgets(Command_Str, BUFFER_MAX_32767, stdin) == NULL) {
		if (feof(stdin)) {
			return ;
		}
	}

	char *_StrPtr = strchr(Command_Str,'\n');
	if (_StrPtr != NULL) *_StrPtr = '\0';
}

void Engine::Clear_Str() {
	memset(Command_Str, NULL, sizeof(Command_Str));
}

bool Engine::Connect_Server() {
	bool _TIsConnected = false;
	if (_TelepathyClient->IsInitializeClient == true) {
		// Server 연결 성공시에, ClientReceivedCallback을 묶어
		// Receive 할 때 Server에서 전송된 내용을 받아야 한다.
		_TelepathyClient->TClientReceivedCallback = ClientReceivedCallback;
		_TelepathyClient->ClientReceiveStart();
		//IsSocketConnented = true;
	}
	//return _TClient->ClientConnect();
}

void Engine::Disconnect_Server() {
	_TelepathyClient->ClientDisconnect();
}

void Engine::Clear_ClientSocket() {
	if (_TelepathyClient != NULL)
		delete _TelepathyClient;
}

void Engine::SendToGUI(const char *Str, ...) {
	va_list _Argument_List;
	char _Str[BUFFER_MAX_4096];

	va_start(_Argument_List, Str);
	vsprintf(_Str, Str, _Argument_List);
	va_end(_Argument_List);

	fprintf(stdout, "%s\n", _Str);
}

void Engine::Command_UCI() {
	// 1. 현재 Name, 저작자 보냄.
	SendToGUI("id name OpenCVE " ENGINE_EXEC_VER);
	SendToGUI("id author {Doohoon Kim, Sungpil Moon, Kyuhong Choi} in ARTeam of SW Maestro 4th");

	// Print Options.
	list<EO> _TEO = _Option->GetEngineValues();
	
	// Test "for_TypeToEnd", not Standard.
	for_IterToEnd(list, EO, _TEO) {
		if (_TVal->_OptionEnable == true) {
			if (_TVal->_VariableOptionString != NULL && _TVal->_VariableOptionString != '\0')
				SendToGUI("option name %s type %s default %s %s",
				_TVal->_VariableName, _TVal->_ControlerType, _TVal->_CurrentVariable, _TVal->_VariableOptionString);
			else
				SendToGUI("option name %s type %s default %s",
				_TVal->_VariableName, _TVal->_ControlerType, _TVal->_CurrentVariable);
		}
	}
	/*
	list<EO>::iterator _TEOIter;
	for (_TEOIter = _TEO.begin();
		_TEOIter != _TEO.end(); _TEOIter++) {
		if (_TEOIter->_OptionEnable == true){
			if (_TEOIter->_VariableOptionString != NULL && _TEOIter->_VariableOptionString != '\0')
				SendToGUI("option name %s type %s default %s %s",
					_TEOIter->_VariableName, _TEOIter->_ControlerType, _TEOIter->_CurrentVariable, _TEOIter->_VariableOptionString);
			else
				SendToGUI("option name %s type %s default %s",
					_TEOIter->_VariableName, _TEOIter->_ControlerType, _TEOIter->_CurrentVariable);
		}
	}
	*/
	// 2. Send UCI String is "uciok".
	SendToGUI("uciok");
}

void Engine::Command_Debug() {
	// Debug용이긴 하나 실제로 UCI에서 무엇을 Debugging하는지 의미 자체를 알 수 없으므로,
	// 이 Option은 구현하지 않는다.

	// No Implementation.
}

void Engine::Command_Isready() {
	// CVES, CVEC가 Ready 되어 있느냐, 그렇지 않느냐를 판단하여 정리한다.

	// 1. CVES의 준비상태를 물어본다.

	// 2. CVEC를 잠시 Thread로 돌려 대기하게 한다.

	// 3. CVEC는 이미 준비가 되어 있으므로, "readyok"를 날린다.
	SendToGUI("readyok");
}

void Engine::Command_Setoption(CS *_UCICS) {
	bool _IsName = false;
	bool _IsValue = false;

	string _StringSetoption_Name;
	string _StringSetoption_Value;

	while (_UCICS->NextCharArrayIter()){
		int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			case VALUE_SETOPTION_NAME :
				_IsName = true;
				break;
			case VALUE_SETOPTION_VALUE :
				_IsValue = true;
				break;
			case VALUE_ANYVALUES :
				if (_IsName) {
					_StringSetoption_Name = string((const char *)*_UCICS->CharArrayListIter);
					_IsName = false;
				}
				else if (_IsValue) {
					_StringSetoption_Value = string((const char *)*_UCICS->CharArrayListIter);
					_IsValue = false;
				}
				break;
		}
	}
}

void Engine::Command_Ucinewgame() {
	// Game의 모든 변수를 Setting 한다.
	// 
}

void Engine::Command_Register() {
	// Register를 쓸 일이 없다.

	// No Implementation
}

//
void Engine::Command_Position(CS *_UCICS) {
	bool _IsFen = false;
	bool _IsStartpos = false;
	bool _IsMoves = false;

	while (_UCICS->NextCharArrayIter()){
		int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			case VALUE_POSITION_FEN :
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
void Engine::Command_Go(CS *_UCICS) {
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

void Engine::Command_Stop() {
	// CVES의 인식을 멈춘다.

	// 
}

void Engine::Command_Ponderhit() {
	// AI가 아니기 때문에 ponderhit은 구현 하지 않는다.
  // 이 명령어는 AI가 예측 했던게 맞았냐, 틀렸냐에 따라 오는 명령어이기 때문이다.

	// No Implementation
}

void Engine::Command_Quit() {
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

	// 3. 완전 종료.
	EngineEnable = false;
	//exit(EXIT_SUCCESS);
}

void Engine::Parsing_Command() {
	StringTokenizer *_StringTokenizer = new StringTokenizer();
	
	// Get UCI String.
	Get_Command_Str();
	//FILE *ofp = fopen("OpenCVE_Log.txt","a+");

	//fprintf(ofp, "%s\n", Command_Str);
	//fclose(ofp);

	_StringTokenizer->SetInputCharString((const char *)Command_Str);
	_StringTokenizer->SetSingleToken(" ");
	if (_StringTokenizer->StringTokenGo() == false)
		return ;
	//_UCICommandString->UCIStringList = _StringTokenizer->GetTokenedStringList();
	CS *_UCICommandString = new CS(_StringTokenizer->GetTokenedCharListArrays());

	// 단지 명령어 Matching 하여 값만 비교.
	//int _NSeek_GUIToEngine = _UCICommandParser.UCIString_Seeker(Command_Str);

	/*
	list<char *>::iterator _CharIter;
	_CharIter = _UCICommandString->UCICharArrayList->begin();
	*/
	int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICommandString->CharArrayListIter);

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
	G_Engine->CheckingCVESProcess();
	// 2. Process Enable 뒤 Server 접속.

	// 3. Parser가 살아있을 때 까지 무조건 계속 while 돌며 Process가 살아있는지, 통신이 살아있는지 Check 함.
	while (G_Engine->EngineEnable) {
		bool _Urgency = false;

		// 만약 여기서 끊기면, 게임이 끊겼다고 생각하고 재실행 및 재접속 작업에 돌입한다.
		// 일단 될 때까지 무한 반복.
		if (G_Engine->Get_CVESProcessStatus() != true) {
			// CVES 없어지고 다시 실행할 때까지 계속 돈다.
			while (!G_Engine->CheckingCVESProcess()) ;
			// Server와 통신해야 하는 긴급한 상황(죽은 경우기 때문에).
			_Urgency = true;
		}
		if (G_Engine->Get_TelepathyClient()->IsConnectedClient != true) {
			// CVES에 접속할 때까지 계속 돈다.
			while (!G_Engine->Connect_Server()) ;
			//G_Parser->
			if (_Urgency == true) {
				// 긴급한 상황.
				// Socket이 유효할 때, 복구가 가능한지를 CVES에 물어본다.
				G_Engine->Get_TelepathyClient()->SendData("IsRestorePossible");
			}
		}
	}
	return 0;
}
#pragma endregion CVEC_CVESCheckingThread

bool Engine::Get_CVESProcessStatus() {
	return _ProcessConfirm->IsProcessActive;
}

bool Engine::Get_CVESConnectionStatus() {
	//return _TClient->
	return _TelepathyClient->IsConnectedClient;
}

Telepathy::Client* Engine::Get_TelepathyClient() {
	return _TelepathyClient;
}

bool Engine::CheckingCVESProcess() {
	bool _IsCVESProcessActive = false;
	bool _IsAnotherCVECProcessActive = false;

	// 1. CVES Process 확인. 
	_IsCVESProcessActive = _ProcessConfirm->CheckProcess(SERVER_ENGINE_EXEC_FILENAME);
	_IsAnotherCVECProcessActive = _ProcessConfirm->CheckProcess(CLIENT_ENGINE_EXEC_FILENAME);

	// 2. CVES Process가 없다면 Process 실행.
	// 여기서 주의 할 점 정리.
	// (1) 초반에 실행될 때는 CVES Process가 없고, 내가 Process를 가지고 있지 않을 때.
	// (2) 내게 CVES Process가 없고, 다른 CVES Process가 떠 있을 때(다른 Client가 가지고 있을 때).
	// (3) 내가 Process를 가지고 있어서 이미 실행 중일 때.

	// 이건 내가 Process를 가지고 있다는 이야기 이므로..
	if (_ProcessConfirm->IsProcessActive == true) {
	//if (IsGetCVESProcess == true) {
		// 하지만 이미 있다면?
		// 본 Process가 CVES Process를 가지고 있으므로, 무조건 Process를 실행.

		// Check File Exists.
		// 여기에 걸리는 경우는.. 아마 갑자기 파일이 삭제 되었을 때 정도.
		if (!_File.CheckFileExist(SERVER_ENGINE_EXEC_FILENAME))
			return false;

		_ProcessConfirm->CreateProcessOnThread(SERVER_ENGINE_EXEC_FILENAME);
		return true;
	}
	else {
		// CVES가 없다면(Engine이 시작되고 초반)..
		if (_IsCVESProcessActive == false) {
			/*
			if (_BIsAnotherCVECProcessActive == false) {
				// 어차피 In
				CVEC_CVESControlInitial = false;
			}
			*/
			// Check File Exists.
			if (!_File.CheckFileExist(SERVER_ENGINE_EXEC_FILENAME))
				return false;

			// CVES Process를 이 Process가 갖는다.
			//IsGetCVESProcess = true;
			// CVES 실행.
			_ProcessConfirm->CreateProcessOnThread(SERVER_ENGINE_EXEC_FILENAME);
			return true;
		}
		else {
			// 이미 이 때는 다른 CVEC가 CVES Process를 가지고 있으므로, CVES를 실행 할 필요가 없다.
		}
	}
	return false;
}

void Engine::Parser_Engine_Start() {
	Engine_Initializing();
	
	// CVES의 Process와 CVES <-> CVEC 간의 통신이 끊기지 않도록 Check 하는 Thread.
#if WINDOWS_SYS
	DWORD _TThreadID = 0;
	#ifdef _AFXDLL
	//AfxBeginThread(CVEC_CVESCheckingThread, 0);
	CreateThread(NULL, 0, CVEC_CVESCheckingThread, 0, 0, &_TThreadID);
	#endif
#elif POSIX_SYS

#endif
	while (EngineEnable) Parsing_Command();

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

	CS *_InternalProtocolCS = new CS(_StringTokenizer->GetTokenedCharListArrays());

	int _NSeek_CVESToCVEC = _InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_InternalProtocolCS->CharArrayListIter);
	
	switch (_NSeek_CVESToCVEC) {
		case VALUE_ALIVE :
			// 게임 재개.
			break;
		case VALUE_BUSY :
			// No Implement.
			break;
		case VALUE_MOVE :
			// UCI로 좌표 송신.
			break;
		case VALUE_RESTOREOK :
			// 복구 완료.
			// 게임 재개.
			break;
		case VALUE_RESTORENOT :
			// 복구 미완료.
			// 게임 All Stop.
			break;
	}
}
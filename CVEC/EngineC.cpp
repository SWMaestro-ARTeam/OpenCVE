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
#include "EngineC.hpp"

// Callback을 위한 Menber.
EngineC *G_EngineC;

#pragma region Constructor & Destructor
// Constructor
EngineC::EngineC() {
	_IsGetCVESProcess = false;
	_IsNoCVESProcess = false;
	_ServerPID = 0;
	EngineEnable = false;
	EnginePause = false;
	G_EngineC = this;
}

// Destructor
EngineC::~EngineC() {
	_IsGetCVESProcess = false;
	_IsNoCVESProcess = false;
	_ServerPID = 0;
	EngineEnable = false;
	EnginePause = false;
	G_EngineC = NULL;
}
#pragma endregion Constructor & Destructor

#pragma region Initialize & Deinitialize Functions
void EngineC::Initialize_CommandStr() {
	// Command는 BUFFER_MAX값에 의해서 결정
	_Command = (char *)calloc(BUFFER_MAX_32767, sizeof(char));
	Clear_Str();
}

void EngineC::Deinitialize_CommandStr() {
	Clear_Str();
	free(_Command);
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
	if (_TelepathyClient->IsConnectedClient == true)
		Disconnect_Server();
	Clear_ClientSocket();
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
		SendToGUI("Socket Initialization Failed.");
}

void EngineC::Engine_DeInitializing() {
	while (_TelepathyClient->IsConnectedClient) ;
	Deinitialize_CommandStr();
	Deinitialize_CVEOption();
	Deinitialize_ProcessConfirm();
	// 통신이 종료되면 알아서 닫기 때문에 따로 Telepathy를 닫아줄 필요가 없다.
	//Deinitialize_TClient();
}
#pragma endregion Initialize & Deinitialize Functions

void EngineC::Get_Command_Str() {
	if (fgets(_Command, BUFFER_MAX_32767, stdin) == NULL) {
		if (feof(stdin)) {
			return ;
		}
	}

	char *_StrPtr = strchr(_Command,'\n');

	if (_StrPtr != NULL)
		*_StrPtr = '\0';
}

void EngineC::Clear_Str() {
	memset(_Command, NULL, sizeof(_Command));
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
			_TelepathyClient->TClientDisconnectedCallback = ClientDisconnectedCallback;
			_TelepathyClient->ClientReceiveStart();
			// Command Queue 생성.
			CommandQueue = new queue<char *>();
			// Command 처리용 Thread를 생성.
			HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ClientCommandQueueProcessingThread, this, 0, NULL);
			
			_TIsConnected = true;
		}
	}

	return _TIsConnected;
}

void EngineC::Disconnect_Server() {
	_TelepathyClient->ClientDisconnect();
	delete CommandQueue;
	_TelepathyClient->TClientReceivedCallback = NULL;
	_TelepathyClient->TClientDisconnectedCallback = NULL;
}

bool EngineC::Reconnect_Server() {
	//Disconnect_Server();

	if (Initialize_TClient() != true)
		SendToGUI("Socket Initialize Failed.");
	
	return Connect_Server();
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
	list<EngineOptions> _TEngineOption = _Option->GetEngineValues();
	
	// Test "for_TypeToEnd", not Standard.
	for_IterToEnd(list, EngineOptions, _TEngineOption) {
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
	SystemControlsOfTime _TSystemControlsOfTime;
	// CVES, CVEC가 Ready 되어 있느냐, 그렇지 않느냐를 판단하여 정리한다.
	_IsCVESReady = false;
	// 1. CVES의 준비상태를 물어본다.
	_TelepathyClient->SendData("IsServerReady");
	// 2. CVEC를 잠시 while로 돌려 10초간 대기하게 한다.
	// 이래도 안되면, 접속이 안된 것으로 간주한다.
	//if (_TSystemControlsOfTime.WaitSecondsUntilSwitch(10, _IsCVESReady) == true) {
	//	// 여기에 들어오면 일단 10초를 지난 것으로 간주.
	//	// 나올 수 있는 예외를 처리해준다.
	//	return ;
	//}
	// 3. CVEC는 이미 준비가 되어 있으므로, "readyok"를 날린다.
	SendToGUI("readyok");
}

void EngineC::Command_Setoption(CommandString *_UCICS) {
	bool _TIsName = false;
	bool _TIsValue = false;

	StringTools _TStringTools;
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

	if (_Option->FindEngineOptionName(
		(const char *)_TStringTools.StirngToConstCharPointer(_TStringSetoption_Name)) == true) {
		EngineOptions _TEngineOptions = EngineOptions(
			(const char *)_TStringTools.StirngToConstCharPointer(_TStringSetoption_Name),
			true,
			(const char *)NULL,
			(const char *)NULL,
			(const char *)NULL,
			(const char *)NULL
			);

		_Option->SetEngineValue(_TEngineOptions);
	}
}

void EngineC::Command_Ucinewgame() {
	// Game의 모든 변수를 Setting 한다.

	// Null Move가 들어올 때, 자신이 White가 된다.
	// 따라서 처음에는 전부 흑으로 놓고, 어차피 Null Move가 된 Client를 가리면 된다.
	_IsWhite = false;
}

void EngineC::Command_Register() {
	// Register를 쓸 일이 없다.

	// No Implementation
}

void EngineC::Command_Position(CommandString *_UCICS) {
	StringTools _TStringTools;
	string _TString = string("");

	bool _IsFen = false;
	bool _IsStartpos = false;
	bool _IsMoves = false;

	_TString.append("Info Position ");

	while (_UCICS->NextCharArrayIter()) {
		int _NSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_UCICS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			case VALUE_POSITION_FEN :
				// No Implementation.
				// 당장에 필요가 없을 것 같다.
				_IsFen = true;
				break;
			case VALUE_POSITION_STARTPOS :
				_IsStartpos = true;
				// Null Move인지 아닌지를 검사.
				// Null Move 이면 White, 아니면 Black.
				if (_UCICS->IsLastCharArrayIter() == true) {
					_IsWhite = true;
					_TString.append(STR_I_INFO_MOVENULL);
				}
				break;
			case VALUE_POSITION_MOVES :
				// Moves가 올 때, Enemy Move를 보내준다.
				// 상대측 좌표만 보내준다.
				_IsMoves = true;
				_TString.append(STR_I_INFO_ENEMYMOVE);
				_TString.append(" ");
				break;
			case VALUE_ANYVALUES :
				if (_IsStartpos && _IsMoves) {
					// CVES에서는 상대의 움직임을 맨 마지막 것만 보내준다.
					// 실제로 모든 정보를 다 가지고 있을 필요는 없으며, Human vs AI를 위한 처리이다.
					if (_UCICS->IsLastCharArrayIter() == true)
						_TString.append(string((const char *)*_UCICS->CharArrayListIter));
				}
				break;
		}
	}
	// while이 종료되면 해야 할 것들.
	// 1. while에서 받아놓은 Move String을 보낸다.
	_TelepathyClient->SendData((char *)_TStringTools.StirngToConstCharPointer(_TString));
}

//
void EngineC::Command_Go(CommandString *_UCICS) {
	StringTools _TStringTools;
	string _TString = string("");
	
	bool _IsBinc = false;
	bool _IsBtime = false;
	bool _IsWinc = false;
	bool _IsWtime = false;
	bool _IsMovestogo = false;
	bool _IsPonder = false;
	
	_TString.append("Info Go ");
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
				// 실제 Infinite에 대한 경우는 시간을 무한대로 줬을 때이다.
				// 사람이 할 때에는 실제로 별 상관 없다.
				// 어쨋거나 명령어 처리 때문에 어쩔수 없이 준다.
				_TString.append(string((const char *)*_UCICS->CharArrayListIter));
				break;
			case VALUE_GO_MATE :
				// No Implementation.
				// 실제 Chess AI들에서도 비워놓은 부분.
				break;
			case VALUE_GO_MOVESTOGO :
				_IsMovestogo = true;
				break;
			case VALUE_GO_MOVETIME :
				// No Implementation.
				// 실제 Chess AI들에서도 비워놓은 부분.
				break;
			case VALUE_GO_NODES :
				// No Implementation.
				// 실제 Chess AI들에서도 비워놓은 부분.
				break;
			case VALUE_GO_PONDER :
				// Implement me.
				// ponder가 와도 무시한다.
				// 사실은 ponder가 AI에서는 계산의 delay를 결정하지만, 실제 사람이 두는 것은
				// delay가 무한대이므로, 이 명령이 와도 무시한다.
				// 만일의 경우를 위해 남겨둔다. 왜냐면, AI를 붙일 수도 있기 때문이다.
				_IsPonder = true;
				break;
			case VALUE_GO_SEARCHMOVES :
				// No Implementation.
				// 실제 Chess AI들에서도 비워놓은 부분.
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
					// 현재 흑색 Player의 시간을 나타내준다.
					// 나중에 CVES에서 흑색 Player의 현재 시간을 Check할 때 쓴다(만약 내가 흑색일 때).
					if (_IsWhite == true) {
						_TString.append(STR_I_INFO_BLACKTIME);
						_TString.append(" ");
						_TString.append(string((const char *)*_UCICS->CharArrayListIter));
						_TString.append(" ");
					}
					
					_IsBtime = false;
				}
				else if (_IsWinc) { _IsWinc = false; }
				else if (_IsWtime) {
					// 현재 백색 Player의 시간을 나타내준다.
					// 나중에 CVES에서 백색 Player의 현재 시간을 Check할 때 쓴다(만약 내가 백색일 때).
					if (_IsWhite != true) {
						_TString.append(STR_I_INFO_WHITETIME);
						_TString.append(" ");
						_TString.append(string((const char *)*_UCICS->CharArrayListIter));
						_TString.append(" ");
					}
					
					_IsWtime = false;
				}
				else if (_IsMovestogo) {
					// 현재 자신의 턴수를 CVES에 보낼 String에 넣는다.
					// 항상 턴수는 마지막에 온다.
					_TString.append(STR_I_INFO_TURN);
					_TString.append(" ");
					_TString.append(string((const char *)*_UCICS->CharArrayListIter));
					_IsMovestogo = false;
				}
				break;
		}
	}

	_TelepathyClient->SendData((char *)_TStringTools.StirngToConstCharPointer(_TString));
}

void EngineC::Command_Stop() {
	// CVES의 인식을 멈춘다.
	_TelepathyClient->SendData("Stop");
	// 
}

void EngineC::Command_Ponderhit() {
	// AI가 아니기 때문에 ponderhit은 구현 하지 않는다.
  // 이 명령어는 AI가 예측 했던게 맞았냐, 틀렸냐에 따라 오는 명령어이기 때문이다.
	// AI를 위해 함수 원형만 남긴다.

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

//FILE *ofp =fopen("CVEC_Log.txt", "w");
void EngineC::Parsing_Command() {
	StringTokenizer *_TStringTokenizer = new StringTokenizer();
	
	// Get UCI String.
	Get_Command_Str();

	//fprintf(ofp,"%s\n",_Command);
	_TStringTokenizer->SetInputCharString((const char *)_Command);
	_TStringTokenizer->SetSingleToken(" ");
	if (_TStringTokenizer->StringTokenGo() == false)
		return ;
	
	CommandString *_TUCICommandString = new CommandString(_TStringTokenizer->GetTokenedCharListArrays());
	// 단지 명령어 Matching 하여 값만 비교.
	int _TSeek_GUIToEngine = _UCICommandSeeker.UCIString_Seeker((const char *)*_TUCICommandString->CharArrayListIter);

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
			Command_Setoption(_TUCICommandString); //
			break;
		case VALUE_UCINEWGAME :
			Command_Ucinewgame();
			break;
		case VALUE_REGISTER :
			Command_Register();
			break;
		case VALUE_POSITION :
			Command_Position(_TUCICommandString); //
			break;
		case VALUE_GO :
			Command_Go(_TUCICommandString); //
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

	delete _TUCICommandString;
	delete _TStringTokenizer;
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

			_VarProtectMutex.lock();
			memset(_TStrBuff, NULL, sizeof(_TStrBuff));
			// 가장 마지막에 있는 '\' 뒤에는 반드시 파일이 있기 때문이다.
			// 고로 맨 앞은 현재 파일의 Directory Path.
			strcpy(_TStrBuff, strrchr(_TStr, '\\') + 1);
			_TStrBuff[strlen(_TStrBuff)] = '\0';
			_VarProtectMutex.unlock();

			// 이름은 같은데 다른 Client Process가 이미 존재하는 경우.
			if (strcmp(_TStrBuff, CLIENT_ENGINE_EXEC_FILENAME) == 0 && _TVal->PID != _TMyPID)
				_TIsAnotherCVECProcessActive = true;

			// Server Process가 존재하는 경우.
			if (strcmp(_TStrBuff, SERVER_ENGINE_EXEC_FILENAME) == 0) {
				_TIsCVESProcessActive = true;
				_ServerPID = _TVal->PID;
			}
		}
		
		// 2. CVES Process가 없다면 Process 실행.
		// 무조건 실행의 경우는 최종정리 1, 3의 경우이다.
		if (_TIsCVESProcessActive == false) {
			// Check File Exists.
			// 파일이 없으면 실행을 할 수 없기 때문에 처리.
			// 이외의 경우 2번에도 적용 가능.
			if (!_File.CheckFileExist(SERVER_ENGINE_EXEC_FILENAME))
				return false;
			
			// Image Path 만들어 주기.
			_VarProtectMutex.lock();
			char _TCharArr[MAX_PATH], _TImageCmd[MAX_PATH + BUFFER_MAX_2048];
			memset(_TCharArr, NULL, sizeof(_TCharArr));
			strcpy(_TCharArr,_File.GetCurrentPath());
			memset(_TImageCmd, NULL, sizeof(_TImageCmd));
			sprintf(_TImageCmd, "%s%s", _TCharArr, SERVER_ENGINE_EXEC_FILENAME);
			_VarProtectMutex.unlock();
			
			// CVES(Server) 실행.
			if (_TIsAnotherCVECProcessActive != true) {
				// CVES Process를 이 Process가 갖는다.
				_IsGetCVESProcess = true;
				_ProcessConfirm->CreateProcessOnThread((char *)_TImageCmd);
			}
			else {
				if (_IsGetCVESProcess == true){
					// 다른 Client가 있다고 해도, 내가 Server의 실행권한을 가지고 있다면 Server 실햄.
					_ProcessConfirm->CreateProcessOnThread((char *)_TImageCmd);
				}
			}
			return true;
		}
		else {
			// 먼저 실행된 다른 Client(CVEC)가 있을 경우에는 해당 Client가 Server를 Watchdog 할 가능성이 높으므로,
			// 혹은 이미 Server를 실행해서 가지고 있으므로 Server를 실행하지 않는다.
			_IsNoCVESProcess = true;
		}
		
		return false;
	}
}

void EngineC::EngineC_Start() {
	// 1. EngineC 초기화.
	Engine_Initializing();
	
	// 2. Check & Start-up CVES.
	// 2. CVES의 Process와 CVES <-> CVEC 간의 통신이 끊기지 않도록 Check 하는 Thread.
#if WINDOWS_SYS
	//#ifdef _AFXDLL
	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, CVEC_CVESCheckingThread, this, 0, NULL);
	//#endif
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

#pragma region Client Received Callback
void EngineC::ClientReceivedCallback(char *Buffer) {
	// using mutex.
	G_EngineC->_QueueProtectMutex.lock();
	char _TBuffer[BUFFER_MAX_32767];
	memset(_TBuffer, NULL, sizeof(_TBuffer));
	strcpy(_TBuffer, Buffer);
	G_EngineC->CommandQueue->push(_TBuffer);
	G_EngineC->_QueueProtectMutex.unlock();
}
#pragma endregion Client Received Callback

#pragma region Client Received Callback
void EngineC::ClientDisconnectedCallback() {
	bool _TIsConnected = false;
	// 3. Parser가 살아있을 때 까지 무조건 계속 while 돌며 Process가 살아있는지, 통신이 살아있는지 Check 함.
	// 간혹 Loop에 걸렸을때 갑자기 종료가 될 수 있기 때문에, 항상 전제를 Engine이 Enable일 때만 돌게 끔 작업.
	while (G_EngineC->EngineEnable) {
	//if (G_EngineC->EngineEnable) {
		//Sleep(100);
		bool _Urgency = false;

		// 만약 여기서 끊기면, 게임이 끊겼다고 생각하고 재실행 및 재접속 작업에 돌입한다.
		// 일단 될 때까지 무한 반복.
		// Server를 가지고 있지 않아도 통신이 끊긴건 일단 비상 상황이므로, 통신을 재개할 수단을 마련해야 한다.
		if (((G_EngineC->_IsNoCVESProcess) ?
			0 : (G_EngineC->_ProcessConfirm->IsProcessActive != true))
			&& G_EngineC->EngineEnable == true) {
				// Server가 Error가 나서 죽었는데 Process가 Active 된 경우에는 일단 해당 Server Process를 죽이고 들어간다.
				HANDLE _TServerProcessHandle = G_EngineC->_ProcessConfirm->FindProcessByPID(G_EngineC->_ServerPID);
				if (G_EngineC->_ProcessConfirm->GetProcessStatus(_TServerProcessHandle) ==
#if WINDOWS_SYS
					STILL_ACTIVE
#elif POSIX_SYS

#endif
					) {
						G_EngineC->_ProcessConfirm->TerminateProcess(_TServerProcessHandle);
				}
				// CVES 없어지고 다시 실행할 때까지 계속 돈다.
				// Server를 가지고 있지 않다면, 굳이 실행 할 필요가 없다.
				while (((G_EngineC->_IsNoCVESProcess) ? 0 : !G_EngineC->CheckingCVESProcess()) && G_EngineC->EngineEnable == true) ;

				// Process가 시작될 때까지 기다린다.
				// 역시 Server를 가지고 있지 않다면, 굳이 실행 할 필요가 없다.
				while ((G_EngineC->_IsNoCVESProcess) ? 0 : !G_EngineC->_ProcessConfirm->IsProcessActive) ;

				// Server와 통신해야 하는 긴급한 상황(죽은 경우기 때문에).
				_Urgency = true;
		}

		if (G_EngineC->_TelepathyClient->IsConnectedClient != true && G_EngineC->EngineEnable == true) {
			// CVES에 접속할 때까지 계속 돈다.
			while (((G_EngineC->_IsNoCVESProcess) ? 1 : G_EngineC->_ProcessConfirm->IsProcessActive)
				&& G_EngineC->EngineEnable == true) {
					if (G_EngineC->Reconnect_Server()) {
						_TIsConnected = true;
						break;
					}
			}

			if (_Urgency == true) {
				// 긴급한 상황.
				// Socket이 유효할 때, 복구가 가능한지를 CVES에 물어본다.
				G_EngineC->_TelepathyClient->SendData("IsRestorePossible");
			}

			if (_TIsConnected == true)
				break;
		}
	}
}

#pragma region CVEC_CVESCheckingThread
// CVEC와는 별개로 CVES 및 CVES의 통신이 죽으면 안 되므로,
// 이를 방지하기 위해 별도의 Thread를 두어 이들을 Checking 한다.
#if WINDOWS_SYS
UINT WINAPI
//DWORD WINAPI
#elif POSIX_SYS
void *
#endif
 EngineC::CVEC_CVESCheckingThread(
#if WINDOWS_SYS
	 LPVOID
#elif POSIX_SYS
	 void *
#endif
	 Param) {
	EngineC *_TEngine_C = (EngineC *)Param;
	// 맨 처음에 해야 할 일.
	// 1. Process Checking.

	_TEngine_C->CheckingCVESProcess();
	
	// Process가 시작될 때까지 기다린다.
	// 만약 _TEngine_C->IsNoCVESProcess가 true면 0(이는, Server를 가지고 있지 않으므로 while을 실행하지 않는다는 이야기)
	// 만약 _TEngine_C->IsNoCVESProcess가 false면 _TEngine_C->_ProcessConfirm->IsProcessActive를 검사.
	while ((_TEngine_C->_IsNoCVESProcess) ? 0 : !_TEngine_C->_ProcessConfirm->IsProcessActive) ;
	// 2. Process Enable 뒤 Server 접속.
	_TEngine_C->Connect_Server();
	//// 3. Parser가 살아있을 때 까지 무조건 계속 while 돌며 Process가 살아있는지, 통신이 살아있는지 Check 함.
	//// 간혹 Loop에 걸렸을때 갑자기 종료가 될 수 있기 때문에, 항상 전제를 Engine이 Enable일 때만 돌게 끔 작업.
	//while (_TEngine_C->EngineEnable) {
	//	Sleep(100);
	//	bool _Urgency = false;

	//	// 만약 여기서 끊기면, 게임이 끊겼다고 생각하고 재실행 및 재접속 작업에 돌입한다.
	//	// 일단 될 때까지 무한 반복.
	//	// Server를 가지고 있지 않아도 통신이 끊긴건 일단 비상 상황이므로, 통신을 재개할 수단을 마련해야 한다.
	//	if (((_TEngine_C->_IsNoCVESProcess) ?
	//		0 : (_TEngine_C->_ProcessConfirm->IsProcessActive != true))
	//		&& _TEngine_C->EngineEnable == true) {
	//		// CVES 없어지고 다시 실행할 때까지 계속 돈다.
	//		// Server를 가지고 있지 않다면, 굳이 실행 할 필요가 없다.
	//		while (((_TEngine_C->_IsNoCVESProcess) ? 0 : !_TEngine_C->CheckingCVESProcess()) && _TEngine_C->EngineEnable == true) ;
	//		
	//		// Process가 시작될 때까지 기다린다.
	//		// 역시 Server를 가지고 있지 않다면, 굳이 실행 할 필요가 없다.
	//		while ((_TEngine_C->_IsNoCVESProcess) ? 0 : !_TEngine_C->_ProcessConfirm->IsProcessActive) ;
	//		
	//		// Server와 통신해야 하는 긴급한 상황(죽은 경우기 때문에).
	//		_Urgency = true;
	//	}

	//	if (_TEngine_C->_TelepathyClient->IsConnectedClient != true && _TEngine_C->EngineEnable == true) {
	//		// CVES에 접속할 때까지 계속 돈다.
	//		while (((_TEngine_C->_IsNoCVESProcess) ? 1 : _TEngine_C->_ProcessConfirm->IsProcessActive)
	//			&& _TEngine_C->EngineEnable == true) {
	//			if (_TEngine_C->Reconnect_Server())
	//				break;
	//		}

	//		if (_Urgency == true) {
	//			// 긴급한 상황.
	//			// Socket이 유효할 때, 복구가 가능한지를 CVES에 물어본다.
	//			_TEngine_C->_TelepathyClient->SendData("IsRestorePossible");
	//		}
	//	}
	//}
	_endthread();
	return 0;
}
#pragma endregion CVEC_CVESCheckingThread

#pragma region Command Queue Processing Thread
#if WINDOWS_SYS
UINT WINAPI
//DWORD WINAPI
#elif POSIX_SYS
// using pthread
void *
#endif
	EngineC::ClientCommandQueueProcessingThread(
#if WINDOWS_SYS
	LPVOID
	//void *
#elif POSIX_SYS
	void *
#endif
	Param) {
	EngineC *_TEngine_C = (EngineC *)Param;
	
	while (_TEngine_C->_TelepathyClient->IsConnectedClient) {
		Sleep(10);
		if (_TEngine_C->CommandQueue->empty() != true) {
			_TEngine_C->_QueueProtectMutex.lock();
			char _TStrBuffer[BUFFER_MAX_32767];
			memset(_TStrBuffer, NULL, sizeof(_TStrBuffer));
			strcpy(_TStrBuffer, _TEngine_C->CommandQueue->front());
			_TEngine_C->CommandQueue->pop();
			_TEngine_C->_QueueProtectMutex.unlock();

			// 내부 Protocol 송신(CVEC -> CVES).
			StringTokenizer *_StringTokenizer = new StringTokenizer();
			InternalProtocolSeeker _InternalProtocolSeeker;

			_StringTokenizer->SetInputCharString((const char *)_TStrBuffer);
			_StringTokenizer->SetSingleToken(" ");
			if (_StringTokenizer->StringTokenGo() == false)
				continue;
				//return ;

			StringTools _TStringTools;
			string _TString = string("");
			CommandString *_InternalProtocolCS = new CommandString(_StringTokenizer->GetTokenedCharListArrays());
			int _NSeek_CVESToCVEC = _InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_InternalProtocolCS->CharArrayListIter);

			switch (_NSeek_CVESToCVEC) {
				case VALUE_I_ALIVE :
					// 게임 재개.
					//G_EngineC->EnginePause = true;
					_TEngine_C->_TelepathyClient->SendData(STR_I_START);
					break;
				case VALUE_I_BUSY :
					// No Implement.
					break;
				case VALUE_I_MOVE :
					// UCI로 좌표 송신.
					_InternalProtocolCS->NextCharArrayIter();
					_TEngine_C->SendToGUI("bestmove %s", _InternalProtocolCS->CharArrayListIter);
					break;
				case VALUE_I_RESTOREOK :
					// 복구 완료.
					// 게임 재개.
					_TEngine_C->_TelepathyClient->SendData(STR_I_START);
					break;
				case VALUE_I_RESTORENOT :
					// 복구 미완료.
					// All Stop the game.
					// CVES Process를 죽인다.
					_TEngine_C->_TelepathyClient->SendData(STR_I_STOP);
					_TEngine_C->_TelepathyClient->SendData(STR_I_SERVERKILL);
					_TEngine_C->EngineEnable = false;
					break;
				case VALUE_I_SERVERISREADY :
					// CVES가 준비 되었음을 알림.
					_TEngine_C->_IsCVESReady = true;
					break;

				case VALUE_I_PTYPE :
					// 여기는 Client이므로 Info Type에 Client라고 실어 날려준다.
					// make "Info Type Client"
					_TString.append(STR_I_INFO);
					_TString.append(" ");
					_TString.append(STR_I_INFO_TYPE);
					_TString.append(" ");
					_TString.append(STR_I_INFO_TYPE_CLIENT);
					_TEngine_C->_TelepathyClient->SendData((char *)_TStringTools.StirngToConstCharPointer(_TString));
					//_TEngine_C->_TelepathyClient->SendData("Info Type Client");
					break;
			}

			delete _InternalProtocolCS;
			delete _StringTokenizer;
		}
	}
	_endthread();
	return 0;
}
#pragma endregion Command Queue Processing Thread

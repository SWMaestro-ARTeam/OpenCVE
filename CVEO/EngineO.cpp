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

#include "EngineO.hpp"

EngineO *G_EngineO;

EngineO::EngineO() {
	EngineEnable = false;
	EngineEnd = false;
	G_EngineO = this;
}

EngineO::~EngineO() {
	EngineEnable = false;
	EngineEnd = false;
	G_EngineO = NULL;
}

bool EngineO::Initialize_TClient() {
	_TelepathyClient = new Telepathy::Client();

	if (_TelepathyClient->ClientInitialize() != true) {
		// Server 연결 실패.
		//SendToGUI("Client Initialize Failed.");
		return false;
	}

	return true;
}

void EngineO::Deinitialize_TClient() {
	if (_TelepathyClient->IsConnectedClient == true)
		Disconnect_Server();
	Clear_ClientSocket();
}

void EngineO::Engine_Initializing() {
	// Initialize Client Socket.
	if (Initialize_TClient() != true) {
		//SendToGUI("Socket Initialization Failed.");
	}
}

void EngineO::Engine_DeInitializing() {

}

void EngineO::Clear_ClientSocket() {
	if (_TelepathyClient != NULL)
		delete _TelepathyClient;
}

bool EngineO::Connect_Server() {
	bool _TIsConnected = false;

	if (_TelepathyClient->IsInitializeClient == true) {
		if (_TelepathyClient->IsConnectedClient == true)
			return true;

		// Server 연결 성공시에, ClientReceivedCallback을 묶어
		// Receive 할 때 Server에서 전송된 내용을 받아야 한다.
		if (_TelepathyClient->ClientConnect()) {
			// Command Queue 생성.
			CommandQueue = new queue<char *>();

			_TelepathyClient->TClientReceivedCallback = ObserverReceivedCallback;
			_TelepathyClient->TClientDisconnectedCallback = ObserverDisconnectedCallback;
			_TelepathyClient->ClientReceiveStart();

			// Command 처리용 Thread를 생성.
			HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ObserverCommandQueueProcessingThread, this, 0, NULL);

			_TIsConnected = true;
		}
	}

	return _TIsConnected;
}

void EngineO::Disconnect_Server() {
	_TelepathyClient->ClientDisconnect();
	delete CommandQueue;
	_TelepathyClient->TClientReceivedCallback = NULL;
	_TelepathyClient->TClientDisconnectedCallback = NULL;
}

bool EngineO::Reconnect_Server() {
	if (Initialize_TClient() != true)
		//SendToGUI("Socket Initialize Failed.");

	return Connect_Server();
}

void EngineO::ObserverReceivedCallback(char *Buffer) {
	char _TBuffer[BUFFER_MAX_32767];
	memset(_TBuffer, NULL, sizeof(_TBuffer));
	strcpy(_TBuffer, Buffer);
	G_EngineO->CommandQueue->push(_TBuffer);
}

void EngineO::ObserverDisconnectedCallback() {
	bool _TIsConnected = false;
	// 3. Parser가 살아있을 때 까지 무조건 계속 while 돌며 Process가 살아있는지, 통신이 살아있는지 Check 함.
	// 간혹 Loop에 걸렸을때 갑자기 종료가 될 수 있기 때문에, 항상 전제를 Engine이 Enable일 때만 돌게 끔 작업.
	//if (G_EngineC->EngineEnable == false)
	//	G_EngineC->_TelepathyClient->IsConnectedClient = false;

	while (G_EngineO->EngineEnable) {
		if (G_EngineO->_TelepathyClient->IsConnectedClient != true && G_EngineO->EngineEnable == true) {
			// CVES에 접속할 때까지 계속 돈다.
			while (G_EngineO->EngineEnable == true) {
				if (G_EngineO->Reconnect_Server()) {
					_TIsConnected = true;
					break;
				}
			}

			if (_TIsConnected == true)
				break;
		}
		//Sleep(10);
	}
}

#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
	// using pthread
	void *
#endif
	EngineO::ObserverCommandQueueProcessingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif POSIX_SYS
	void *
#endif
	Param) {
	EngineO *_TEngine_O = (EngineO *)Param;

	while (_TEngine_O->EngineEnable != false) {
		if (_TEngine_O->CommandQueue->empty() != true && _TEngine_O->_TelepathyClient->IsConnectedClient) {
			//_TEngine_C->_QueueProtectMutex.lock();
			char _TStrBuffer[BUFFER_MAX_32767];
			memset(_TStrBuffer, NULL, sizeof(_TStrBuffer));
			strcpy(_TStrBuffer, _TEngine_O->CommandQueue->front());
			_TEngine_O->CommandQueue->pop();
			//_TEngine_C->_QueueProtectMutex.unlock();

			// 내부 Protocol 송신(CVEC -> CVES).
			StringTokenizer *_StringTokenizer = new StringTokenizer();

			_StringTokenizer->SetInputCharString((const char *)_TStrBuffer);
			_StringTokenizer->SetSingleToken(" ");
			if (_StringTokenizer->StringTokenGo() == false)
				continue;

			string _TString = string("");
			CommandString *_InternalProtocolCS = new CommandString(_StringTokenizer->GetTokenedCharListArrays());
			int _NSeek_CVESToCVEO = _TEngine_O->_InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_InternalProtocolCS->CharArrayListIter);

			switch (_NSeek_CVESToCVEO) {
				case VALUE_I_DISP :
					// 한꺼번에 처리가 가능하게 끔 CallBack으로 보내준다.
					while (_InternalProtocolCS->NextCharArrayIter()) {
						_TString.append(*_InternalProtocolCS->CharArrayListIter);
						if (_InternalProtocolCS->IsLastCharArrayIter() != true)
							_TString.append(" ");
					}
					_TEngine_O->TEngineODataReceivedCallback(_TEngine_O->_StringTools.StringToConstCharPointer(_TString));
					break;
				case VALUE_I_OMOVE :
					while (_InternalProtocolCS->NextCharArrayIter()) {
						_TString.append(*_InternalProtocolCS->CharArrayListIter);
						if (_InternalProtocolCS->IsLastCharArrayIter() != true)
							_TString.append(" ");
					}
					_TEngine_O->TEngineODataReceivedCallback(_TEngine_O->_StringTools.StringToConstCharPointer(_TString));
					break;
			}

			delete _InternalProtocolCS;
			delete _StringTokenizer;
		}
		Sleep(10);
	}
#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}

#pragma region CVEO Processing Thread
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
	Param) {
	EngineO *_TEngine_O = (EngineO *)Param;

	_TEngine_O->Engine_Initializing();
	_TEngine_O->Connect_Server();

	while (_TEngine_O->EngineEnable) ;

	_TEngine_O->Engine_DeInitializing();

	_TEngine_O->EngineEnd = true;

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
#pragma endregion CVEO Processing Thread

void EngineO::EngineO_Start() {
	_Thread.StartThread(CVEOProcessingThread, this);
	/*
#if defined(WINDOWS_SYS)
	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, CVEOProcessingThread, this, 0, NULL);
#elif defined(POSIX_SYS)
	pthread_t _TThread;
	pthread_attr_t _TThreadAttr;
	// pthread attribute initialize.
	pthread_attr_init(&_TThreadAttr);
	// Detached thread.
	pthread_attr_setdetachstate(&_TThreadAttr, PTHREAD_CREATE_DETACHED);
	// User space thread.
	pthread_attr_setscope(&_TThreadAttr, PTHREAD_SCOPE_SYSTEM);
	// Create thread.
	pthread_create(&_TThread, NULL, CVEOProcessingThread, (void *)this);
#endif
*/
}

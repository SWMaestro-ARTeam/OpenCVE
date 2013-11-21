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

#include "Telepathy.hpp"

#pragma region Server Class
// Telepathy Server Class Area.
Telepathy::Server *G_TelepathyServer;

#pragma region Constructor & Destructor
// constructor
Telepathy::Server::Server() {
	IsInitializeServer = false;
	IsServerStarted = false;
}

// destructor
Telepathy::Server::~Server() {
	if (IsInitializeServer == true){
		ServerClose();
	}
}
#pragma endregion Constructor & Destructor

#pragma region Server Threads
#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
void *
#endif
	Telepathy::Server::Server_ConnectionThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	while (1) 
		G_TelepathyServer->ServerListentoClient();

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}

#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
void *
#endif
	Telepathy::Server::Server_ReceivingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	SOCKET _CTlientSocket = (SOCKET) Param;

	while (1) {
		// 현재 Thread는 계속 받는다.
		if (G_TelepathyServer->ServerReceiving(_CTlientSocket) == false)
			break;
	}
#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
#pragma endregion Server Threads

// Server 초기화.
bool Telepathy::Server::ServerInitialize() {
	// 이 부분은 통째로 Windows용
	// 추후 다른 OS도 추가.

	// WSAStartUp
	if (WSAStartup(0x101, &_WSAData) != 0)
		return false;

	// using IPv4
	_ServerAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port 사용
	_ServerAddress.sin_port = htons((u_short)CVE_PORT);

	// Socket Create.
	_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	int _TOptionValue = 1;
	// TCP No Delay Option.
	setsockopt(_ServerSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&_TOptionValue, sizeof(_TOptionValue));

	// Socket이 잘못 되었다면..
	if (_ServerSocket == INVALID_SOCKET)
		return false;

	// socket bind.
	if (bind(_ServerSocket, (sockaddr *)&_ServerAddress, sizeof(_ServerAddress)) != 0) {
		ServerClose();
		return false;
	}

	// socket listen.
	if (listen(_ServerSocket, LISTEN_QUEUE) != 0) {
		ServerClose();
		return false;
	}

	// 외부 Receive 함수용.
	G_TelepathyServer = this;
	IsInitializeServer = true;

	return true;
}

// Server 기동.
bool Telepathy::Server::ServerStart() {
	bool _TIsStarted = false;
	if (IsInitializeServer != true) {
		// failed started server.
		_TIsStarted = false;
	}
	else {
		// Client 관리 Thread 시작.
		_Thread.StartThread(Server_ConnectionThread, NULL);
		/*
#if defined(WINDOWS_SYS)
		HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, Server_ConnectionThread, NULL, 0, NULL);
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
		pthread_create(&_TThread, NULL, Server_ConnectionThread, (void *)0);
#endif
		*/
		_TIsStarted = IsServerStarted = true;
	}
	return _TIsStarted;
}

// Server 종료.
void Telepathy::Server::ServerClose() {
	if (_ServerSocket != NULL){
		closesocket(_ServerSocket);
		WSACleanup();
		IsInitializeServer = false;
		IsServerStarted = false;
	}
}

// Client가 Server 접속시 Socket List에 붙여 Listen하게 하는 과정.
// User의 접속을 위하여 필요한 과정.
void Telepathy::Server::ServerListentoClient() {
	// Client 접속시, 접속 연결 기능.
	SOCKET _TSocket;
	SOCKADDR_IN _TClientAddress;
	int _TClientLength = sizeof(_TClientAddress);

	// Accept를 하여 Socket을 연결한다.
	_TSocket = accept(_ServerSocket, (struct sockaddr *)&_TClientAddress, &_TClientLength);

	int _TOptionValue = 1;
	// TCP No Delay Option.
	setsockopt(_TSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&_TOptionValue, sizeof(_TOptionValue));

	// Client Sockets를 넣어준다.
	if (_TSocket != INVALID_SOCKET) {
		ClientsList _TClientList;
		// Clientlist Initialize.
		_TClientList.ClientsListInitialize();
		// Client Address.
		_TClientList.ClientAddress = _TClientAddress;
		// Socket.
		_TClientList.ClientSocket = _TSocket;
		// Client Type.
		_TClientList.ClientType = "";
		// Client Name.
		_TClientList.ClientName = "";
		// push at lists back.
		ClientList.push_back(_TClientList);
		//ConnectorsSocketList.push_back(_TSocket);
		TAnyConnentionNotifier(_TSocket);
	}
	else
		return ;

	// Thread Begin.
	_Thread.StartThread(Server_ReceivingThread, 
#if defined(WINDOWS_SYS)
		(LPVOID)
#elif defined(POSIX_SYS)
		(void *)
#endif
		_TSocket);
	/*
#if defined(WINDOWS_SYS)
	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, Server_ReceivingThread, (LPVOID)_TSocket, 0, NULL);
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
	pthread_create(&_TThread, NULL, Server_ReceivingThread, (void *)_TSocket);
#endif
	*/
}

// Server가 Client들에게 정보를 받는 과정.
bool Telepathy::Server::ServerReceiving(SOCKET ClientSocket) {
	char _TBuffer[BUFFER_MAX_32767];
	int _TReadBufferLength;

	memset(_TBuffer, NULL, sizeof(_TBuffer));

	_TReadBufferLength =
#if defined(WINDOWS_SYS)
		recv(ClientSocket, _TBuffer, BUFFER_MAX_32767, 0);
#elif defined(POSIX_SYS)

#endif
	
	if (_TReadBufferLength == -1) {
		// ClientsList로 인하여 구현이 바뀜.
		// User Spacific하게 바꿀 수 있음.
		
		for_IterToEnd(list, ClientsList, ClientList) {
			if (_TVal->ClientSocket == ClientSocket) {
				// Iterator를 하나 더 만들어준다.
				list<ClientsList>::iterator _TClientListIter = _TVal;
				// Iterator가 위치를 잃어버려도 어차피 return하기 때문에 상관 없음.
				// 다른 Iterator를 만들어 이것을 하나 증가시켜 현재 Iterator의 위치가 end인지 검사한다.
				// 아니라면 삭제.
				if ((++_TClientListIter) != ClientList.end())
					_TVal = ClientList.erase(_TVal);
				else {
					// List에서 pop_back.
					ClientList.pop_back();
					// break를 넣어주지 않으면 죽어버린다.
					break;
				}
			}
		}
		return false;
	}
	else {
		// 만약 있다면, 다른 곳의 외부 함수 호출.
		// 정확히 말하면, Server를 쓰는 곳에서 호출.
		TServerReceivedCallback(_TBuffer, ClientSocket);
	}
	return true;
}

bool Telepathy::Server::SendDataToOne(char *Str, SOCKET ClientSocket) {
	// LIt is List Iterator.
	//list<SOCKET>::iterator _TLIt;
	int _TSendStatus = 0;

	// 지정된 Client에게 보냄.
	// STL List의 특성상, remove할 때 문제가 있으며, erase를 할 때도 Iterator에서 다음을 인식 할 수 없는 것에 매우 주의 하였다.
	// 그러므로 List의 끝에 있는 접속자를(begin에서 end로 iterator가 돌 경우) 삭제 할 때,
	// Error가 날 수 있다는 것을 전제하여 다음과 같이 Code를 만들었다.
	for_IterToEnd(list, ClientsList, ClientList) {
		// 일단 보낼 쪽의 Socket을 검사한다.
		if (_TVal->ClientSocket == ClientSocket) {
			_TSendStatus = send(_TVal->ClientSocket, Str, strlen(Str) + 1, 0);
			if (_TSendStatus == -1) {
				// Iterator를 하나 더 만들어준다.
				list<ClientsList>::iterator _TClientListIter = _TVal;
				// Iterator가 위치를 잃어버려도 어차피 return하기 때문에 상관 없음.
				// 다른 Iterator를 만들어 이것을 하나 증가시켜 현재 Iterator의 위치가 end인지 검사한다.
				// 아니라면 삭제.
				if ((++_TClientListIter) != ClientList.end())
					_TVal = ClientList.erase(_TVal);
				else {
					// List에서 pop_back.
					ClientList.pop_back();
					// break를 넣어주지 않으면 죽어버린다.
					break;
				}
				return false;
			}
			return true;
		}
	}
	return true;
}

void Telepathy::Server::SendDataToAll(char *Str) {
	// 모두에게 Message를 보낸다.
	// 이 함수는 어떤 Client의 Socket이 끊겼다면 해당 Client로의 전송이 실패할 수도 있기 때문에
	// 어떤 Client로의 Message 전송이 실패 할 수도 있다.
	// 즉, 전부 다 성공한다는 보장이 없다.
	int _TSendStatus = 0;

	for_IterToEnd(list, ClientsList, ClientList) {
		_TSendStatus = send(_TVal->ClientSocket, Str, strlen(Str) + 1, 0);
		if (_TSendStatus == -1) {
			// Iterator를 하나 더 만들어준다.
			list<ClientsList>::iterator _TClientListIter = _TVal;
			// Iterator가 위치를 잃어버려도 어차피 return하기 때문에 상관 없음.
			// 다른 Iterator를 만들어 이것을 하나 증가시켜 현재 Iterator의 위치가 end인지 검사한다.
			// 아니라면 삭제.
			if ((++_TClientListIter) != ClientList.end())
				_TVal = ClientList.erase(_TVal);
			else {
				// List에서 pop_back.
				ClientList.pop_back();
				// break를 넣어주지 않으면 죽어버린다.
				break;
			}
		}
	}
}
#pragma endregion Server Class

#pragma region Client Class
// Telepathy Client Class Area.
Telepathy::Client *G_TelepathyClient;

#pragma region Constructor & Destructor
// constructor
Telepathy::Client::Client(){
	IsInitializeClient = false;
	IsConnectedClient = false;

	G_TelepathyClient = this;
}

// deconstructor
Telepathy::Client::~Client(){
	if (IsConnectedClient == true)
		ClientClose();
	IsInitializeClient = false;
	G_TelepathyClient = NULL;
}
#pragma endregion Constructor & Destructor

#pragma region Client Threads
#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
void *
#endif
	Telepathy::Client::Client_ReceivingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	while (1) {
		if (G_TelepathyClient->ClientReceiving() == false) {
			G_TelepathyClient->ClientDisconnect();
			break;
		}
	}

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
#pragma endregion Client Threads

bool Telepathy::Client::ClientInitialize() {
	int _TOptVal;
	
	// WSAStartUp
	if (WSAStartup(0x101, &_WSAData) != 0)
		return false;

	// Socket Create.
	_ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Socket이 잘못 되었다면..
	if (_ClientSocket == INVALID_SOCKET)
		return false;

	int _TOptionValue = 1;
	// TCP No Delay Option.
	setsockopt(_ClientSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&_TOptionValue, sizeof(_TOptionValue));
	//if (setsockopt(_ClientSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&_TOptVal, sizeof(int)) == -1)
	//	return false;
	// Local IP Address.
	_Address = inet_addr(IP_ADDR_LOCAL);
	// get host entry.
	_HostEntry = gethostbyaddr((char *)&_Address, sizeof(_Address), AF_INET);

	if (_HostEntry == NULL){
		ClientClose();
		return false;
	}

	// using IPv4
	_ClientAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_ClientAddress.sin_addr.s_addr = *((unsigned long *)_HostEntry->h_addr);
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port 사용
	_ClientAddress.sin_port = htons((u_short)CVE_PORT);

	IsInitializeClient = true;
	return true;
}

void Telepathy::Client::ClientReceiveStart() {
	if (IsConnectedClient != true) {
		// failed started Client.
	}
	else {
		// Client 관리 Thread 시작.
		_Thread.StartThread(Client_ReceivingThread, NULL);
		/*
#if defined(WINDOWS_SYS)
		HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, Client_ReceivingThread, NULL, 0, NULL);
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
		pthread_create(&_TThread, NULL, Client_ReceivingThread, (void *)_ClientSocket);
#endif
		*/
	}
}

void Telepathy::Client::ClientClose() {
	if (_ClientSocket != NULL) {
		shutdown(_ClientSocket, 0x02); // BOTH.
		closesocket(_ClientSocket);
	}
	IsConnectedClient = false;
}

bool Telepathy::Client::ClientReceiving() {
	char _TBuffer[BUFFER_MAX_32767];
	int _TReadBufferLength;

	memset(_TBuffer, NULL, sizeof(_TBuffer));

	_TReadBufferLength =
#if defined(WINDOWS_SYS)
		recv(_ClientSocket, _TBuffer, BUFFER_MAX_32767, 0);
#elif defined(POSIX_SYS)

#endif

	if (_TReadBufferLength == -1) {
		return false;
	}
	else {
		// 만약 있다면, 다른 곳의 외부 함수 호출.
		// 정확히 말하면, Client를 쓰는 곳에서 호출.

		TClientReceivedCallback(_TBuffer);
	}
	return true;
}

bool Telepathy::Client::ClientConnect() {
	if (connect(_ClientSocket, (sockaddr *)&_ClientAddress, sizeof(_ClientAddress))) {
		ClientClose();
		return false;
	}

	// 외부 Receive 함수용.
	IsConnectedClient = true;

	return true;
}

void Telepathy::Client::ClientDisconnect() {
	ClientClose();
	TClientDisconnectedCallback();
	//IsConnectedClient = false;
}

bool Telepathy::Client::SendData(char *Str) {
	int _TSendStatus = 0;

	_TSendStatus = send(_ClientSocket, Str, strlen(Str)+1, 0);

	if (_TSendStatus == -1)
		return false;

	return true;
}
#pragma endregion Client Class
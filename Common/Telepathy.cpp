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
Telepathy::Server *TServer;

// constructor
Telepathy::Server::Server() {
	_M_BIsConnectedServer = false;
}

// destructor
Telepathy::Server::~Server() {
	if (_M_BIsConnectedServer == true){
		ServerClose();
	}
}

#pragma region Server Threads
#if WINDOWS_SYS
	#ifdef _AFXDLL
//UINT 
DWORD WINAPI
	#endif
#elif POSIX_SYS
void *
#endif
	Server_ConnectionThread(
#if WINDOWS_SYS
	LPVOID
#elif POSIX_SYS
	void *
#endif
	Param) {
	while (1) 
		TServer->ServerListentoClient();

	return 0;
}

#if WINDOWS_SYS
//UINT 
DWORD WINAPI
#elif POSIX_SYS
void *
#endif
	Server_ReceivingThread(
#if WINDOWS_SYS
	LPVOID
#elif POSIX_SYS
	void *
#endif
	Param) {
	SOCKET _ClientSocket = (SOCKET) Param;

	while (1) {
		// 현재 Thread는 계속 받는다.
		if (TServer->ServerReceiving(_ClientSocket) == false)
			break;
	}

	return 0;
}
#pragma endregion Server Threads

// Server 초기화.
bool Telepathy::Server::ServerInitialize() {
	// 이 부분은 통째로 Windows용
	// 추후 다른 OS도 추가.

	// WSAStartUp
	if (WSAStartup(0x101, &_M_WSAData) != 0)
		return false;

	// Socket Create.
	_M_HServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Socket이 잘못 되었다면..
	if (_M_HServerSocket = INVALID_SOCKET)
		return false;

	// using IPv4
	_M_ServerAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_M_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port 사용
	_M_ServerAddress.sin_port = htons((u_short)CVE_PORT);

	// socket bind.
	if (bind(_M_HServerSocket, (sockaddr *)&_M_ServerAddress, sizeof(_M_ServerAddress)) != 0) {
		ServerClose();
		return false;
	}

	// socket listen.
	if (listen(_M_HServerSocket, LISTEN_QUEUE) != 0) {
		ServerClose();
		return false;
	}

	// 외부 Receive 함수용.
	TServer = this;
	_M_BIsConnectedServer = true;

	return true;
}

// Server 기동.
void Telepathy::Server::ServerStart() {
	if (_M_BIsConnectedServer != true) {
		// failed started server.
	}
	else {
		// Client 관리 Thread 시작.
#if WINDOWS_SYS
	#ifdef _AFXDLL
		//AfxBeginThread(Server_ConnectionThread, 0);
		DWORD _TThreadID = 0;
		CreateThread(NULL, 0, Server_ConnectionThread, 0, 0, &_TThreadID);
	#endif
#elif POSIX_SYS
#endif
	}
}

// Server 종료.
void Telepathy::Server::ServerClose() {
	if (_M_HServerSocket != NULL){
		closesocket(_M_HServerSocket);
		WSACleanup();
		_M_BIsConnectedServer = false;
	}
}

// Client가 Server 접속시 Socket List에 붙여 Listen하게 하는 과정.
// User의 접속을 위하여 필요한 과정.
void Telepathy::Server::ServerListentoClient() {
	// Client 접속시, 접속 연결 기능.
	SOCKADDR_IN _ClientAddress;
	int _CLen = sizeof(_ClientAddress);

	// Accept를 하여 Socket을 연결한다.
	_M_HClientSocket = accept(_M_HServerSocket, (struct sockaddr *)&_ClientAddress, &_CLen);

	// Client Sockets를 넣어준다.
	if (_M_HClientSocket != INVALID_SOCKET)
		_M_HClientSocketArray.push_back(_M_HClientSocket);
	else
		return ;

	// Thread Begin.
#if WINDOWS_SYS
	// windows용.
	#ifdef _AFXDLL
	//AfxBeginThread(Server_ReceivingThread, (void *)_M_HClientSocket);
	DWORD _TThreadID = 0;
	CreateThread(NULL, 0, Server_ReceivingThread, (LPVOID)_M_HClientSocket, 0, &_TThreadID);
	#endif // _AFXDLL
#elif POSIX_SYS
	pthread_t _TThread;
	pthread_attr_t _TThreadAttr;
	// pthread attribute initialize.
	pthread_attr_init(&_TThreadAttr);
	// Detached thread.
	pthread_attr_setdetachstate(&_TThreadAttr, PTHREAD_CREATE_DETACHED);
	// User space thread.
	pthread_attr_setscope(&_TThreadAttr, PTHREAD_SCOPE_SYSTEM);
	// Create thread.
	pthread_create(&_TThread, NULL, Server_ReceivingThread, (void *)_M_HClientSocket);
#endif
}

// Server가 Client들에게 정보를 받는 과정.
bool Telepathy::Server::ServerReceiving(SOCKET ClientSocket) {
	char _Buffer[BUFFER_MAX_32767];
	int _ReadBufferLength;

	memset(_Buffer, NULL, sizeof(_Buffer));

	_ReadBufferLength =
#if WINDOWS_SYS
		recv(ClientSocket, _Buffer, BUFFER_MAX_32767, 0);
#elif POSIX_SYS

#endif
	
	if (_ReadBufferLength == -1) {
		// 만약 Length가 없다면, 해당 Client Socket 연결 삭제.
		_M_HClientSocketArray.remove(ClientSocket);
		return false;
	}
	else {
		// 만약 있다면, 다른 곳의 외부 함수 호출.
		// 정확히 말하면, Server를 쓰는 곳에서 호출.
		
		TServerReceivedCallback(_Buffer);
	}
	return true;
}

bool Telepathy::Server::SendData(char *Str) {
	// LIt is List Iterator.
	list<SOCKET>::iterator _LIt;
	int _SendStatus = 0;

	for (_LIt = _M_HClientSocketArray.begin();
		_LIt != _M_HClientSocketArray.end(); _LIt++) {
		_SendStatus = send(*_LIt, Str, strlen(Str)+1, 0);

		// Send 했을 때, Client가 끊겨 있다면(곧, _SendStatus가 -1일 때).
		// Socket List를 지워버린다.
		if (_SendStatus == -1)
			_M_HClientSocketArray.remove(*_LIt);
	}

	if (_SendStatus == -1)
		return false;

	return true;
}
#pragma endregion Server Class

#pragma region Client Class
// Telepathy Client Class Area.
Telepathy::Client *TClient;

// constructor
Telepathy::Client::Client(){
	IsInitializeClient = false;
	IsConnectedClient = false;
}

// deconstructor
Telepathy::Client::~Client(){
	if (IsConnectedClient == true)
		ClientClose();
	IsInitializeClient = false;
}

#pragma region Client Threads
#if WINDOWS_SYS
	#ifdef _AFXDLL
//UINT 
DWORD WINAPI
	#endif
#elif POSIX_SYS
void *
#endif
	Client_ReceivingThread(
#if WINDOWS_SYS
	LPVOID
#elif POSIX_SYS
	void *
#endif
	Param) {
	while (1) {
		if (TClient->ClientReceiving() == false)
			break;
	}

	return 0;
}
#pragma endregion Client Threads

bool Telepathy::Client::ClientInitialize() {
	// WSAStartUp
	if (WSAStartup(0x101, &_M_WSAData) != 0)
		return false;

	// Socket Create.
	_M_HClientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Socket이 잘못 되었다면..
	if (_M_HClientSocket == INVALID_SOCKET)
		return false;

	// Local IP Address.
	_M_Address=inet_addr(IP_ADDR_LOCAL);
	// get host entry.
	_M_HostEntry=gethostbyaddr((char*)&_M_Address, sizeof(_M_Address), AF_INET);

	if (_M_HostEntry == NULL){
		ClientClose();
		return false;
	}

	// using IPv4
	_M_ClientAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_M_ClientAddress.sin_addr.s_addr = *((unsigned long*)_M_HostEntry->h_addr);
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port 사용
	_M_ClientAddress.sin_port = htons((u_short)CVE_PORT);

	IsInitializeClient = true;
	return true;
}

void Telepathy::Client::ClientReceiveStart() {
	if (IsConnectedClient != true) {
		// failed started Client.
	}
	else {
		// Client 관리 Thread 시작.
#if WINDOWS_SYS
	#ifdef _AFXDLL
		//AfxBeginThread(Client_ReceivingThread, 0);
		DWORD _TThreadID = 0;
		CreateThread(NULL, 0, Client_ReceivingThread, 0, 0, &_TThreadID);
	#endif
#elif POSIX_SYS
		pthread_t _TThread;
		pthread_attr_t _TThreadAttr;
		// pthread attribute initialize.
		pthread_attr_init(&_TThreadAttr);
		// Detached thread.
		pthread_attr_setdetachstate(&_TThreadAttr, PTHREAD_CREATE_DETACHED);
		// User space thread.
		pthread_attr_setscope(&_TThreadAttr, PTHREAD_SCOPE_SYSTEM);
		// Create thread.
		pthread_create(&_TThread, NULL, Server_ReceivingThread, (void *)_M_HClientSocket);
#endif
	}
}

void Telepathy::Client::ClientClose() {
	if (_M_HClientSocket != NULL) {
		closesocket(_M_HClientSocket);
	}
	IsConnectedClient = false;
}

bool Telepathy::Client::ClientReceiving() {
	char _Buffer[BUFFER_MAX_32767];
	int _ReadBufferLength;

	memset(_Buffer, NULL, sizeof(_Buffer));

	_ReadBufferLength =
#if WINDOWS_SYS
		recv(_M_HClientSocket, _Buffer, BUFFER_MAX_32767, 0);
#elif POSIX_SYS

#endif

	if (_ReadBufferLength == -1) {
		return false;
	}
	else {
		// 만약 있다면, 다른 곳의 외부 함수 호출.
		// 정확히 말하면, Client를 쓰는 곳에서 호출.

		TClientReceivedCallback(_Buffer);
	}
	return true;
}

bool Telepathy::Client::ClientConnect() {
	if (connect(_M_HClientSocket, (sockaddr *)&_M_ClientAddress, sizeof(_M_ClientAddress))){
		ClientClose();
		return false;
	}

	// 외부 Receive 함수용.
	TClient = this;
	IsConnectedClient = true;

	return true;
}

void Telepathy::Client::ClientDisconnect() {
	ClientClose();
}

bool Telepathy::Client::SendData(char *Str) {
	int _SendStatus = 0;

	_SendStatus = send(_M_HClientSocket, Str, strlen(Str)+1, 0);

	if (_SendStatus == -1)
		return false;

	return true;
}

#pragma endregion Client Class
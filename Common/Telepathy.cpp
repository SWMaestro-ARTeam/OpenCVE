//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright © 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
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

// Telepathy Server Area.
Telepathy::Server TServer;

// constructor
Telepathy::Server::Server() {
	_M_BIsConnectedServer = false;

	if (ServerInitialize() == false){
		// debug 넣어주어야 함.
		return ;
	}
	else
		_M_BIsConnectedServer = true;

	// Server Call 등록
	_T_SERVERRECEIVEDCALLBACK _ServCall;
}

// destructor
Telepathy::Server::~Server() {
	ServerClosing();
}

#pragma region Server Threads
UINT Server_ConnectionThread(LPVOID Param) {
	while (1) 
		TServer.ServerListentoClient();

	return 0;
}

UINT Server_ReceivingThread(LPVOID Param) {
	SOCKET _ClientSocket = (SOCKET) Param;

	while (1) {
		// 현재 Thread는 계속 받는다.
		if (TServer.ServerReceiving(_ClientSocket) == false)
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
		ServerClosing();
		return false;
	}

	// socket listen.
	if (listen(_M_HServerSocket, LISTEN_QUEUE) != 0) {
		ServerClosing();
		return false;
	}

	return true;
}

// Server 종료.
void Telepathy::Server::ServerClosing() {
	if (_M_HServerSocket != NULL){
		closesocket(_M_HServerSocket);
		WSACleanup();
		_M_BIsConnectedServer = false;
	}
}

// Server 기동.
void Telepathy::Server::ServerStart() {
	if (_M_BIsConnectedServer != true) {
		// failed started server.
	}
	else {
		// Client 관리 Thread 시작.
#if WINDOWS
		AfxBeginThread(Server_ConnectionThread, 0);
#elif OTHER
#endif
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
#if WINDOWS
	// windows용.
	AfxBeginThread(Server_ReceivingThread, (void *)_M_HClientSocket);
#elif OTHER
#endif
}

// Server가 Client들에게 정보를 받는 과정.
bool Telepathy::Server::ServerReceiving(SOCKET ClientSocket) {
	
	char _Buffer[BUFFER_MAX_32767];
	int _ReadBufferLength;

	memset(_Buffer, NULL, sizeof(_Buffer));

	_ReadBufferLength =
#if WINDOWS
	recv(ClientSocket, _Buffer, BUFFER_MAX_32767, 0);
#elif OTHER
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

// Telepathy Client Area.
Telepathy::Client TClient;

// constructor
Telepathy::Client::Client(){
	_M_BIsConnectedClient = false;
}

// deconstructor
Telepathy::Client::~Client(){
	if (_M_BIsConnectedClient == true) {
		ClientClosing();
		_M_BIsConnectedClient = false;
	}
}

#pragma region Client Threads
UINT Client_ReceivingThread(LPVOID Param) {
	while (1) {
		if (TClient.ClientReceiving() == false)
			break;
	}

	return 0;
}
#pragma endregion Client Threads

void Telepathy::Client::ClientStart() {
	if (_M_BIsConnectedClient != true) {
		// failed started Client.
	}
	else {
		// Client 관리 Thread 시작.
#if WINDOWS
		AfxBeginThread(Client_ReceivingThread, 0);
#elif OTHER
#endif
	}
}

void Telepathy::Client::ClientClosing() {
	if (_M_HClientSocket != NULL)
		closesocket(_M_HClientSocket);
}

bool Telepathy::Client::ClientInitialize() {
	// WSAStartUp
	if (WSAStartup(0x101, &_M_WSAData) != 0)
		return false;

	// Socket Create.
	_M_HClientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Socket이 잘못 되었다면..
	if (_M_HClientSocket = INVALID_SOCKET)
		return false;

	// Local IP Address.
	_M_Address=inet_addr(IP_ADDR_LOCAL);
	// get host entry.
	_M_HostEntry=gethostbyaddr((char*)&_M_Address, sizeof(_M_Address), AF_INET);

	if (_M_HostEntry == NULL){
		ClientClosing();
		return false;
	}

	// using IPv4
	_M_ClientAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_M_ClientAddress.sin_addr.s_addr = *((unsigned long*)_M_HostEntry->h_addr);
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port 사용
	_M_ClientAddress.sin_port = htons((u_short)CVE_PORT);

	if (connect(_M_HClientSocket, (sockaddr *)&_M_ClientAddress, sizeof(_M_ClientAddress))){
		ClientClosing();
		return false;
	}

	_M_BIsConnectedClient = true;

	return true;
}

bool Telepathy::Client::ClientReceiving() {

	char _Buffer[BUFFER_MAX_32767];
	int _ReadBufferLength;

	memset(_Buffer, NULL, sizeof(_Buffer));

	_ReadBufferLength =
#if WINDOWS
		recv(_M_HClientSocket, _Buffer, BUFFER_MAX_32767, 0);
#elif OTHER
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
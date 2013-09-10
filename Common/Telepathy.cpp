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
}

// destructor
Telepathy::Server::~Server() {
	ServerClosing();
}

#pragma region Threads
UINT Server_ConnectionThread(LPVOID Param) {
	while (1) 
		TServer.ServerListentoClient();

	return 0;
}

UINT Server_ClientInteractionThread(LPVOID Param) {
	SOCKET _ClientSocket = (SOCKET) Param;

	while (1) {
		// 현재 Thread는 계속 받는다.
		if (TServer.ServerReceiving(_ClientSocket) == false)
			break;
	}
}
#pragma endregion Threads

// Server 초기화.
bool Telepathy::Server::ServerInitialize() {
	if (WSAStartup(0x101, &_M_WSAData) != 0)
		return ;

	// using IPv4
	_M_ServerAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_M_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port 사용
	_M_ServerAddress.sin_port = htons((u_short)CVE_PORT);
	// Socket Create.
	_M_HServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Socket이 잘못 되었다면..
	if (_M_HServerSocket = INVALID_SOCKET)
		return false;

	// socket bind.
	if (bind(_M_HServerSocket, (sockaddr *)&_M_ServerAddress, sizeof(_M_ServerAddress)) != 0)
		return false;

	// socket listen.
	if (listen(_M_HServerSocket, LISTEN_QUEUE) != 0)
		return false;
}

// Server 종료.
bool Telepathy::Server::ServerClosing() {
	closesocket(_M_HServerSocket);
	WSACleanup();
}

// Server 기동.
void Telepathy::Server::ServerStart() {
	if (_M_BIsConnectedServer != true) {
		// failed started server.
	}
	else {
		// Client 관리 Thread 시작.
		AfxBeginThread(Server_ConnectionThread, 0);
	}
}

// Client가 Server 접속시 Socket List에 붙여 Listen하게 하는 과정.
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

	AfxBeginThread(Server_ClientInteractionThread, (void *)_M_HClientSocket);
}

// Server가 Client들에게 정보를 받는 과정.
bool Telepathy::Server::ServerReceiving(int ClientSocket) {
	
	char _Buffer[BUFFER_MAX_32767];
	int _ReadBufferLength;

	memset(_Buffer, NULL, sizeof(_Buffer));
	_ReadBufferLength = recv(ClientSocket, _Buffer, BUFFER_MAX_32767, 0);

	if (_ReadBufferLength == -1) {
		_M_HClientSocketArray.remove(ClientSocket);
		return false;
	}
	else {
		
	}
	return true;
}
// Telepathy Client Area.


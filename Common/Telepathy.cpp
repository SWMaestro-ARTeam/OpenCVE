#include "Telepathy.hpp"

// Telepathy Server Area.

Telepathy::Server TServer;

// constructor
Telepathy::Server::Server() {
	_M_BIsConnectedServer = false;

	if (ServerInitialize() == false){
		// debug �־��־�� ��.
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
		// ���� Thread�� ��� �޴´�.
		if (TServer.ServerReceiving(_ClientSocket) == false)
			break;
	}
}
#pragma endregion Threads

// Server �ʱ�ȭ.
bool Telepathy::Server::ServerInitialize() {
	if (WSAStartup(0x101, &_M_WSAData) != 0)
		return ;

	// using IPv4
	_M_ServerAddress.sin_family = AF_INET;
	// 32bit IPv4 address
	_M_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	//_M_ServerAddress.sin_addr.s_addr = inet_addr(IP_ADDR_LOCAL);
	// port ���
	_M_ServerAddress.sin_port = htons((u_short)CVE_PORT);
	// Socket Create.
	_M_HServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Socket�� �߸� �Ǿ��ٸ�..
	if (_M_HServerSocket = INVALID_SOCKET)
		return false;

	// socket bind.
	if (bind(_M_HServerSocket, (sockaddr *)&_M_ServerAddress, sizeof(_M_ServerAddress)) != 0)
		return false;

	// socket listen.
	if (listen(_M_HServerSocket, LISTEN_QUEUE) != 0)
		return false;
}

// Server ����.
bool Telepathy::Server::ServerClosing() {
	closesocket(_M_HServerSocket);
	WSACleanup();
}

// Server �⵿.
void Telepathy::Server::ServerStart() {
	if (_M_BIsConnectedServer != true) {
		// failed started server.
	}
	else {
		// Client ���� Thread ����.
		AfxBeginThread(Server_ConnectionThread, 0);
	}
}

// Client�� Server ���ӽ� Socket List�� �ٿ� Listen�ϰ� �ϴ� ����.
void Telepathy::Server::ServerListentoClient() {
	// Client ���ӽ�, ���� ���� ���.
	SOCKADDR_IN _ClientAddress;
	int _CLen = sizeof(_ClientAddress);

	// Accept�� �Ͽ� Socket�� �����Ѵ�.
	_M_HClientSocket = accept(_M_HServerSocket, (struct sockaddr *)&_ClientAddress, &_CLen);

	// Client Sockets�� �־��ش�.
	if (_M_HClientSocket != INVALID_SOCKET)
		_M_HClientSocketArray.push_back(_M_HClientSocket);
	else
		return ;

	AfxBeginThread(Server_ClientInteractionThread, (void *)_M_HClientSocket);
}

// Server�� Client�鿡�� ������ �޴� ����.
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


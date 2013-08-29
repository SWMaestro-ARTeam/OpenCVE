#ifndef _Telepathy_hpp_
#define _Telepathy_hpp_

#include "Common.hpp"

#ifdef WINDOWS
#include <winsock2.h>
// ws2_32.lib ��ũ
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;

// �츮 Telepathy����.. Server�� Client�� �־��.. ��?! ��������
class Telepathy {
private:

public:
	class Server {
	private:
#ifdef WINDOWS
		WSADATA _M_WSAData;
		SOCKET _M_HServSock;
		SOCKADDR_IN _M_ServAddr;

		SOCKET _M_HSockArray[WSA_MAXIMUM_WAIT_EVENTS];
		SOCKET _M_HClntSock;
		SOCKADDR_IN _M_ClntAddr;

		WSAEVENT _M_HEventArray[WSA_MAXIMUM_WAIT_EVENTS];
		WSAEVENT _M_NewEvent;
		WSANETWORKEVENTS _M_NetEvents;
#endif
	public:

	};

	class Client {
	private:

	public:
	};
};

#endif
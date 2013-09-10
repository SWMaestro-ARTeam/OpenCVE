#ifndef _Telepathy_hpp_
#define _Telepathy_hpp_

#include <list>

#include "Common.hpp"

#ifdef WINDOWS
#include <afxwin.h>
#include <winsock2.h>
// ws2_32.lib 링크
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;

// 우리 Telepathy에는.. Server와 Client가 있어요.. 응?! ㄷㄷㄷㄷ
class Telepathy {
private:
	
public:
	class Server {
	private:
#ifdef WINDOWS
		typedef struct _ClientsList {
			SOCKADDR_IN ClientAddress;
			SOCKET ClientSocket;
			int UserNumber;
			char *UserName;
		} ClientsList;

		WSADATA _M_WSAData;
		SOCKET _M_HServerSocket;
		SOCKADDR_IN _M_ServerAddress;

		list<SOCKET> _M_HClientSocketArray;
		//list<ClientsList> _M_HClientLists;
		SOCKET _M_HClientSocket;
		//SOCKADDR_IN _M_ClntAddr;

		/*
		WSAEVENT _M_HEventArray[WSA_MAXIMUM_WAIT_EVENTS];
		WSAEVENT _M_NewEvent;
		WSANETWORKEVENTS _M_NetEvents;
		*/
		bool _M_BIsConnectedServer;
#endif
	public:
		Server();
		~Server();

		void ServerStart();
		bool ServerInitialize();
		bool ServerClosing();
		void ServerListentoClient();
		bool ServerReceiving(int ClientSocket);
	};

	class Client {
	private:

	public:
		Client();
		~Client();


	};
};

#endif
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

#ifndef _Telepathy_hpp_
#define _Telepathy_hpp_

#define _AFXDLL
#include <list>

#include "Common.hpp"

#if WINDOWS
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
		typedef struct _ClientsList {
#if WINDOWS
			SOCKADDR_IN ClientAddress;
			SOCKET ClientSocket;
#elif OTHER
#endif
			int UserNumber;
			char *UserName;
		} ClientsList;
#if WINDOWS
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
#elif OTHER
#endif
		bool _M_BIsConnectedServer;

	public:
		Server();
		~Server();

		bool ServerInitialize();
		void ServerStart();
		void ServerClosing();
		void ServerListentoClient();
		bool ServerReceiving(SOCKET ClientSocket);
	};

	class Client {
	private:
		unsigned int _M_Address;
		HOSTENT *_M_HostEntry;
		WSADATA _M_WSAData;
		SOCKADDR_IN _M_Client;

		bool _M_BIsConnectedClient;
	public:
		Client();
		~Client();

		bool ClientInitialize();
		void ClientStart();
		void ClientClosing();
		
	};
};

// server Callback
// __stdcall
extern void CALLBACK _ServerExProc(char *Buffer);
typedef void (* _T_SERVERCALLBACK)(char *Buffer);

extern void _ClientExProc(char *Buffer);
#endif
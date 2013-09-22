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

#ifndef _Telepathy_hpp_
#define _Telepathy_hpp_

#include "Common.hpp"

#include <list>

#if WINDOWS
//#include <afxwin.h>
#include <winsock2.h>
// ws2_32.lib 링크
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;

class Telepathy {
private:
	
public:
	// Server Class
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
#elif OTHER
#endif
		bool _M_BIsConnectedServer;

	public:
		Server();
		~Server();

		// server Callback
		typedef void (* _T_SERVERRECEIVEDCALLBACK)(char *Buffer);

		bool ServerInitialize();
		void ServerStart();
		void ServerClose();
		void ServerListentoClient();
		bool ServerReceiving(SOCKET ClientSocket);

		bool SendData(char *Str);

		// Server Receive Callback Pointer.
		_T_SERVERRECEIVEDCALLBACK TServerReceivedCallback;
	};

	// Client Class
	class Client {
	private:
		unsigned int _M_Address;
#if WINDOWS
		HOSTENT *_M_HostEntry;
		WSADATA _M_WSAData;
		SOCKET _M_HClientSocket;
		SOCKADDR_IN _M_ClientAddress;
#elif OTHER
#endif		
		bool _M_BIsConnectedClient;
	public:
		Client();
		~Client();

		// client Callback
		typedef void (* _T_CLIENTRECEIVEDCALLBACK)(char *Buffer);

		bool ClientInitialize();
		void ClientStart();
		void ClientClose();
		bool ClientReceiving();

		bool SendData(char *Str);

		// Client Receive Callback Pointer.
		_T_CLIENTRECEIVEDCALLBACK TClientReceivedCallback;
	};
};
#endif
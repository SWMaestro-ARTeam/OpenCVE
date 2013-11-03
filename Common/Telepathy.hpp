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

#include "ExtendedBlackBox.hpp"

#include <list>

#if WINDOWS_SYS
//	#ifdef _AFXDLL
//#include <afxwin.h>
//	#else
		#if MINGW_USING
#include <winsock.h>
//#include <winsock2.h>
		#else
#include <windows.h>
		#endif
#include <tchar.h>
	//#endif
// ws2_32.lib 링크
#pragma comment(lib, "ws2_32.lib")
#elif POSIX_SYS
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>
#endif

using namespace std;

typedef struct _ClientsList {
#if WINDOWS_SYS
	SOCKADDR_IN ClientAddress;
	SOCKET ClientSocket;
#elif POSIX_SYS

#endif
	int ClientNumber;
	// 이름 및 type의 최대 길이는 32까지.
	char *ClientType;
	char *ClientName;

	void ClientsListInitialize() {
		ClientNumber = -1;
		ClientType = new char[BUFFER_MAX_32];
		ClientName = new char[BUFFER_MAX_32];
	}

	void ClientsListDeinitialize() {
		ClientNumber = -1;
		delete ClientType;
		delete ClientName;
	}
} ClientsList;

class Telepathy {
private:
	
public:
	// Server Class
	class Server {
	private:
#if WINDOWS_SYS
		WSADATA _WSAData;
		SOCKET _ServerSocket;
		SOCKADDR_IN _ServerAddress;
#elif POSIX_SYS

#endif

	public:
		Server();
		~Server();

		//list<SOCKET> ConnectorsSocketList;
		list<ClientsList> ClientList;

		bool IsInitializeServer;
		bool IsServerStarted;

		// server Callback
		typedef void (* _T_SERVERRECEIVEDCALLBACK)(char *Buffer, SOCKET ClientSocket);

		bool ServerInitialize();
		bool ServerStart();
		void ServerClose();
		void ServerListentoClient();
		bool ServerReceiving(SOCKET ClientSocket);

		bool SendDataToOne(char *Str, SOCKET ClientSocket);
		void SendDataToAll(char *Str);

		// Server Receive Callback Pointer.
		_T_SERVERRECEIVEDCALLBACK TServerReceivedCallback;
	};

	// Client Class
	class Client {
	private:
		unsigned int _Address;
#if WINDOWS_SYS
		HOSTENT *_HostEntry;
		WSADATA _WSAData;
		SOCKET _ClientSocket;
		SOCKADDR_IN _ClientAddress;
#elif POSIX_SYS

#endif		
	public:
		Client();
		~Client();

		bool IsInitializeClient;
		bool IsConnectedClient;

		// client Callback
		typedef void (* _T_CLIENTRECEIVEDCALLBACK)(char *Buffer);

		bool ClientInitialize();
		void ClientReceiveStart();
		void ClientClose();
		bool ClientReceiving();

		bool ClientConnect();
		void ClientDisconnect();
		//bool ClientReConnect();

		bool SendData(char *Str);

		// Client Receive Callback Pointer.
		_T_CLIENTRECEIVEDCALLBACK TClientReceivedCallback;
	};
};
#endif

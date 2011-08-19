
#define _WINSOCKAPI_    // stops windows.h including winsock.h

#include <windows.h>
#include <stdio.h>

#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")
#pragma once
 
struct remotepost_file{
	char filename[1024];
	long size;
	char postname[32];
	remotepost_file *next;
};

class CSocket
{
       //-------------------------------------------------------------------
       // Variable declarations
       //-------------------------------------------------------------------
       private: WSADATA              m_wsaData; // start data
       private: SOCKET                     m_sListeningSocket; // server listening socket
       private: SOCKADDR_IN              m_siServerInfo; // server information
       private: BOOL                     m_bServerOnline; // is server online
 
       public:  CHAR*                     m_cRecievePacket; // buffer that the recv from any client will be put into
 
       //-------------------------------------------------------------------
       // Constructor / Destructor
       //-------------------------------------------------------------------
       public:                CSocket(); // constructor
       public:                ~CSocket(); // destructor
       
       public:  INT              disconnectSocket(SOCKET m_sConnection); // close connection
       public:  INT              disconnect(); // close connection
 
       //-------------------------------------------------------------------
       // Server functions
       //-------------------------------------------------------------------
       public:  BOOL              initializeServer(INT m_iPort); // initialize a server
       public:  SOCKET              acceptClient(); // accept client - this should be looped as it is the listener for clients
       public:  CHAR*              clientRecieve(SOCKET m_sClient, int *size); // recieve from a client - buffer will be moved to m_cRecievePacket
       public:  INT              clientSend(SOCKET m_sClient, CHAR* m_cPacket); // send packet/buffer to client / socket
	   public:  INT              clientSend(SOCKET m_sClient, BYTE* m_cPacket, int len); // send packet/buffer to client / socket

       //-------------------------------------------------------------------
       // Remote connections functions
       //-------------------------------------------------------------------
       public:  SOCKET              remoteConnect(CHAR* m_IP, INT m_iPort); // connect to ip / host
       public:  INT              remoteRecieve(SOCKET m_sConnection, CHAR* m_cRemoteBuffer, INT m_iLength); // recieve buffer
       public:  CHAR*              remoteRetrieveCookies(CHAR* m_cRemoteBuffer); // retrieve cookies from buffer
       
       public:  INT              remoteGET(SOCKET m_sConnection, CHAR* m_cHost, CHAR* m_cPath, CHAR* m_cReferer, CHAR* m_cCookies); // GET a page, basically visit a website
       public:  INT              remotePOST(SOCKET m_sConnection, CHAR* m_cHost, CHAR* m_cPath, CHAR* m_cData, CHAR* m_cReferer, CHAR* m_cCookies); // POST a form, for posting forms on a web-site
	   public:	INT				 remotePOSTFile(SOCKET m_sConnection, CHAR* m_cHost, CHAR* m_cPath, remotepost_file* m_cData, CHAR* m_cReferer, CHAR* m_cCookies);
};

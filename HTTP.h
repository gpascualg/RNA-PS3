#include <psl1ght/lv2/net.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <sys/stat.h>

#pragma once
 
class CSocket
{
private:

    //-------------------------------------------------------------------
    // Variable declarations
    //-------------------------------------------------------------------
	int conn_s;
	int list_s;

	sockaddr_in m_siServerInfo; // server information
    bool m_bServerOnline; // is server online
	char *m_cRecievePacket; // buffer that the recv from any client will be put into
 
public: 
	//-------------------------------------------------------------------
	// Constructor / Destructor
	//-------------------------------------------------------------------
	CSocket(); // constructor
	~CSocket(); // destructor
       
	int disconnectSocket(int m_sConnection); // close connection
	int disconnect(); // close connection
 
    //-------------------------------------------------------------------
    // Server functions
    //-------------------------------------------------------------------
    bool initializeServer(int m_iPort); // initialize a server
    int acceptClient(); // accept client - this should be looped as it is the listener for clients
	char *clientRecieve(int m_sClient, int *size); // recieve from a client - buffer will be moved to m_cRecievePacket
	int clientSend(int m_sClient, char* m_cPacket); // send packet/buffer to client / socket
	int clientSend(int m_sClient, u8* m_cPacket, int len); // send packet/buffer to client / socket

    //-------------------------------------------------------------------
    // Remote connections functions
    //-------------------------------------------------------------------
	int remoteConnect(char* m_IP, int m_iPort); // connect to ip / host
	int remoteRecieve(int m_sConnection, char* m_cRemoteBuffer, int m_iLength); // recieve buffer
	char *remoteRetrieveCookies(char* m_cRemoteBuffer); // retrieve cookies from buffer
       
	int remoteGET(int m_sConnection, char* m_cHost, char* m_cPath, char* m_cReferer, char* m_cCookies); // GET a page, basically visit a website
	int remotePOST(int m_sConnection, char* m_cHost, char* m_cPath, char* m_cData, char* m_cReferer, char* m_cCookies); // POST a form, for posting forms on a web-site

	char *clearBuffer(char* buffer);
};

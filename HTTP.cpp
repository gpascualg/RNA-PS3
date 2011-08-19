/*
 
  CSocket Class by OneWhoSighs
  www.b0ts.org
 
  This class was inspired by Waryas's original library. I was bored.
 
*/
 
#include "HTTP.h"

//-------------------------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------------------------
 
CSocket::CSocket()
{
       WSAStartup(MAKEWORD(2,2), &m_wsaData);
       m_cRecievePacket = (CHAR*)malloc(4096);
}
 
CSocket::~CSocket()
{
       WSACleanup();
       free(m_cRecievePacket);
}
 
INT CSocket::disconnectSocket(SOCKET m_sConnection)
{
       INT iReturn = closesocket(m_sConnection);
 
       if(m_sConnection == m_sListeningSocket)
              m_bServerOnline = FALSE;
 
       if(iReturn == 0)
              return 1;
       else
              return 0;
}
INT CSocket::disconnect(){
	closesocket(m_sListeningSocket);
    m_bServerOnline = FALSE;
	return 1;
}
//-------------------------------------------------------------------
// Server functions
//-------------------------------------------------------------------
 
BOOL CSocket::initializeServer(INT m_iPort)
{
       m_sListeningSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
 
       m_siServerInfo.sin_family = AF_INET;
       m_siServerInfo.sin_addr.s_addr = INADDR_ANY; 
       m_siServerInfo.sin_port = htons(m_iPort);

       if(bind(m_sListeningSocket, (LPSOCKADDR)&m_siServerInfo, sizeof(struct sockaddr)) != SOCKET_ERROR)
       {
              m_bServerOnline = TRUE;
              return TRUE;
       }
 
       return FALSE;
}
 
SOCKET CSocket::acceptClient()
{
       if(m_bServerOnline)
       {
              if(listen( m_sListeningSocket, SOMAXCONN ) == SOCKET_ERROR)
                     return NULL;
 
              SOCKET m_sClient;
              INT m_iLength;
 
              m_iLength = sizeof m_siServerInfo;
 
              m_sClient = accept( m_sListeningSocket, (LPSOCKADDR)&m_siServerInfo, &m_iLength );
 
              if(m_sClient != SOCKET_ERROR)
                     return m_sClient;
 
              return NULL;
       }
       else
       {
              return NULL;
       }
}
 
CHAR* CSocket::clientRecieve(SOCKET m_sClient, int *size)
{
       if(m_bServerOnline)
       {
              memset(m_cRecievePacket,0,4096);
 
              INT m_iReturn = recv(m_sClient,m_cRecievePacket,4096,0);
              
              if(m_iReturn > 0){
					 if(size)
						 *size = m_iReturn;

                     return m_cRecievePacket;
			  }

              if(m_iReturn == SOCKET_ERROR)
                     return NULL;

              return NULL;
       }
       else
       {
              return NULL;
       }
}
 
INT CSocket::clientSend(SOCKET m_sClient, CHAR* m_cPacket)
{
       return send(m_sClient,m_cPacket,strlen(m_cPacket),0);
}
INT CSocket::clientSend(SOCKET m_sClient, BYTE* m_cPacket, int len){
	return send(m_sClient,(char*)m_cPacket,len,0);
}
 
//-------------------------------------------------------------------
// Remote connections functions
//-------------------------------------------------------------------
 
SOCKET CSocket::remoteConnect(CHAR* m_IP, INT m_iPort)
{
       SOCKET m_sConnection;
       struct sockaddr_in m_siSin;
       struct hostent* m_hHost;
 
       m_sConnection = socket( AF_INET, SOCK_STREAM, 0 );
	   
       m_hHost = gethostbyname(m_IP);

	   if(m_hHost == NULL)
		   return NULL;
	   
       memset( &m_siSin, 0, sizeof m_siSin );
	   
	   m_siSin.sin_family = AF_INET;
       m_siSin.sin_addr = *(struct in_addr *)m_hHost->h_addr;
       m_siSin.sin_port = htons( m_iPort );
	   
       if ( !connect( m_sConnection, (SOCKADDR *)&m_siSin, sizeof(m_siSin)) )
       {
              return m_sConnection;
       }
       else
       {
              return NULL;
       }
 
       return NULL;
}

INT CSocket::remoteRecieve(SOCKET m_sConnection, CHAR* m_cRemoteBuffer, INT m_iLength)
{
       memset(m_cRemoteBuffer,0,m_iLength);
       INT m_iReturn = recv(m_sConnection,m_cRemoteBuffer,m_iLength,0);
 
       return m_iReturn;
}
 
CHAR* CSocket::remoteRetrieveCookies(CHAR* m_cRemoteBuffer)
{
       CHAR* m_cCookies = strstr(m_cRemoteBuffer,"Set-Cookie:");
 
       if(m_cCookies)
       {
              CHAR* m_cCookie = (CHAR*)malloc(1024);
              memset(m_cCookie,0,1024);
              
              while(strstr(m_cCookies,"Set-Cookie:"))
              {
                     m_cCookies = strstr(m_cCookies,"Set-Cookie:");
                     m_cCookies += strlen("Set-Cookie:");
                     *m_cCookies++;
                     
                     strncat(m_cCookie,m_cCookies,strstr(m_cCookies,";")-m_cCookies);
 
                     if(strstr(m_cCookies,"Set-Cookie:"))
                            strcat(m_cCookie,"; ");
              }
 
              return m_cCookie;
       }
       else
       {
              return NULL;
       }
 
       return NULL;
}
 
INT CSocket::remoteGET(SOCKET m_sConnection, CHAR* m_cHost, CHAR* m_cPath, CHAR* m_cReferer, CHAR* m_cCookies)
{
       CHAR* m_cCompleteBuffer = (CHAR*)malloc(2096);
 
       sprintf(m_cCompleteBuffer,
              "GET %s " 
              "HTTP/1.1\r\n"
              "Host: %s\r\n"
              "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; fr; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3\r\n"
              "Keep-Alive: 300\r\n"
              "Connection: keep-alive\r\n"
              ,m_cPath
              ,m_cHost);

       if(m_cReferer)
       {
              strcat(m_cCompleteBuffer,"Referer: ");
              strcat(m_cCompleteBuffer,m_cReferer);
              if(m_cCookies)
                     strcat(m_cCompleteBuffer,"\r\n");
              else
                     strcat(m_cCompleteBuffer,"\r\n\r\n");
       }
 
       if(m_cCookies)
       {
              strcat(m_cCompleteBuffer,"Cookie: ");
              strcat(m_cCompleteBuffer,m_cCookies);
              strcat(m_cCompleteBuffer,"\r\n\r\n");
       }
 
       INT m_iReturn = send(m_sConnection,m_cCompleteBuffer,strlen(m_cCompleteBuffer),0);
       
       free(m_cCompleteBuffer);
 
       return m_iReturn;
}
 

INT CSocket::remotePOSTFile(SOCKET m_sConnection, CHAR* m_cHost, CHAR* m_cPath, remotepost_file* m_cData, CHAR* m_cReferer, CHAR* m_cCookies)
{
       CHAR* m_cCompleteBuffer = (CHAR*)malloc(2096);
 
       if(m_cCookies)
       {
              sprintf(m_cCompleteBuffer,
                     "POST %s "
                     "HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; fr; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3\r\n"
                     "Referer: %s\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\r\n"
                     "Cookie: %s\r\n"
                     ,m_cPath
                     ,m_cHost
                     ,m_cReferer
                     ,m_cCookies);
       }
       else
       {
              sprintf(m_cCompleteBuffer,
                     "POST %s "
                     "HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; fr; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3\r\n"
                     "Referer: %s\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\r\n"
                     ,m_cPath
                     ,m_cHost
                     ,m_cReferer);
       }
 
       CHAR* m_cDummy  = "Content-Type: application/x-www-form-urlencoded\r\n";
       CHAR* m_cDummy2 = (CHAR*)malloc(128);
 
	   INT m_iLength = 0;
	   remotepost_file *it = m_cData;
	   while(it){
		   m_iLength += strlen(it->postname) + it->size;
		   it = it->next;
	   }

       sprintf(m_cDummy2,"Content-Length: %i\r\n\r\n",m_iLength);

       INT m_iReturn = send(m_sConnection,m_cCompleteBuffer,strlen(m_cCompleteBuffer),0);
       
       if(m_iReturn)
       {
              m_iReturn = send(m_sConnection,m_cDummy,strlen(m_cDummy),0);
              
              if(m_iReturn)
              {
                     m_iReturn = send(m_sConnection,m_cDummy2,strlen(m_cDummy2),0);
                            
                            if(m_iReturn)
                            {
									while(m_cData){
									   FILE *fp = fopen(m_cData->filename, "rb");
									   if(fp){
											char *fp_buff = (char*)malloc(m_cData->size+strlen(m_cData->postname));
											strcpy(fp_buff, m_cData->postname);
											fread((fp_buff+strlen(m_cData->postname)), 1, m_cData->size, fp);

											m_iReturn = send(m_sConnection,fp_buff,m_cData->size,0);

											free(fp_buff);
									   }
										
									   m_cData = m_cData->next;
									}
									free(m_cDummy2);
									free(m_cCompleteBuffer);
									return m_iReturn;
                            }
              }
       }
 
       free(m_cDummy2);
       free(m_cCompleteBuffer);
 
       return m_iReturn;
}


INT CSocket::remotePOST(SOCKET m_sConnection, CHAR* m_cHost, CHAR* m_cPath, CHAR* m_cData, CHAR* m_cReferer, CHAR* m_cCookies)
{
       CHAR* m_cCompleteBuffer = (CHAR*)malloc(2096);
 
       if(m_cCookies)
       {
              sprintf(m_cCompleteBuffer,
                     "POST %s "
                     "HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; fr; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3\r\n"
                     "Referer: %s\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\r\n"
                     "Cookie: %s\r\n"
                     ,m_cPath
                     ,m_cHost
                     ,m_cReferer
                     ,m_cCookies);
       }
       else
       {
              sprintf(m_cCompleteBuffer,
                     "POST %s "
                     "HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; fr; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3\r\n"
                     "Referer: %s\r\n"
                     "Keep-Alive: 300\r\n"
                     "Connection: keep-alive\r\n"
                     ,m_cPath
                     ,m_cHost
                     ,m_cReferer);
       }
 
       CHAR* m_cDummy  = "Content-Type: application/x-www-form-urlencoded\r\n";
       CHAR* m_cDummy2 = (CHAR*)malloc(128);
 
       INT m_iLength = strlen(m_cData);
 
       sprintf(m_cDummy2,"Content-Length: %i\r\n\r\n",m_iLength);
 
       INT m_iReturn = send(m_sConnection,m_cCompleteBuffer,strlen(m_cCompleteBuffer),0);
       
       if(m_iReturn)
       {
              m_iReturn = send(m_sConnection,m_cDummy,strlen(m_cDummy),0);
              
              if(m_iReturn)
              {
                     m_iReturn = send(m_sConnection,m_cDummy2,strlen(m_cDummy2),0);
                            
                            if(m_iReturn)
                            {
                                   m_iReturn = send(m_sConnection,m_cData,strlen(m_cData),0);
 
                                   if(m_iReturn)
                                   {
                                          free(m_cDummy2);
                                          free(m_cCompleteBuffer);
                                          return m_iReturn;
                                   }
                            }
              }
       }
 
       free(m_cDummy2);
       free(m_cCompleteBuffer);
 
       return m_iReturn;
}

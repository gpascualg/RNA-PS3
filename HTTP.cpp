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
       netInitialize();
       m_cRecievePacket = (char*)malloc(4096);
}
 
CSocket::~CSocket()
{
       netDeinitialize();
       free(m_cRecievePacket);
}
 
int CSocket::disconnectSocket(int m_sConnection)
{
    int ret = shutdown(m_sConnection, SHUT_RDWR);
	if (ret < 0) 
		return -1;
	
	ret = close(m_sConnection);
	if (ret < 0) 
		return -1;

	return 0;
}

int CSocket::disconnect(){
	int ret = shutdown(list_s, SHUT_RDWR);
	if (ret < 0) 
		return -1;
	
	ret = close(list_s);
	if (ret < 0) 
		return -1;

    m_bServerOnline = false;

	return 1;
}
//-------------------------------------------------------------------
// Server functions
//-------------------------------------------------------------------
 
bool CSocket::initializeServer(int m_iPort)
{
       list_s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
 
       m_siServerInfo.sin_family = AF_INET;
       m_siServerInfo.sin_addr.s_addr = INADDR_ANY; 
       m_siServerInfo.sin_port = htons(m_iPort);

	   if(bind(list_s, (sockaddr*)&m_siServerInfo, sizeof(struct sockaddr)) == 0)
       {
              m_bServerOnline = true;
              return true;
       }
 
       return false;
}
 
int CSocket::acceptClient()
{
       if(m_bServerOnline)
       {
              if(listen( list_s, 256 ) != 0)
                     return NULL;
 
              int m_sClient;
              int m_iLength;
 
              m_iLength = sizeof m_siServerInfo;
 
              m_sClient = accept( list_s, (sockaddr*)&m_siServerInfo, (socklen_t*)&m_iLength );
 
              if(m_sClient != 0)
                     return m_sClient;
 
              return NULL;
       }
       else
       {
              return NULL;
       }
}
 
char* CSocket::clientRecieve(int m_sClient, int *size)
{
       if(m_bServerOnline)
       {
              memset(m_cRecievePacket,0,4096);
 
              int m_iReturn = recv(m_sClient,m_cRecievePacket,4096,0);
              
              if(m_iReturn > 0){
					 if(size)
						 *size = m_iReturn;

                     return m_cRecievePacket;
			  }

              if(m_iReturn != 0)
                     return NULL;

              return NULL;
       }
       else
       {
              return NULL;
       }
}
 
int CSocket::clientSend(int m_sClient, char* m_cPacket)
{
       return send(m_sClient,m_cPacket,strlen(m_cPacket),0);
}
int CSocket::clientSend(int m_sClient, u8* m_cPacket, int len){
	return send(m_sClient,(char*)m_cPacket,len,0);
}
 
//-------------------------------------------------------------------
// Remote connections functions
//-------------------------------------------------------------------
 
int CSocket::remoteConnect(char* m_IP, int m_iPort)
{
    int m_sConnection = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
     
	memset(&m_siServerInfo, 0, sizeof(sockaddr_in));
	m_siServerInfo.sin_len = sizeof(sockaddr_in);
	m_siServerInfo.sin_family = AF_INET;
	inet_pton(AF_INET, m_IP, &m_siServerInfo.sin_addr);
	m_siServerInfo.sin_port = htons(m_iPort);

	int ret = connect(m_sConnection, (struct sockaddr*)&m_siServerInfo, sizeof(sockaddr_in));
	if (ret)
		return -1;

	return m_sConnection;
}

int CSocket::remoteRecieve(int m_sConnection, char* m_cRemoteBuffer, int m_iLength)
{
       memset(m_cRemoteBuffer,0,m_iLength);
       int m_iReturn = recv(m_sConnection,m_cRemoteBuffer,m_iLength,0);
 
       return m_iReturn;
}
 
char* CSocket::remoteRetrieveCookies(char* m_cRemoteBuffer)
{
       char* m_cCookies = strstr(m_cRemoteBuffer,"Set-Cookie:");
 
       if(m_cCookies)
       {
              char* m_cCookie = (char*)malloc(1024);
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
 
int CSocket::remoteGET(int m_sConnection, char* m_cHost, char* m_cPath, char* m_cReferer, char* m_cCookies)
{
       char* m_cCompleteBuffer = (char*)malloc(2096);
 
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
 
       int m_iReturn = send(m_sConnection,m_cCompleteBuffer,strlen(m_cCompleteBuffer),0);
       
       free(m_cCompleteBuffer);
 
       return m_iReturn;
}
 
int CSocket::remotePOST(int m_sConnection, char* m_cHost, char* m_cPath, char* m_cData, char* m_cReferer, char* m_cCookies)
{
       char* m_cCompleteBuffer = (char*)malloc(2096);
 
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
 
       char* m_cDummy  = "Content-Type: application/x-www-form-urlencoded\r\n";
       char* m_cDummy2 = (char*)malloc(128);
 
       int m_iLength = strlen(m_cData);
 
       sprintf(m_cDummy2,"Content-Length: %i\r\n\r\n",m_iLength);
 
       int m_iReturn = send(m_sConnection,m_cCompleteBuffer,strlen(m_cCompleteBuffer),0);
       
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

//Deletes HTTP headers
char* CSocket::clearBuffer(char* buffer){
	bool end = false;
	bool old = false;
	while(!end){
		if(*buffer == '\0')
			break;
		else if(*buffer == '\n'){
			if(!old)
				old = true;
			else
				end = true;
		}else if(*buffer == '\r'){}
		else
			old = false;	
		
		buffer++;
	}

	return buffer;
}
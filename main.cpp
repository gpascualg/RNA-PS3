#include <stdio.h>
#include <stdlib.h>

#include "HTTP.h"
#include "packet.h"
#include "blowfish.h"
#include "tropusr_parser.h"
#include "dirent.h"

CSocket *csocket = NULL;
SOCKET sock = NULL;

int main(){
	csocket = new CSocket();
	
	//Let's rock =)
	sock = csocket->remoteConnect("localhost", 80);

	unsigned char L1 = 'b';
	unsigned char L2 = 'l';
	unsigned char L3 = 'i';
	unsigned char L4 = 'p';
	unsigned char L5 = 'i';	

	char path[1024] = {0};
	char packet[1024] = {0};
								//5 = ulen
	sprintf(packet, "01111600000005%.2X%.2X%.2X%.2X%.2X46f94c8de14fb36680850768ff1b7f2a", L1, L2, L3, L4, L5);

	BYTE crc32 = set_crc_byte(packet);
	
	printf("[C->S] %s\n", packet);

	sprintf(path, "/RNA/parse_packet.php?packet=%s", packet);

	csocket->remoteGET(sock, "localhost", path, "PKG", NULL);

	char *buffer = new char[1024];
	memset(buffer, 0, 1024);

	csocket->remoteRecieve(sock, buffer, 1024);
	bool end = false;
	bool old = false;
	while(!end){
		if(*buffer == '\n'){
			if(!old)
				old = true;
			else
				end = true;
		}else if(*buffer == '\r'){}
		else
			old = false;	
		
		buffer++;
	}


	printf("[S->C] %s\n", buffer);

	processPacket(buffer);

	syncTrophies();
	
	listFriends();

	printf("\n\n");
	system("pause");
}
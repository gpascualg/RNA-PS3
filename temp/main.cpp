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

	printf("\n -------------------- DISCLAIMER ---------------------\n");
	printf("|| El usuario al usar este programa, que a partir de ||\n");
	printf("|| ahora sera referido como RNA, se compromete a no  ||\n");
	printf("||    darle un uso ilegal, ya sea para compartir     ||\n");
	printf("||  informacion con derechos de propiedad o por ende ||\n");
	printf("||   juegos cuya funcion LAN implique el uso de un   ||\n");
	printf("||     servidor externo o metodo ajeno a su (del     ||\n");
	printf("||                  usuario) propiedad.              ||\n");
	printf("||                                                   ||\n");
	printf("||     Pulsar cualquier tecla y continuar con el     ||\n");
	printf("|| programa implica la aceptacion immediata de tales ||\n");
	printf("||   condiciones y la asumision total de los actos   ||\n");
	printf("||        realizados, eximiendo de cualquier         ||\n");
	printf("||  responsabilidad al creador de esta herramienta   ||\n");
	printf("||  de comunicacion entre plataformas PlayStation 3  ||\n");
	printf("||                     de Sony (c)                   ||\n");
	printf(" -----------------------------------------------------\n\n");

	system("pause");
	printf("\n");

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

	printf("\nAdding Friend \"test\"\n");
	addFriend();

	printf("\n\n");
	system("pause");
}
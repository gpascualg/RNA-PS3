#include "packet.h"

extern CSocket *csocket;
extern SOCKET sock;

int count = 0;
u8 session_id[16] = {0};
DWORD user_id = 0x0;
u8 security = 0x0;

char *recv_buffer = NULL;

//Blowfish
BYTE blowfish_key[8] = {0};
u8 byte0 = 0;
u8 byte1 = 0;
u8 byte2 = 0;
u8 byte3 = 0;
u8 byte4 = 0;
u8 byte5 = 0;
u8 byte6 = 0;
u8 byte7 = 0;


u32 ReadDWord(char *packet);
u16 ReadWord(char *packet);
u8 ReadByte(char *packet);
void ReadArray(char *packet, u8* out, int len);

void process_6000(char *packet);
void build_6000_answer();

u8 chartohex(char ch){
	if(ch >= '0' && ch <= '9')
		return (u8)(ch - '0');

	if(ch >= 'a' && ch <= 'f')
		return (u8)(ch - 'a') + 0xA;
	
	if(ch >= 'A' && ch <= 'F')
		return (u8)(ch - 'A') + 0xA;

	return -1;
}

char hextochar(BYTE ch){
	char buff[2];
	sprintf(buff, "%.1X", ch);

	return buff[0];
}

int set_crc_byte(char *packet){
	int len = strlen(packet);

	char *crc_packet = new char[len];
	memcpy(crc_packet, packet, len);

	crc_packet[10] = '0';
	crc_packet[11] = '0';

	for(int i = 0; i < len; i++)
		crc_packet[i] = toupper(crc_packet[i]);
	
	u32 crc = crc32buf(crc_packet, len);
	char crc_str[256] = {0};
	sprintf(crc_str, "%X", crc);

	char *crc_md5 = MDString(crc_str);
	
	BYTE crc32 = 0;
	for(int i = 0; i < 16*2; i+=2)
		crc32 ^= ((chartohex(crc_md5[i]) << 4) + chartohex(crc_md5[i+1]) );	
	
	packet[10] = hextochar(crc32>>4);
	packet[11] = hextochar(crc32 & 0xF);

	return 1;
}

int set_security_byte(char *packet){
	u32 result = (0x3F*(~security+0x34));
	result = result ^ ( result >> 4 );
	result = result & 0xFF;
	security = result;

	packet[8] = hextochar(security>>4);
	packet[9] = hextochar(security & 0xF);

	return 1;
}

void processPacket(char *packet){
	count = 0;

	if(!recv_buffer)
		recv_buffer = new char[1024];
	
	u16 opcode = ReadWord(packet);

	switch(opcode){
		case 0x6000:
			process_6000(packet);
		break;
	}
}

void process_6000(char *packet){
	u16 size = ReadWord(packet);
	u8 flag = ReadByte(packet);

	switch(flag){
		case 0x50:{
			security = ReadByte(packet);
			ReadByte(packet); //0x5 -> Login succes
			ReadByte(packet); //0x1 -> User permisses
			user_id = ReadDWord(packet); //save uid
			ReadArray(packet, session_id, 16); //save sid

			build_6000_answer();
		}break;
		case 0x55:{
			byte0 = ReadByte(packet);
			byte1 = ReadByte(packet);
			byte2 = ReadByte(packet);
			byte3 = ReadByte(packet);
			
			blowfish_key[0] = byte0;
			blowfish_key[1] = byte1;
			blowfish_key[2] = byte2;
			blowfish_key[3] = byte3;
			blowfish_key[4] = byte7;
			blowfish_key[5] = byte6;
			blowfish_key[6] = byte5;
			blowfish_key[7] = byte4;
			
			u8 magic[9] = {0};		
			for(int i = 0; i < 8; i++)
				magic[i] = ReadByte(packet);
			
			cBlowFish *blowfish = new cBlowFish();
			blowfish->Initialize(blowfish_key, 8);
			blowfish->Decode(magic, magic, 8);

			if(strcmp("00133700", (char*)magic) == 0){
				printf("\nSUCCESSFULLY LOGGED IN!\n\n");
				
				printf("Requesting user info...\n");
				//LOGUED IN AND HANDSHAKE = 1
			}
		}break;
	}
}

void add_session_id(char *packet){
	for(int i = 0; i < 16; i++){
		char buff[3];
		sprintf(buff, "%.2X", session_id[i]);
		strcat(packet, buff);
	}
}

void build_6000_answer(){
	char packet[256] = {0};
	char path[256*2] = {0};
	sprintf(packet, "006020000000");
	add_session_id(packet);

	srand((unsigned)time(0)); 
    byte4 = rand() & 0xFF; 
	byte5 = rand() & 0xFF; 
	byte6 = rand() & 0xFF; 
	byte7 = rand() & 0xFF;

	char buff[9];
	sprintf(buff, "%.2X%.2X%.2X%.2X", byte4, byte5, byte6, byte7);
	strcat(packet, buff);

	set_security_byte(packet);
	set_crc_byte(packet);

	sprintf(path, "/RNA/parse_packet.php?packet=%s", packet);
	csocket->remoteGET(sock, "localhost", path, "PKG", NULL);

	printf("[C->S] %s\n", packet);

	csocket->remoteRecieve(sock, recv_buffer, 1024);
	bool end = false;
	bool old = false;
	while(!end){
		if(*recv_buffer == '\n'){
			if(!old)
				old = true;
			else
				end = true;
		}else if(*recv_buffer == '\r'){}
		else
			old = false;	
		
		recv_buffer++;
	}
	
	printf("[S->C] %s\n", recv_buffer);
	processPacket(recv_buffer);
}

u8 ReadByte(char *packet){
	u8 ret = (chartohex(packet[count]) << 4) + chartohex(packet[count + 1]);

	count += 1*2;
	return ret;
}

u16 ReadWord(char *packet){
	u16 ret = ((chartohex(packet[count]) << 4) + chartohex(packet[count + 1])) + (((chartohex(packet[count + 2]) << 4) + chartohex(packet[count + 3])) << 8);

	count += 2*2;
	return ret;
}

u32 ReadDWord(char *packet){
	u32 ret = ((chartohex(packet[count]) << 4) + chartohex(packet[count + 1])) + (((chartohex(packet[count + 2]) << 4) + chartohex(packet[count + 3])) << 8) + (((chartohex(packet[count + 4]) << 4) + chartohex(packet[count + 5])) << 16) + (((chartohex(packet[count + 6]) << 4) + chartohex(packet[count + 7])) << 24);

	count += 4*2;
	return ret;
}

void ReadArray(char *packet, u8* out, int len){
	for(int i = 0; i < len; i++)
		out[i] = ReadByte(packet);
}
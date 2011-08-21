#include <stdio.h>
#include <stdlib.h>

#include "HTTP.h"
#include "crc32.h"
#include "md5.h"
#include "blowfish.h"
#include "dirent.h"
#include "tropusr_parser.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

BYTE chartohex(char ch);
char hextochar(BYTE ch);

int set_crc_byte(char *packet);

void processPacket(char *packet);

void syncTrophies();
void listFriends();
void addFriend();
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>

#include "HTTP.h"
#include "crc32.h"
#include "md5.h"
#include "blowfish.h"
#include "tropusr_parser.h"


u8 chartohex(char ch);
char hextochar(u8 ch);

int set_crc_byte(char *packet);

void processPacket(char *packet);

char* send_login(char *username, char *password);

void syncTrophies();
void listFriends();
void addFriend();
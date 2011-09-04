#ifndef _TROPUSR_PARSER_H
#define _TROPUSR_PARSER_H

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#define cplusplus

#pragma pack(1)

typedef struct tropusr_trophies{	
	DWORD unk1;		//06 00 00 00
	DWORD unk2;
	BYTE unk3[0x8];
	BYTE unk4[3];

	BYTE trophie_id;

	BYTE unk5[3];

	BYTE unlocked;
	BYTE unk00;
	BYTE platinum_unlocked;
	BYTE unk10;	//0x10 / 0x20
	BYTE unk6[5];
	BYTE timestamp[16];

	BYTE unk7[0x40];
}				
#ifdef cplusplus
;
#else
tropusr_trophies;
#endif

typedef struct tropusr_trophies_list{	
	struct tropusr_trophies *trophie;
	struct tropusr_trophies_list *next;
}				
#ifdef cplusplus
;
#else
tropusr_trophies_list;
#endif

typedef struct tropusr_ret{	
	char game_name[16];
	BYTE max_trophies;
	BYTE user_trophies;

	struct tropusr_trophies_list *trophies_list;
}				
#ifdef cplusplus
;
#else
tropusr_ret;
#endif

struct tropusr_ret *parse_tropusr( char *path );

#endif
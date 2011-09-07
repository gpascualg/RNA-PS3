#ifndef _TROPUSR_PARSER_H
#define _TROPUSR_PARSER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <psl1ght/types.h>

#define cplusplus

#pragma pack(1)

typedef struct tropusr_trophies{	
	u32 unk1;		//06 00 00 00
	u32 unk2;
	u8 unk3[0x8];
	u8 unk4[3];

	u8 trophie_id;

	u8 unk5[3];

	u8 unlocked;
	u8 unk00;
	u8 platinum_unlocked;
	u8 unk10;	//0x10 / 0x20
	u8 unk6[5];
	u8 timestamp[16];

	u8 unk7[0x40];
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
	u8 max_trophies;
	u8 user_trophies;

	struct tropusr_trophies_list *trophies_list;
}				
#ifdef cplusplus
;
#else
tropusr_ret;
#endif

struct tropusr_ret *parse_tropusr( char *path );

#endif
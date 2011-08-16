#include "tropusr_parser.h"

#define cplusplus

#pragma pack(1)

typedef struct tropusr_header{			//0x00
	DWORD sig;			//81 8F 54 AD
	BYTE unk1;			//0
	BYTE unk2;			//1
	BYTE unk3[5];		//00 00 00 00 00
	BYTE num_params;	//0A
	BYTE unk5[0x24];	//00 -> 39
}
#ifdef cplusplus
;
#else
tropusr_header;
#endif

typedef struct tropusr_params{			//0x30	
	DWORD idend;
	DWORD unk2;
	DWORD unk3;
	DWORD value;			
	DWORD unk4;
	DWORD unk5;
	BYTE unk6[8];
}										//[LOOP UNTIL (idend >> 24) & 0xFF == tropusr_header::num_params]
										//(idend >> 24) & 0xFF == 0x4 -> GAME_NUM_TROPHIES
#ifdef cplusplus
;
#else
tropusr_header;
#endif

typedef struct tropusr_params2{			//0x180
	DWORD unk1;
	DWORD unk2;
	BYTE unk3[8];
	BYTE unk4;			//00
	BYTE unk5;			//01
	BYTE unk6;			//00
	BYTE unk7;			//01
	BYTE unk8[0x2F];	//00
	DWORD unk9;			//02 00 00 00
	DWORD unk10;			//40 00 00 00
	BYTE unk11[0x48];
	DWORD unk12;			//08 00 00 00
	BYTE unk13;			//40
	BYTE unk14[0x8];	//00

	BYTE params_unk[0x40];
	
	DWORD unk15;		//00 00 00 03
	WORD unk16;			//00 00
	WORD unk17;			//01 10

	BYTE unk18[8];

	BYTE game_name[16];

	BYTE unk19;			//00
	WORD unk20;			//01 00
	WORD unk21;			//01 00 / 00 00
	WORD unk22;			//01 00
	DWORD unk23;		//01 00 00 00 // 00 00 00 00
	DWORD unk24;		//
	BYTE unk25;
}
#ifdef cplusplus
;
#else
tropusr_params2;
#endif

typedef struct tropusr_unknownff{			//0x30	
	DWORD unk1;		//FF FF FF FF
	WORD unk2;			//00 00
	WORD unk3;		//1F FF / FF FF

	char unk4[0x18];
	DWORD unk5;
	DWORD unk6;
	char unk7[0x1B];
	DWORD unk8;
	DWORD unk9;
	DWORD unk10;
	DWORD unk11;
	DWORD unk12;
	char unk13[0x1A];
	WORD unk14;
	char unk15[0x1D];
	DWORD unk16;
	DWORD unk17;
	char unk18[0xF];

	BYTE end1;			//01
	DWORD end2;			//FF FF FF FF
	
	DWORD unk19;
}
#ifdef cplusplus
;
#else
tropusr_unknownff;
#endif

typedef struct tropusr_subunknownff{			//0x30	
	DWORD unk1;		//FF FF FF FF
	WORD unk2;			//00 00
	WORD unk3;		//1F FF / FF FF

	char unk4[0x3B];
	DWORD unk5;
	DWORD unk6;
	
	char unk7[0xC];

	BYTE end1;			//01
	DWORD end2;			//FF FF FF FF
	
	DWORD unk19;
}			
#ifdef cplusplus
;
#else
tropusr_subunknownff;
#endif

//DWORD
//WORD
//WORD
//char [0x3B]

typedef struct tropusr_routineunknown{	
	DWORD unk1;		//04 00 00 00 -> 05 00 00 00 = END
	DWORD unk2;		//50 00 00 00 -> ?
	DWORD unk3;		//ID
	DWORD unk4;		//00 00 00 00
	DWORD unk5;		//ID
	DWORD unk6;		//04 00 00 00
	char unk7[0x48];
}				
#ifdef cplusplus
;
#else
tropusr_routineunknown;
#endif

typedef struct tropusr_routineunknown05{	
	DWORD unk1;		//04 00 00 00 -> 05 00 00 00 = END
	DWORD unk2;		//50 00 00 00 -> ?
	char unk3[0x1E];
	char unk4[16];
	char unk5[32];
	char unk6[0x2A];
	BYTE user_trophies;
	char unk7[0xF];
	DWORD unk8;
	DWORD unk9;
	char unk10[0x15];
	char unk11[0x30];

	DWORD unk12; //00 00 00 07 
	DWORD unk13; //00 00 00 50
	char unk14[8];

	DWORD unk15;
	char unk16[0x4C];
}				
#ifdef cplusplus
;
#else
tropusr_routineunknown;
#endif

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



int test(){
	FILE *fp = fopen("I:\\trophy\\NPWR00499_00\\TROPUSR.DAT", "rb");
	BYTE GAME_NUM_TROPHIES = 0x0;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);

	char *data = new char[size];
	char *data_org = data;
	fread(data, 1, size, fp);

	tropusr_header *header = (tropusr_header*)(data);
	data += sizeof(tropusr_header);

	//[LOOP UNTIL (idend >> 24) & 0xFF == tropusr_header::num_params]
	//(idend >> 24) & 0xFF == 0x4 -> GAME_NUM_TROPHIES
	while(1){
		tropusr_params *param = (tropusr_params*)(data);
		data += sizeof(tropusr_params);
		
		if(((param->idend >> 24) & 0xFF) == 0x4)
			GAME_NUM_TROPHIES = ((param->value >> 24) & 0xFF);
		else if(((param->idend >> 24) & 0xFF) == header->num_params)
			break;
	}
	
	tropusr_params2 *params2 = (tropusr_params2*)(data);
	data += sizeof(tropusr_params2);

	
	tropusr_unknownff *routineff = (tropusr_unknownff*)(data);
	data += sizeof(tropusr_unknownff);

	while(routineff->end1 != 0x1){
		tropusr_subunknownff *routinesubff = (tropusr_subunknownff*)(data);
		data += sizeof(tropusr_subunknownff);

		if(routinesubff->end1 == 0x1)
			break;
	}
	
	data += sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + 0x3B;
	
	while(1){
		tropusr_routineunknown *routineunk = (tropusr_routineunknown*)(data);
		if((routineunk->unk1 & 0xFF) == 0x4)
			data += sizeof(tropusr_routineunknown);
		else
			break;
	}

	tropusr_routineunknown05 *routineunk05 = (tropusr_routineunknown05*)(data); 
	data += sizeof(tropusr_routineunknown05);
	
	printf("NAME:\t\t%s\n", params2->game_name);
	printf("MAX TROPHIES:\t%d\n", GAME_NUM_TROPHIES);
	printf("USER TROPHIES:\t%d\n\n", routineunk05->user_trophies);

	while(data < data_org + size){
		tropusr_trophies *trophie = (tropusr_trophies*)(data);

		printf("TROPHIE\n");
		printf("\tID:\t%X\n", trophie->trophie_id);
		printf("\tUNLOCKED:\t%X\n", trophie->unlocked);
		printf("\tP_UNLOCKED:\t%X\n\n", trophie->platinum_unlocked);

		data += sizeof(tropusr_trophies);
	}

	printf("\n\n");

	return 1;
}
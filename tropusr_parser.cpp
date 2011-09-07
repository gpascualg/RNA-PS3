#include "tropusr_parser.h"

#pragma pack(1)

typedef struct tropusr_header{			//0x00
	u32 sig;			//81 8F 54 AD
	u8 unk1;			//0
	u8 unk2;			//1
	u8 unk3[5];		//00 00 00 00 00
	u8 num_params;	//0A
	u8 unk5[0x24];	//00 -> 39
}
#ifdef cplusplus
;
#else
tropusr_header;
#endif

typedef struct tropusr_params{			//0x30	
	u32 idend;
	u32 unk2;
	u32 unk3;
	u32 value;			
	u32 unk4;
	u32 unk5;
	u8 unk6[8];
}										//[LOOP UNTIL (idend >> 24) & 0xFF == tropusr_header::num_params]
										//(idend >> 24) & 0xFF == 0x4 -> GAME_NUM_TROPHIES
#ifdef cplusplus
;
#else
tropusr_header;
#endif

typedef struct tropusr_params2{			//0x180
	u32 unk1;
	u32 unk2;
	u8 unk3[8];
	u8 unk4;			//00
	u8 unk5;			//01
	u8 unk6;			//00
	u8 unk7;			//01
	u8 unk8[0x2F];	//00
	u32 unk9;			//02 00 00 00
	u32 unk10;			//40 00 00 00
	u8 unk11[0x48];
	u32 unk12;			//08 00 00 00
	u8 unk13;			//40
	u8 unk14[0x8];	//00

	u8 params_unk[0x40];
	
	u32 unk15;		//00 00 00 03
	u16 unk16;			//00 00
	u16 unk17;			//01 10

	u8 unk18[8];

	u8 game_name[16];

	u8 unk19;			//00
	u16 unk20;			//01 00
	u16 unk21;			//01 00 / 00 00
	u16 unk22;			//01 00
	u32 unk23;		//01 00 00 00 // 00 00 00 00
	u32 unk24;		//
	u8 unk25;
}
#ifdef cplusplus
;
#else
tropusr_params2;
#endif

typedef struct tropusr_unknownff{			//0x30	
	u32 unk1;		//FF FF FF FF
	u16 unk2;			//00 00
	u16 unk3;		//1F FF / FF FF

	char unk4[0x18];
	u32 unk5;
	u32 unk6;
	char unk7[0x1B];
	u32 unk8;
	u32 unk9;
	u32 unk10;
	u32 unk11;
	u32 unk12;
	char unk13[0x1A];
	u16 unk14;
	char unk15[0x1D];
	u32 unk16;
	u32 unk17;
	char unk18[0xF];

	u8 end1;			//01
	u32 end2;			//FF FF FF FF
	
	u32 unk19;
}
#ifdef cplusplus
;
#else
tropusr_unknownff;
#endif

typedef struct tropusr_subunknownff{			//0x30	
	u32 unk1;		//FF FF FF FF
	u16 unk2;			//00 00
	u16 unk3;		//1F FF / FF FF

	char unk4[0x3B];
	u32 unk5;
	u32 unk6;
	
	char unk7[0xC];

	u8 end1;			//01
	u32 end2;			//FF FF FF FF
	
	u32 unk19;
}			
#ifdef cplusplus
;
#else
tropusr_subunknownff;
#endif

//u32
//u16
//u16
//char [0x3B]

typedef struct tropusr_routineunknown{	
	u32 unk1;		//04 00 00 00 -> 05 00 00 00 = END
	u32 unk2;		//50 00 00 00 -> ?
	u32 unk3;		//ID
	u32 unk4;		//00 00 00 00
	u32 unk5;		//ID
	u32 unk6;		//04 00 00 00
	char unk7[0x48];
}				
#ifdef cplusplus
;
#else
tropusr_routineunknown;
#endif

typedef struct tropusr_routineunknown05{	
	u32 unk1;		//04 00 00 00 -> 05 00 00 00 = END
	u32 unk2;		//50 00 00 00 -> ?
	char unk3[0x1E];
	char unk4[16];
	char unk5[32];
	char unk6[0x2A];
	u8 user_trophies;
	char unk7[0xF];
	u32 unk8;
	u32 unk9;
	char unk10[0x15];
	char unk11[0x30];

	u32 unk12; //00 00 00 07 
	u32 unk13; //00 00 00 50
	char unk14[8];

	u32 unk15;
	char unk16[0x4C];
}				
#ifdef cplusplus
;
#else
tropusr_routineunknown;
#endif



struct tropusr_ret *parse_tropusr(char *path){
	FILE *fp = fopen(path, "rb");
	
	tropusr_ret *ret = new tropusr_ret;

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
			ret->max_trophies = ((param->value >> 24) & 0xFF);
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
	
	data += sizeof(u32) + sizeof(u16) + sizeof(u16) + 0x3B;
	
	while(1){
		tropusr_routineunknown *routineunk = (tropusr_routineunknown*)(data);
		if((routineunk->unk1 & 0xFF) == 0x4)
			data += sizeof(tropusr_routineunknown);
		else
			break;
	}

	tropusr_routineunknown05 *routineunk05 = (tropusr_routineunknown05*)(data); 
	data += sizeof(tropusr_routineunknown05);
	
	strcpy(ret->game_name, (char*)params2->game_name);
	ret->user_trophies = routineunk05->user_trophies;

	tropusr_trophies_list *f = NULL, *l = NULL;

	while(data < data_org + size){
		tropusr_trophies *trophie = (tropusr_trophies*)(data);
		if(l == NULL)
			l = f = new tropusr_trophies_list;
		else{
			l->next = new tropusr_trophies_list;
			l = l->next;
		}
		memset(l, 0, sizeof(tropusr_trophies_list));
		l->trophie = trophie;

		data += sizeof(tropusr_trophies);
	}
	
	ret->trophies_list = f;

	return ret;
}
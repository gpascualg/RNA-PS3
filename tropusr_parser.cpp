#include <Windows.h>

#define cplusplus

typedef struct tropusr_header{			//0x00
	DWORD sig;			//81 8F 54 AD
	BYTE unk1;			//0
	BYTE unk2;			//1
	char unk3[5];		//00 00 00 00 00
	BYTE num_params;	//0A
	char unk5[0x2B];	//00 -> 39
	DWORD unk6;
	DWORD unk7;
	DWORD unk8;
	BYTE unk9;
}
#ifdef cplusplus
;
#else
tropusr_header;
#endif

typedef struct tropusr_params{			//0x30	
	char unk1[6];
	BYTE param_1;
	BYTE param_2;
	char unk2[0xB];
	DWORD param_3;
	DWORD param_4;
	DWORD param_5;
	BYTE value;
};					//[0xB]
#ifdef cplusplus
;
#else
tropusr_header;
#endif

typedef struct tropusr_params2{			//0x180
	BYTE unk1;			//00
	BYTE unk2;			//01
	BYTE unk3;			//00
	BYTE unk4;			//01
	char unk5[0x2F];	//00
	DWORD unk6;			//02 00 00 00
	DWORD unk7;			//40 00 00 00
	char unk8[0x48];
	DWORD unk9;			//08 00 00 00
	BYTE unk10;			//40
	char unk11[0x8];	//00

	char params_unk[0x40];
	
	DWORD unk12;		//00 00 00 03
	WORD unk13;			//00 00
	WORD unk14;			//01 10

	char unk15[8];

	char game_name[16];

	BYTE unk16;			//00
	WORD unk17;			//01 00
	WORD unk18;			//01 00 / 00 00
	WORD unk19;			//01 00
	DWORD unk20;		//01 00 00 00 // 00 00 00 00
	DWORD unk21;		//
	BYTE unk22;
};
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
};	
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
	
	char unk7[0x15];
	
};			
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
	DWORD unk2;
	DWORD unk3;
	DWORD unk4;
	DWORD unk5;
	DWORD unk6;
	char unk7[0x48];
};				
#ifdef cplusplus
;
#else
tropusr_routineunknown;
#endif

//char [0xE]
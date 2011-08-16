#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct PARAM_SFO_HEADER {
	 unsigned long Signature_bytes;
	 unsigned long File_Version;
	 unsigned long Start_of_Variable_Name_Table;
	 unsigned long Start_of_Variable_Data_Table;
	 unsigned long Number_of_Variables;
}PARAM_SFO_HEADER;

typedef struct INTERNAL_SFO_DATA {
	unsigned short Zero_based_offset_from_Start_of_Variable_Name_Table;
	unsigned short Unknow_YET;
	unsigned long Total_Used_Bytes_of_Current_Data_Block;
	unsigned long Total_Size_In_Bytes_of_Current_Data_Block;
	unsigned long Zero_based_offset_from_Start_of_Variable_Data_Table;
 }INTERNAL_SFO_DATA;

char *Variable_Name_table[0xB+1]={
	 "ACCOUNT_ID\0x00",
	 "ATTRIBUTE\0x00",
	 "CATEGORY\0x00",
	 "DETAIL0\x00",
	  "PARAMS\0x00",
	 "PARAMS2\0x00",
	 "PARENTAL_LEVEL\0x00",
	 "SAVEDATA_DIRECTORY\0x00",
	 "SAVEDATA_LIST_PARAM\0x00",
	 "SUB_TITLE\0x00",
	 "TITLE\0x00",
	 "\0x00\0x00"
 };

typedef struct INTERNAL_SFO_VARIABLES {
	unsigned char Account_ID[16];
	unsigned long Attribute;
	unsigned char Category[4];
	unsigned char Detail[1024]; 
	unsigned char Params2[12]; 
	unsigned long Parental_level; 
	unsigned char SaveData_directory[64]; 
	unsigned char Sub_Title[128];
	unsigned char Title[128];
 }INTERNAL_SFO_VARIABLES;

#ifdef _TEST_
int main(){

	unsigned char b = 0xf3;
	unsigned char result = (0x3F*(~b+0x34));
	result = result ^ ( result >> 4 );
	b = result;

	printf("%x\n", result);

	result = (0x3F*(~b+0x34));
	result = result ^ ( result >> 4 );
	b = result;
	printf("%x\n", result);
	
	result = (0x3F*(~b+0x34));
	result = result ^ ( result >> 4 );
	b = result;
	printf("%x\n", result);
	
	result = (0x3F*(~b+0x34));
	result = result ^ ( result >> 4 );
	b = result;

	printf("%x\n\n", result);

	system("pause");

	return 0;

	FILE *fp = fopen("E:\\Documents and Settings\\Administrador\\Mis documentos\\Mis archivos recibidos\\Psn\\NPEB00283-SAVE-\\PARAM.SFO", "rb");

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);

	unsigned char *buffer = (unsigned char*)malloc(size);
	fread(buffer, 1, size, fp);

	fclose(fp);

	int start = (int)buffer;

	PARAM_SFO_HEADER *header = (PARAM_SFO_HEADER*)(buffer);
	buffer += sizeof(PARAM_SFO_HEADER);

	printf("INFO (%d): %X\n", size, 
		header->Number_of_Variables);
	
	printf("POS: %X\n", (int)buffer-start);
	printf("S: %X\n", sizeof(INTERNAL_SFO_DATA));

	system("pause");

	for(unsigned short i = 0; i < header->Number_of_Variables; i++){
		INTERNAL_SFO_DATA *internal_data = (INTERNAL_SFO_DATA*)(buffer);
		buffer += sizeof(INTERNAL_SFO_DATA);
	}

	printf("POS (%X): %X\n", header->Number_of_Variables, (int)buffer-start);

	//Skip Variable_Name_table
	int n = 0;
	for(unsigned short i = 0; i < header->Number_of_Variables;){
		if(buffer[n++] == '\0')
			i++;
	}
	buffer += n;
	buffer += 2;

	printf("POS: %X\n", (int)buffer-start);

	system("pause");

	
	INTERNAL_SFO_VARIABLES *variables = (INTERNAL_SFO_VARIABLES*)(buffer);

	printf("AID: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n\n", variables->Account_ID[0], variables->Account_ID[1], variables->Account_ID[2], variables->Account_ID[3], variables->Account_ID[4], variables->Account_ID[5], variables->Account_ID[6], variables->Account_ID[7], variables->Account_ID[8], variables->Account_ID[9], variables->Account_ID[10], variables->Account_ID[11], variables->Account_ID[12], variables->Account_ID[13], variables->Account_ID[14], variables->Account_ID[15]);

	system("pause");

	return 0;
}
#endif
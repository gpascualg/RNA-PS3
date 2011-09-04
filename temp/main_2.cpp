// ----- LOCAL INCLUDES ----- //
#include "headers.h"
#include "font.h"
#include "graphics.h"

#include <psl1ght/lv2.h>
#include <psl1ght/lv2/thread.h>
#include <sys\time.h>
#include <limits.h>

#include <dirent.h>

#include <psl1ght\lv2\memory.h>
#include <io\osk.h>

#include <sysutil/events.h>
#include <io/msg.h>


// ----- DEFINES ----- //
#define	N_IMAGES		9	
#define ABORT_TIME	5
#define ENTERED_TIMES 3

// ----- STRUCTS ----- //
typedef struct PAD_BLOCK{
	u8 UP_BLOCKED;
	u8 DOWN_BLOCKED;
	u8 LEFT_BLOCKED;
	u8 RIGHT_BLOCKED;
	
	struct timeval UP_COOLDOWN;
	struct timeval DOWN_COOLDOWN;
	struct timeval LEFT_COOLDOWN;
	struct timeval RIGHT_COOLDOWN;
}PAD_BLOCK;

enum userAction{
	ACTION_MENU = 0,
	ACTION_TROPHY_LOAD,
	ACTION_TROHPY,
}uAction = ACTION_MENU;


// ----- VARIABLES ----- //
static char hdd_folder_installed[64] = "RNA000001"; // folder for program

//Tiny3D Resources
u32 *texture_mem;
u32 * texture_pointer;
PngDatas IMAGES[N_IMAGES]; // PNG container of texture3
u32 IMAGES_offset[N_IMAGES] = {0}; // offset for texture3 (used to pass the texture)

//SDK modules
u32 module_flag;

//Store Status
static bool running = 1;
u32 menu_sel = 0;
u8 fade = 0xFF;
u32 fade_anim = 0;
PAD_BLOCK pad_block = {0};

struct timeval menu_fade1 = {0};
struct timeval menu_fade2 = {0};

// ----- PROTOTYPES ----- //
void Debug( char *format, ... );
void drawScene( void );
void releaseAll( void );
int loadModules( void );
void customLoadPNG( void );
void loadTexture( void );
void drawScene( void );
static void sys_callback(uint64_t status, uint64_t param, void* userdata);


/// ---- CODE ---- ///

void Debug(char *format, ...){
	char str[1024];
	char filename[1024];

    va_list argp;        
    va_start(argp, format);
	vsprintf(str, format, argp);
	va_end(argp);

	FILE *fDebug;
	sprintf(filename, "/dev_hdd0/game/%s/USRDIR/log.txt",hdd_folder_installed);
	fDebug = fopen(filename, "a+");

	fputs(str, fDebug);

	fclose(fDebug);
}

void releaseAll() {
	
	sysUnregisterCallback(EVENT_SLOT0);
	
	if(module_flag & 4)
		SysUnloadModule( SYSMODULE_IO );

	if(module_flag & 2)
		SysUnloadModule(SYSMODULE_PNGDEC);

	if(module_flag & 1)
		SysUnloadModule(SYSMODULE_FS);

	running = 0;
}

int loadModules( void ){
	if(SysLoadModule(SYSMODULE_FS)!=0) return 0; else module_flag |=1;

	if(SysLoadModule(SYSMODULE_PNGDEC)!=0) return 0; else module_flag |=2;
	
	if(SysLoadModule(SYSMODULE_IO)!=0) return 0; else module_flag |=4;

	return 1;
}

static void trophie_sync(u64 arg){

	struct dirent *dp;

	const char *dir_path="dev_hdd0/home/trophies";
	DIR *dir = opendir(dir_path);
	while ((dp=readdir(dir)) != NULL) {
		if(dp->d_type == DT_DIR){

		}
	}
	closedir(dir);

	sys_ppu_thread_yield();
	sleep(2);

	sys_ppu_thread_exit(0);
}

volatile int dialog_action = 0;
void my_dialog(msgButton button, void *userdata)
{
    switch(button) {

        case MSGDIALOG_BUTTON_OK:
            dialog_action = 1;
            break;
        case MSGDIALOG_BUTTON_NO:
        case MSGDIALOG_BUTTON_ESCAPE:
            dialog_action = 2;
            break;
        case MSGDIALOG_BUTTON_NONE:
            dialog_action = -1;
            break;
        default:
		    break;
    }
}

u8 oskdialog_mode = 0;

static void sys_callback(uint64_t status, uint64_t param, void* userdata) {

     switch (status) {
		case EVENT_REQUEST_EXITAPP:			
			oskdialog_mode = 5;
			releaseAll();
			sysProcessExit(1);
			break;

		case OSK_LOADED:
			break;

		case OSK_FINISHED:
			oskdialog_mode = 5;
			break;

		case OSK_UNLOADED:
			break;

		case OSK_INPUT_ENTERED:
			oskdialog_mode = 3;
			break;

		case OSK_INPUT_CANCELED:
			oskdialog_mode = 4;
			break;
				
		default:
		   break;
         
	}
}


void drawScene(){
	
	tiny3d_Project2D();
	
	DrawTexture2d(0, 0, 741.5583, 511.0518, IMAGES_offset[0], IMAGES[0]); //MASK 1
	DrawTexture2d(0, 0, 848, 512, IMAGES_offset[1], IMAGES[1]); //MASK 2

	//MENU FADE (FLECHAS HORIZONTALES)
	if(fade_anim > 0){
		gettimeofday(&menu_fade2, NULL);

		u32 d1 = (menu_fade1.tv_sec * 1000) + (menu_fade1.tv_usec / 1000);
		u32 d2 = (menu_fade2.tv_sec * 1000) + (menu_fade2.tv_usec / 1000);

		u32 at = ((d2 > d1)?d2-d1:d1-d2);
		
		if(at > 0){		
			if(fade_anim == 1){
				fade = 0xFF - (at * 0xFF / 1000); 
				if(fade == 0 || (255 - (at * 255 / 1000)) <= 0 ){
					gettimeofday(&menu_fade1, NULL);
					fade_anim = 2;
				}
			}else if(fade_anim == 2){
				fade = (at * 0xFF / 1000);
				if(fade == 0xFF || (at * 255 / 1000) >= 255)
					fade_anim = 0;
			}
		}
	}
	
	//MENU DRAW
	u32 color = 0xFFFFFF00;
	color += fade;

	SetCurrentFont(1);
	SetFontColor(color, 0x0);
	SetFontSize(32, 38);

	float x = 625.4, y = 294.4;
	for(int i = 0; i < 4; i++){
		if(menu_sel == i)
			DrawTexture2d(x, y, 34.45, 41.2444, IMAGES_offset[3], IMAGES[3]);
		else
			DrawTexture2d(x, y, 34.45, 41.2444, IMAGES_offset[2], IMAGES[2]);

		if(i == 0)
			DrawString(x + 20, y + 41.2444/2 - 38/2 - 5, "AMIGOS");
		else if(i == 1)
			DrawString(x + 20, y + 41.2444/2 - 38/2 - 5, "TROFEOS");
		else if(i == 2)
			DrawString(x + 20, y + 41.2444/2 - 38/2 - 5, "SAVES");
		else if(i == 3)
			DrawString(x + 20, y + 41.2444/2 - 38/2 - 5, "NETWORK");

		y += 38.8740;
	}

	x = 632.9083;
	y = 219.9703;
	
	SetFontColor(0xFFFFFFFF, 0x0);	
	DrawString(x, y, "Bienvenido,\n");

	SetFontColor(0xFFFD00FF, 0x0);
	DrawFormatString(632.9083 + 30, GetFontY(), "%s", "Blipi");


	switch(uAction){
		case ACTION_TROPHY_LOAD:{
			sys_ppu_thread_t id;
			u64 priority = 1500;
			size_t stack_size = 0x1000;
			sys_ppu_thread_create(	&id, trophie_sync, (u64)NULL, priority, stack_size, THREAD_JOINABLE, "TROPHY_SYNC");

			msgType mdialogprogress = MSGDIALOG_SINGLE_PROGRESSBAR;
    
			msgDialogOpen2(mdialogprogress, "Sincronizando trofeos", my_dialog, (void *) 0x33330001, NULL);
			msgDialogProgressBarMessage(PROGRESSBAR_INDEX0, "Espere");
			msgDialogResetProgressBar(PROGRESSBAR_INDEX0);
   
			dialog_action = 0;
			while(!dialog_action){
				sysCheckCallback();
				tiny3d_Flip();
			}

			msgDialogClose();
		}break;
		default:
			break;
	}
}

void customLoadPNG(){

	for(int i = 0; i < N_IMAGES; i++){
		char filename[1024] = {0};

		if(i==0) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/MASK1.PNG",hdd_folder_installed);
		if(i==1) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/MASK2.PNG",hdd_folder_installed);
		if(i==2) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/MENU.PNG",hdd_folder_installed);
		if(i==3) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/MENUSEL.PNG",hdd_folder_installed);
		if(i==4) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/LOGO.PNG",hdd_folder_installed);
		if(i==5) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/CRUCETA.PNG",hdd_folder_installed);
		if(i==6) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/X.PNG",hdd_folder_installed);
		if(i==7) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/O.PNG",hdd_folder_installed);
		if(i==8) sprintf(filename, "/dev_hdd0/game/%s/USRDIR/SQ.PNG",hdd_folder_installed);
			
		memset(&IMAGES[i], 0, sizeof(PngDatas));

		LoadPNG(&(IMAGES[i]), filename);
	}
}

void loadTexture()
{
    texture_mem = (u32*)tiny3d_AllocTexture(64*1024*1024); // alloc 64MB of space for textures (this pointer can be global)    

    if(!texture_mem) return; // fail!

    texture_pointer = texture_mem;
   
	customLoadPNG();
	
	for(int i = 0; i < N_IMAGES; i++){
		IMAGES_offset[i] = 0;
		if(IMAGES[i].bmp_out){
			memcpy(texture_pointer, IMAGES[i].bmp_out, IMAGES[i].wpitch * IMAGES[i].height);
            
			free(IMAGES[i].bmp_out);
			IMAGES[i].bmp_out = texture_pointer;
			texture_pointer += (IMAGES[i].wpitch/4 * IMAGES[i].height + 3) & ~3; 
			IMAGES_offset[i] = tiny3d_TextureOffset(IMAGES[i].bmp_out);      // get the offset (RSX use offset instead address)
		}
	}

	ResetFont();
	
	char filename[1024] = {0};

	sprintf(filename, "/dev_hdd0/game/%s/USRDIR/Base02.ttf",hdd_folder_installed);
    TTFLoadFont(filename, (void*)NULL, 0);
    texture_pointer = (u32 *) AddFontFromTTF((u8 *) texture_pointer, 32, 255, 32, 32, TTF_to_Bitmap);
    TTFUnloadFont();	
		
	sprintf(filename, "/dev_hdd0/game/%s/USRDIR/Decibel_2.ttf",hdd_folder_installed);
    TTFLoadFont(filename, (void*)NULL, 0);
    texture_pointer = (u32 *) AddFontFromTTF((u8 *) texture_pointer, 32, 255, 32, 32, TTF_to_Bitmap);
    TTFUnloadFont();	
}

/*** - FUNCION MAIN - ***/
int main(int argc, const char* argv[], const char* envp[])
{
	tiny3d_Init(1024*1024);
	tiny3d_Project2D();

	AdjustViewport( -42.0f, -42.0f );

	loadModules();
		
	ioPadInit(7);
	
    loadTexture();
	
	//OSK
	int ret;
	mem_container_t container_id;
	ret = lv2MemContinerCreate( &container_id, 8 * 1024 * 1024 );
	Debug("[MEM] %d\n", ret);

	char orig[512] = "msg";
	char init_text[512] = "abc";

    wchar_t my_message[((strlen(orig) + 1)*2)];
    mbstowcs(my_message, orig, (strlen(orig) + 1));

    wchar_t INIT_TEXT[((strlen(init_text) + 1)*2)];
    mbstowcs(INIT_TEXT, init_text, (strlen(init_text) + 1));

	oskInputFieldInfo inputFieldInfo;
	inputFieldInfo.message = (uint16_t*)my_message;
	inputFieldInfo.startText = (uint16_t*)INIT_TEXT;
	inputFieldInfo.maxLength = 128;
	
	oskCallbackReturnParam OutputInfo;	
	OutputInfo.result = OSK_OK;	
	OutputInfo.length = 10;	
	uint16_t Result_Text_Buffer[512 + 1];
	OutputInfo.str = Result_Text_Buffer;

	oskSetKeyLayoutOption (OSK_10KEY_PANEL | OSK_FULLKEY_PANEL);
	oskAddSupportLanguage (OSK_PANEL_TYPE_ALPHABET | OSK_PANEL_TYPE_NUMERAL |
		OSK_PANEL_TYPE_ENGLISH |
		OSK_PANEL_TYPE_DEFAULT |
		OSK_PANEL_TYPE_SPANISH |
		OSK_PANEL_TYPE_FRENCH |
		OSK_PANEL_TYPE_RUSSIAN |
		OSK_PANEL_TYPE_JAPANESE |
		OSK_PANEL_TYPE_TRADITIONAL_CHINESE);

	oskParam dialogParam;
	
	dialogParam.allowedPanels = (OSK_PANEL_TYPE_ALPHABET | OSK_PANEL_TYPE_NUMERAL |
		OSK_PANEL_TYPE_ENGLISH |
		OSK_PANEL_TYPE_DEFAULT |
		OSK_PANEL_TYPE_SPANISH |
		OSK_PANEL_TYPE_FRENCH |
		OSK_PANEL_TYPE_RUSSIAN |
		OSK_PANEL_TYPE_JAPANESE |
		OSK_PANEL_TYPE_TRADITIONAL_CHINESE);
	dialogParam.firstViewPanel = OSK_PANEL_TYPE_ALPHABET_FULL_WIDTH;

	oskSetLayoutMode( OSK_LAYOUTMODE_HORIZONTAL_ALIGN_CENTER );

	oskPoint pos = {0.0, 0.0};

	dialogParam.controlPoint = pos;
	dialogParam.prohibitFlags = OSK_PROHIBIT_RETURN;
	oskSetInitialInputDevice(OSK_DEVICE_PAD);
	
	sysRegisterCallback(EVENT_SLOT0, sys_callback, NULL);

	ret = oskLoadAsync(container_id, &dialogParam, &inputFieldInfo);	
	Debug("[LOAD] %d\n", ret);

	oskdialog_mode = 2;
	
	/*** - MAIN LOOP - ***/
	while( running ){
		/*
		tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);

        // Enable alpha Test
        tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

        // Enable alpha blending.
        tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
            NV30_3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | NV30_3D_BLEND_FUNC_DST_ALPHA_ZERO,
            TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
		*/
		//drawScene();
			

		//OSK OSK OSK
		if( oskdialog_mode == 3 ){
			oskGetInputText( &OutputInfo );

			oskdialog_mode = 2;
		}
		else if( oskdialog_mode == 4 ){
			oskdialog_mode = 2;
			oskAbort();
		}
		else if( oskdialog_mode == 5 ){
			oskUnloadAsync( &OutputInfo );
		}
		
		PadInfo padinfo;
		PadData paddata;
		ioPadGetInfo(&padinfo);

		for(int i = 0; i < MAX_PADS; i++){
			if(padinfo.status[i]){
				ioPadGetData(i, &paddata);
				
				if(paddata.BTN_UP && !pad_block.UP_BLOCKED){
					if(menu_sel < 4)
						menu_sel++;

					pad_block.UP_BLOCKED = 1;
					gettimeofday(&pad_block.UP_COOLDOWN, NULL);
				}
				if(paddata.BTN_DOWN && !pad_block.DOWN_BLOCKED){
					if(menu_sel > 0)
						menu_sel--;

					pad_block.DOWN_BLOCKED = 1;
					gettimeofday(&pad_block.DOWN_COOLDOWN, NULL);
				}

				if(paddata.BTN_LEFT && !pad_block.LEFT_BLOCKED){
					fade_anim = 1;
					gettimeofday(&menu_fade1, NULL);

					pad_block.LEFT_BLOCKED = 1;
					gettimeofday(&pad_block.LEFT_COOLDOWN, NULL);
				}
				if(paddata.BTN_RIGHT && !pad_block.RIGHT_BLOCKED){
					fade_anim = 1;
					gettimeofday(&menu_fade1, NULL);

					pad_block.RIGHT_BLOCKED = 1;
					gettimeofday(&pad_block.RIGHT_COOLDOWN, NULL);
				}
			}
		}

        tiny3d_Flip();

		sysCheckCallback();

	}
	
	return 0;
}
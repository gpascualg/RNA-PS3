// ----- LOCAL INCLUDES ----- //
#include "headers.h"
#include "HTTP.h"
#include "font.h"
#include "graphics.h"
#include "xml.h"


#include <sys\time.h>
#include <limits.h>

// ----- DEFINES ----- //


// ----- VARIABLES ----- //
static char hdd_folder_installed[64] = "OPST00002"; // folder for program

//Tiny3D Resources
u32 *texture_mem;
u32 * texture_pointer;

//SDK modules
u32 module_flag;

//Store Status
static bool running = 1;


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
	//sys_ppu_thread_join(thread1_id, &retval);
	
	if(module_flag & 2)
		SysUnloadModule(SYSMODULE_PNGDEC);

	if(module_flag & 1)
		SysUnloadModule(SYSMODULE_FS);

	if(!OFFLINE_MODE)
		endSocket();
	
	running = 0;
}

int loadModules( void ){
	if(SysLoadModule(SYSMODULE_FS)!=0) return 0; else module_flag |=1;

	if(SysLoadModule(SYSMODULE_PNGDEC)!=0) return 0; else module_flag |=2;

	return 1;
}

static void sys_callback(uint64_t status, uint64_t param, void* userdata) {

     switch (status) {
		case EVENT_REQUEST_EXITAPP:
			releaseAll();
			sysProcessExit(1);
			break;
      
       default:
		   break;
         
	}
}


void drawScene(){
	
	tiny3d_Project2D(); 

	PadInfo padinfo;
	PadData paddata;
	ioPadGetInfo(&padinfo);
	
	
}

void loadTexture()
{
    texture_mem = (u32*)tiny3d_AllocTexture(64*1024*1024); // alloc 64MB of space for textures (this pointer can be global)    

    if(!texture_mem) return; // fail!

    texture_pointer = texture_mem;
   
	ResetFont();
	
	char filename[1024] = {0};

	sprintf(filename, "/dev_hdd0/game/%s/USRDIR/SCE-PS3-SR-R-LATIN.ttf",hdd_folder_installed);
    TTFLoadFont(filename, NULL, NULL);
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

	sysRegisterCallback(EVENT_SLOT0, sys_callback, NULL);


	/*** - MAIN LOOP - ***/
	while( running ){
		
		tiny3d_Clear(0x000000ff, TINY3D_CLEAR_ALL);

        // Enable alpha Test
        tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

        // Enable alpha blending.
        tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
            NV30_3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | NV30_3D_BLEND_FUNC_DST_ALPHA_ZERO,
            TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);

		drawScene();
				
        tiny3d_Flip();

		sysCheckCallback();

	}
	
	return 0;

}
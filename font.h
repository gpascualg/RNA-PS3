
#include <libfont.h>
#include <ft2build.h>
#include <freetype/freetype.h> 
#include <freetype/ftglyph.h>

#define size_font 28672

unsigned char font[28672];

int TTFLoadFont(char * path, void * from_memory, int size_from_memory);
void TTFUnloadFont();
void TTF_to_Bitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction);
#ifndef FONT_H
#define FONT_H

#include <libfont.h>
#include <ft2build.h>
#include <freetype/freetype.h> 
#include <freetype/ftglyph.h>

int TTFLoadFont(char * path, void * from_memory, int size_from_memory);
void TTFUnloadFont();
void TTF_toBitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction);

#endif

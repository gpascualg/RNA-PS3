#include "font.h"

int ttf_inited;
FT_Library f_freetype;
FT_Face f_face;

int TTFLoadFont(char * path, void * from_memory, int size_from_memory)
{
   
    if(!ttf_inited)
        FT_Init_FreeType(&f_freetype);
    ttf_inited = 1;

    if(path) {
        if(FT_New_Face(f_freetype, path, 0, &f_face)) return -1;
    } else {
        if(FT_New_Memory_Face(f_freetype, (const FT_Byte*)from_memory, size_from_memory, 0, &f_face)) return -1;
    }

    return 0;
}

void TTFUnloadFont()
{
   FT_Done_FreeType(f_freetype);
   ttf_inited = 0;
}

void TTF_toBitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction)
{
    FT_Set_Pixel_Sizes(f_face, (*w), (*h));
    
    FT_GlyphSlot slot = f_face->glyph;

    memset(bitmap, 0, (*w) * (*h));

    if(FT_Load_Char(f_face, (char) chr, FT_LOAD_RENDER )) {(*w) = 0; return;}

    int n, m, ww;

    *y_correction = (*h) - 1 - slot->bitmap_top;
    
    ww = 0;

    for(n = 0; n < slot->bitmap.rows; n++) {
        for (m = 0; m < slot->bitmap.width; m++) {

            if(m >= (*w) || n >= (*h)) continue;
            
            bitmap[m] = (u8) slot->bitmap.buffer[ww + m];
        }
    
    bitmap += *w;

    ww += slot->bitmap.width;
    }

    *w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0);
    *h = slot->bitmap.rows;
}
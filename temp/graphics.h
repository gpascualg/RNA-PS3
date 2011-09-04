#include "headers.h"

typedef struct RECT{
	float x;
	float y;
	float x2;
	float y2;
}RECT;

void DrawTexture2d(float x, float y, float dx, float dy, u32 texture_offset, PngDatas texture);
void DrawTextureRect2d(int x, int y, int w, int h, RECT rect, u32 texture_offset, PngDatas texture);
void DrawBackground2D(u32 rgba);
struct _SIZE GetTextSize(int sx, int sy, const char* str);
void AdjustViewport( float x, float y );
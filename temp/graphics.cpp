#include "graphics.h"

void DrawTexture2d(float x, float y, float dx, float dy, u32 texture_offset, PngDatas texture)
{
    tiny3d_SetTexture(0, texture_offset, texture.width, texture.height, texture.wpitch, TINY3D_TEX_FORMAT_A8R8G8B8, 1);
    tiny3d_SetPolygon(TINY3D_QUADS);
	
    tiny3d_VertexPos(x  , y  , 1);   
    tiny3d_VertexColor(0xFFFFFFFF);
    tiny3d_VertexTexture(0.0f, 0.0f);

    tiny3d_VertexPos(x + dx, y  , 1);
    tiny3d_VertexTexture(0.99f, 0.0f);

    tiny3d_VertexPos(x + dx, y + dy, 1);
    tiny3d_VertexTexture(0.99f, 0.99f);

    tiny3d_VertexPos(x  , y + dy, 1);
    tiny3d_VertexTexture(0.0f, 0.99f);
    tiny3d_End();
}

void DrawTextureRect2d(int x, int y, int w, int h, RECT rect, u32 texture_offset, PngDatas texture)
{
    tiny3d_SetTexture(0, texture_offset, texture.width, texture.height, texture.wpitch, TINY3D_TEX_FORMAT_A8R8G8B8, 1);
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(x  , y  , 1);   
    tiny3d_VertexColor(0xFFFFFFFF);
    tiny3d_VertexTexture(rect.x / w, rect.y / h);
	
	tiny3d_VertexPos(rect.x2, y  , 1);
	tiny3d_VertexTexture(rect.x / w, rect.y / h);

	tiny3d_VertexPos(rect.x2, rect.y2, 1);
	tiny3d_VertexTexture(rect.x2 / w, rect.y2 / h);

	tiny3d_VertexPos(x  , rect.y2, 1);
    tiny3d_VertexTexture(0.0f, rect.y2 / h);
    tiny3d_End();
}

void DrawBackground2D(u32 rgba)
{
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(0  , 0  , 65535);
    tiny3d_VertexColor(rgba);

    tiny3d_VertexPos(847, 0  , 65535);

    tiny3d_VertexPos(847, 511, 65535);

    tiny3d_VertexPos(0  , 511, 65535);
    tiny3d_End();
}


struct _SIZE GetTextSize(int sx, int sy, const char* str){

	u8 *buf = (u8 *) str;

	long width = 0;
	long height = 0;
	struct _SIZE rSize = {0};

	while(*buf) {

		if(*buf == '\n') {width = 0; height += sy;}
		else if(*buf == 9) width+=sx;
		else if(*buf >= sy) {
			width+=sx;
		}

		buf++;
		
		if(width > rSize.width) rSize.width = width;

		if(width < 0) width= 0;
		if(width > (848-sx)) {width= 0; height+= sy;}
		if(height < 0) height= 0;
		if(height > (512-sy)) { rSize.height = -1; }
	
		if(rSize.height != -1) rSize.height = height;

	}

	if( rSize.height == 0 )
		 rSize.height = sy;

	return rSize;
}

void AdjustViewport( float x, float y )
{
	double sx = (double) Video_Resolution.width;
	double sy = (double) Video_Resolution.height;
	double px = (double) (1000 + x)/1000.0;
	double py = (double) (1000 + y)/1000.0;

	tiny3d_UserViewport( 1,
						(float) ((sx - sx * px) / 2.0), // 2D position
						(float) ((sy - sy * py) / 2.0),
						(float) ((sx * px) / 848.0),    // 2D scale
						(float) ((sy * py) / 512.0),
						(((float) Video_Resolution.width) / 1920.0f) * (float) (1000 + x)/1000.0f,  // 3D scale
						(((float) Video_Resolution.height) / 1080.0f) * (float) (1000 + y)/1000.0f);



}
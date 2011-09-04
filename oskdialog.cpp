#include "oskdialog.h"

u8 osk_loaded = 0;
FT_Face face;

char key_array[2][4][10] = {
	{
		{'!', '"', '·', '$', '%', '&', '/', '(', ')', '='},
		{'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
		{'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Ñ'},
		{'Z', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_'}
	},
	{
		{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
		{'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
		{'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ñ'},
		{'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-'}
	}
};

struct block_data{
	u8 blocked;
	u16 delay;
	long uval;
};

block_data lblock = {0}, rblock = {0}, ublock = {0}, dblock = {0};
block_data xblock = {0};



void TTF_to_Bitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction){
	FT_Set_Pixel_Sizes(face, (*w), (*h));
    
	FT_GlyphSlot slot = face->glyph;

	memset(bitmap, 0, (*w) * (*h));

	if(FT_Load_Char(face, (char) chr, FT_LOAD_RENDER )) {(*w) = 0; return;}

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

void Debug(char *format, ...){
	char str[1024];
	char filename[1024];

    va_list argp;        
    va_start(argp, format);
	vsprintf(str, format, argp);
	va_end(argp);

	FILE *fDebug;
	sprintf(filename, "/dev_hdd0/game/%s/USRDIR/log.txt", "RNA000001");
	fDebug = fopen(filename, "a+");

	fputs(str, fDebug);

	fclose(fDebug);
}



OSK::OSK(u32 *texture_pointer, char *usrdir){
	this->png_displayer = &this->png_displayer_f;
	this->png_loader = NULL;
	this->status = OSK_INITIALIZED;
	this->mode = 1;
	this->sel_row = 0;
	this->sel_num = 0;
	this->ttf_inited = 0;
	this->font_slot = -1;
	this->texture_pointer = texture_pointer;

	if(osk_loaded == 0){
		char path[1024] = {0};
		
		sprintf(path, "%s/BTN.PNG", usrdir);
		this->btn = this->png_loader_f(path, this->texture_pointer);
		this->texture_pointer = this->btn->texture_pointer;

		sprintf(path, "%s/SPACE.PNG", usrdir);
		this->space = this->png_loader_f(path, this->texture_pointer);		
		this->texture_pointer = this->space->texture_pointer;

		osk_loaded = 1;
	}
}

OSK::OSK(method_a *png_loader, method_b *png_displayer, char *usrdir){
	this->png_loader = png_loader;
	this->png_displayer = png_displayer;
	this->status = OSK_INITIALIZED;
	this->mode = 1;
	this->sel_row = 0;
	this->sel_num = 0;
	this->ttf_inited = 0;
	this->font_slot = -1;
	this->texture_pointer = 0;

	if(osk_loaded == 0){
		char path[1024] = {0};
		
		sprintf(path, "%s/BTN.PNG", usrdir);
		this->btn = (*this->png_loader)(path);
		this->btn->width = 35.3333;
		this->btn->height = 41.2444;

		sprintf(path, "%s/SPACE.PNG", usrdir);
		this->space = (*this->png_loader)(path);

		osk_loaded = 1;
	}
}

u32* OSK::loadFont(u8 slot, char *usrdir, u32 *texture_pointer){
	this->font_slot = slot;
	
	if(!usrdir)
		return this->texture_pointer;

	if(!texture_pointer)
		return NULL;

	if(slot == 0)
		ResetFont();

	char path[1024] = {0};		
	sprintf(path, "%s/pala.ttf", usrdir);

	if(!ttf_inited){
		FT_Init_FreeType(&freetype);
		ttf_inited = 1;
	}

    if(FT_New_Face(freetype, path, 0, &face)) return NULL;
	
	this->texture_pointer = (u32 *) AddFontFromTTF((u8 *) texture_pointer, 32, 255, 32, 32, TTF_to_Bitmap);

	FT_Done_FreeType(freetype);
	ttf_inited = 0;

	return this->texture_pointer;
}

void OSK::DrawRect2D(u32 rgba, float x, float y, float width, float height){
    tiny3d_SetPolygon(TINY3D_QUADS);
	
    tiny3d_VertexPos(x  , y  , 65535);
    tiny3d_VertexColor(rgba);

    tiny3d_VertexPos(x + width, y  , 65535);

    tiny3d_VertexPos(x + width, y + height, 65535);

    tiny3d_VertexPos(x , y + height, 65535);
    tiny3d_End();
}

void OSK::DrawBorders2D(u32 rgba, float x, float y, float w, float h, u32 border){
	this->DrawRect2D(rgba, x-border, y-border, w+(border*2), border);
	this->DrawRect2D(rgba, x+w, y-border, border, h+(border*2));
	this->DrawRect2D(rgba, x-border, y+h, w+(border*2), border);
	this->DrawRect2D(rgba, x-border, y-border, border, h+(border*2));
}

u8 OSK::draw(){
	if(status != OSK_RUNNING)
		return -1;

	float x0;
	float y0;

	float x = x0 = this->pos.x;
	float y = y0 = this->pos.y;
	
	float w = 35.3333;
	float h = 41.2444;

	//barra donde se muestra lo escrito
	this->btn->x = x + w/2;
	this->btn->y = y;
	this->btn->width = (w+5)*9;
	this->btn->alpha = 0x99;
	if(this->png_displayer != NULL)
		(*this->png_displayer)(this->btn);
	else
		this->png_displayer_f(this->btn);
	
	if(this->font_slot >= 0){		
		SetCurrentFont(this->font_slot);
		SetFontSize(18, 20);
		SetFontColor(0xFFFFFFFF, 0x0);
		
		DrawString(x + w/2 + 15, y + h/2 - 20/2 - 5, this->buffer);
	}

	this->btn->width = w;
	this->btn->height = h;
	this->btn->alpha = 0xFF;

	y = y0 = y0 + h + 10;

	for(int i = 0; i < 4; i++){
		for(int n = 0; n < 10; n++){

			this->btn->x = x;
			this->btn->y = y;

			if(this->png_displayer != NULL)
				(*this->png_displayer)(this->btn);
			else
				this->png_displayer_f(this->btn);

			if(this->sel_num == n && this->sel_row == i)
				this->DrawBorders2D(0xFFFFFFFF, x, y, w-1, h, 3);

			if(this->font_slot >= 0)
				DrawFormatString(x + w/2 - 18/2 + 3, y + h/2 - 20/2 - 5, "%c", key_array[this->mode][i][n]);
			
			x += w + 5;
		}

		x = x0 = x0 + w/2;
		y = y0 = y0 + h + 10;
	}

	return 0;
}

u8 OSK::handlePad(){
	if(this->status != OSK_RUNNING)
		return -1;

	PadData paddata;
	PadInfo padinfo;

	ioPadGetInfo(&padinfo);

	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	long cur_time = time_now.tv_sec * 1000 + time_now.tv_usec/1000;
	
	for(int i = 0; i < MAX_PADS; i++){
		if(padinfo.status[i]){
			ioPadGetData(i, &paddata);
			
			//ioPadSetPortSetting(i, CELL_PAD_SETTING_PRESS_ON);
			ioPadSetPortSetting(i, CELL_PAD_SETTING_SENSOR_ON);

			s8 plusn = 0;
			s8 plusr = 0;

			if(paddata.BTN_RIGHT){
				if(!rblock.blocked){
					plusn = 1;
					rblock.blocked = 1;
					rblock.delay = 1000;
					rblock.uval = cur_time;
				}else{
					long elapsed = ((cur_time-rblock.uval > 0)?cur_time-rblock.uval:rblock.uval-cur_time);
					if(elapsed >= rblock.delay){
						plusn = 1;
						rblock.uval = cur_time;
						rblock.delay = 300;
					}
				}
			}else
				rblock.blocked = 0;
			
			if(paddata.BTN_LEFT){
				if(!lblock.blocked){
					plusn = -1;
					lblock.blocked = 1;
					lblock.delay = 1000;
					lblock.uval = cur_time;
				}else{
					long elapsed = ((cur_time-lblock.uval > 0)?cur_time-lblock.uval:lblock.uval-cur_time);
					if(elapsed >= lblock.delay){
						plusn = -1;
						lblock.uval = cur_time;
						lblock.delay = 300;
					}
				}
			}else
				lblock.blocked = 0;

			if(paddata.BTN_UP){
				if(!ublock.blocked){
					plusr = -1;
					ublock.blocked = 1;
					ublock.delay = 1000;
					ublock.uval = cur_time;
				}else{
					long elapsed = ((cur_time-ublock.uval > 0)?cur_time-ublock.uval:ublock.uval-cur_time);
					if(elapsed >= ublock.delay){
						plusr = -1;
						ublock.uval = cur_time;
						ublock.delay = 300;
					}
				}
			}else
				ublock.blocked = 0;

			if(paddata.BTN_DOWN){
				if(!dblock.blocked){
					plusr = 1;
					dblock.blocked = 1;
					dblock.delay = 1000;
					dblock.uval = cur_time;
				}else{
					long elapsed = ((cur_time-dblock.uval > 0)?cur_time-dblock.uval:dblock.uval-cur_time);
					if(elapsed >= dblock.delay){
						plusr = 1;
						dblock.uval = cur_time;
						dblock.delay = 300;
					}
				}
			}else
				dblock.blocked = 0;

			if(paddata.BTN_CROSS){
				if(!xblock.blocked){
					
					if(this->sel_num >= 0 && this->sel_num <= 9 && this->sel_row >= 0 && this->sel_row <= 3){
						if(strlen(this->buffer) >= 255)
							continue;

						char buf[5];
						sprintf(buf, "%c", key_array[this->mode][this->sel_row][this->sel_num]);
						strcat(this->buffer, buf);
					}

					xblock.blocked = 1;
					xblock.delay = 1000;
					xblock.uval = cur_time;
				}else{
					long elapsed = ((cur_time-xblock.uval > 0)?cur_time-xblock.uval:xblock.uval-cur_time);
					if(elapsed >= xblock.delay){
						
						if(this->sel_num >= 0 && this->sel_num <= 9 && this->sel_row >= 0 && this->sel_row <= 3){
							if(strlen(this->buffer) >= 255)
								continue;

							char buf[5];
							sprintf(buf, "%c", key_array[this->mode][this->sel_row][this->sel_num]);
							strcat(this->buffer, buf);
						}

						xblock.uval = cur_time;
						xblock.delay = 300;
					}
				}
			}else
				xblock.blocked = 0;

			if(paddata.BTN_START){
				this->status = OSK_RETURN;
				//Callback
			}

			if(plusn != 0){
				this->sel_num += plusn;

				if(this->sel_num < 0)
					this->sel_num = 9;
				else if(this->sel_num >= 10)
					this->sel_num = 0;
			}
			
			if(plusr != 0){
				this->sel_row += plusr;

				if(this->sel_row < 0)
					this->sel_row = 3;
				else if(this->sel_row >= 4)
					this->sel_row = 0;
			}

			break; //Solo queremos el PAD 0
		}		
	}

	return 0;
}

void OSK::setPos(osk_point npos){
	this->pos.x = npos.x;
	this->pos.y = npos.y;
}
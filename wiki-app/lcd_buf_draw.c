#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#ifdef WIKIPCF
#include <wchar.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <file-io.h>
#include <guilib.h>
#include <malloc-simple.h>
#include <msg.h>
#include <lcd.h>
#include <input.h>
#include "delay.h"
#include <tick.h>
#include "history.h"
#include "search.h"
#include "glyph.h"
#endif

#include "bmf.h"
#include "lcd_buf_draw.h"
#include "search.h"
#include "bigram.h"

#define ARTICLE_LINK_COUNT 1000
#define EXTERNAL_LINK_COUNT 1000
#define LCD_UPDATE_MILLISEC 20000

int get_time_tick(void);

pcffont_bmf_t pcfFonts[FONT_COUNT];
static int lcd_draw_buf_inited = 0;
LCD_DRAW_BUF lcd_draw_buf;
unsigned char * file_buffer;
int lcd_draw_buf_size = 0;
int lcd_draw_buf_pos  = 0;
int lcd_draw_cur_y_pos = 0;

int request_display_next_page = 0;
int request_y_pos = 0;
int cur_render_y_pos = 0;

ARTICLE_LINK articleLink[ARTICLE_LINK_COUNT];
EXTERNAL_LINK externalLink[EXTERNAL_LINK_COUNT];

int article_link_count;
int external_link_count;
int is_display_external_link = 0;
int display_first_page = 0;

void lcd_set_pixel(unsigned char *membuffer,int x, int y);
unsigned char * open_article_with_pcf_link(long idx_article);

int display_link_article(long article_link_number);
void drawline_in_framebuffer_copy(unsigned char *buffer,int start_x,int start_y,int end_x,int end_y);
void buf_draw_char_external(LCD_DRAW_BUF *lcd_draw_buf_external,ucs4_t u,int start_x,int end_x,int start_y,int end_y);
int get_external_str_pixel_width(char **pUTF8);
void repaint_framebuffer(unsigned char *buf,int pos);
char* FontFile(int idx);
void msg_info(char *data);
int framebuffer_size();
int framebuffer_width();
int framebuffer_height();

unsigned char *framebuffer_copy;
char msg_out[1024];
extern unsigned char *framebuffer;
int link_article_number_cur;
int article_scroll_delay_time,article_offset,base_delay_time;
int article_distance,article_offset,article_scroll_increment;
int time_scroll_article_start;
int article_scroll_delay_time_total,time_scroll_article_last=0;
int stop_render_article = 0;

void init_lcd_draw_buf()
{
	int i,framebuffersize,fd;

	if (!lcd_draw_buf_inited)
	{
                framebuffersize = framebuffer_size();
                framebuffer_copy = (unsigned char*)Xalloc(framebuffersize);

		lcd_draw_buf.screen_buf = (unsigned char *)Xalloc(LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);

		for (i=0; i < FONT_COUNT; i++)
		{
			pcfFonts[i].file = FontFile(i);
                        fd = load_bmf(&pcfFonts[i]);
			if(fd >= 0)
		        {
				if (i == ITALIC_FONT_IDX - 1)
			   	{
					pcfFonts[i].bPartialFont = 1;
					pcfFonts[i].supplement_font = &pcfFonts[DEFAULT_ALL_FONT_IDX - 1];
			   	}
			   	else if (i == DEFAULT_FONT_IDX - 1)
			   	{
					pcfFonts[i].bPartialFont = 1;
					pcfFonts[i].supplement_font = &pcfFonts[DEFAULT_ALL_FONT_IDX - 1];
				}
				else if (i == BOLD_ITALIC_FONT_IDX - 1)
				{
					pcfFonts[i].bPartialFont = 1;
					pcfFonts[i].supplement_font = &pcfFonts[BOLD_ALL_FONT_IDX - 1];
				}
				else if (i == BOLD_FONT_IDX - 1)
				{
					pcfFonts[i].bPartialFont = 1;
					pcfFonts[i].supplement_font = &pcfFonts[BOLD_ALL_FONT_IDX - 1];
				}
				else
				{
                       			pcfFonts[i].bPartialFont = 0;
				}
				pcfFonts[i].fd = fd;
			
		        }

		}
		lcd_draw_buf_inited = 1;
	}
	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = 0;
	lcd_draw_buf.drawing = 0;
	lcd_draw_buf.pPcfFont = NULL;
	lcd_draw_buf.line_height = 0;
	lcd_draw_buf.align_adjustment = 0;


	if (lcd_draw_buf.screen_buf)
		memset(lcd_draw_buf.screen_buf, 0, LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);
}
void init_file_buffer()
{
        file_buffer = (unsigned char*)Xalloc(FILE_BUFFER_SIZE);
}

// default font is indexed 0, font id 0 is indexed 1, etc. 
char* FontFile(int idx) {
	switch(idx)
	{
		case ITALIC_FONT_IDX - 1:
			return FONT_FILE_ITALIC;
			break;
		case BOLD_ITALIC_FONT_IDX - 1:
			return FONT_FILE_BOLD_ITALIC;
			break;
		case BOLD_FONT_IDX - 1:
			return FONT_FILE_BOLD;
			break;
		case DEFAULT_ALL_FONT_IDX - 1:
			return FONT_FILE_DEFAULT_ALL;
			break;
		case BOLD_ALL_FONT_IDX - 1:
			return FONT_FILE_BOLD_ALL;
			break;
//		case BIG_BOLD_ITALIC_FONT_IDX - 1:
//			return FONT_FILE_BIG_BOLD_ITALIC;
//			break;
//		case BIG_BOLD_FONT_IDX - 1:
//			return FONT_FILE_BIG_BOLD;
//			break;
		case TITLE_FONT_IDX - 1:
			return FONT_FILE_TITLE;
			break;
		case SUBTITLE_FONT_IDX - 1:
			return FONT_FILE_SUBTITLE;
			break;
//		case KEY_FONT_IDX - 1:
//			return FONT_FILE_KEY;
//			break;
//		case SEARCH_HEADING_FONT_IDX - 1:
//			return FONT_FILE_SEARCH_HEADING;
//			break;
//		case SEARCH_LIST_FONT_IDX - 1:
//			return FONT_FILE_SEARCH_LIST;
//			break;
//		case MESSAGE_FONT_IDX - 1:
//			return FONT_FILE_MESSAGE;
//			break;
		case SMALL_FONT_IDX - 1:
			return FONT_FILE_SMALL;
			break;
		default:
			return FONT_FILE_DEFAULT;
			break;
	}
}

ucs4_t UTF8_to_UCS4(unsigned char **pUTF8)
{
	ucs4_t c0, c1, c2, c3;

	/* if 0 returned, it is not a invalid UTF8 character.  The pointer moves to the second byte. */
	c0 = 0;
	if (**pUTF8)
	{
		c0 = (ucs4_t)**pUTF8;
		(*pUTF8)++;
		if (c0 & 0x80) /* multi-byte UTF8 char */
		{
			if ((c0 & 0xE0) == 0xC0) /* 2-byte UTF8 */
			{
				c1 = **pUTF8;
				if ((c1 & 0xC0) == 0x80)
				{
					(*pUTF8)++;
					c0 = ((c0 & 0x1F) << 6) + (c1 & 0x3F);
				}
				else
					c0 = 0; /* invalid UTF8 character */
			} 
			else if ((c0 & 0xF0) == 0xE0) /* 3-byte UTF8 */
			{
				c1 = **pUTF8;
				c2 = *(*pUTF8 + 1);
				if ((c1 & 0xC0) == 0x80 && (c2 & 0xC0) == 0x80)
				{
					(*pUTF8) += 2;
					c0 = ((c0 & 0x0F) << 12) + ((c1 & 0x3F) << 6) + (c2 & 0x3F);
				}
				else
					c0 = 0; /* invalid UTF8 character */
			} 
			else if ((c0 & 0xF1) == 0xF0) /* 4-byte UTF8 */
			{
				c1 = **pUTF8;
				c2 = *(*pUTF8 + 1);
				c3 = *(*pUTF8 + 2);
				if ((c1 & 0xC0) == 0x80 && (c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80)
				{
					(*pUTF8) += 3;
					c0 = ((c0 & 0x07) << 18) + ((c1 & 0x3F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F) ;
				}
				else
					c0 = 0; /* invalid UTF8 character */
			}
			else
					c0 = 0; /* invalid UTF8 character */
		}
	}
	return c0;
}

void UCS4_to_UTF8(ucs4_t u, unsigned char *sUTF8)
{
	if (u < 0x80)
	{
		sUTF8[0] = (unsigned char)u;
		sUTF8[1] = '\0';
	}
	else if (u < 0x800)
	{
		sUTF8[0] = (unsigned char)(0xC0 | (u >> 6));
		sUTF8[1] = (unsigned char)(0x80 | (u & 0x3F));
		sUTF8[2] = '\0';
	}	
	else if (u < 0x10000)
	{
		sUTF8[0] = (unsigned char)(0xC0 | (u >> 12));
		sUTF8[1] = (unsigned char)(0x80 | ((u & 0xFFF) >> 6));
		sUTF8[2] = (unsigned char)(0x80 | (u & 0x3F));
		sUTF8[3] = '\0';
	}	
	else if (u < 0x110000)
	{
		sUTF8[0] = (unsigned char)(0xC0 | (u >> 18));
		sUTF8[1] = (unsigned char)(0x80 | ((u & 0x3FFFF) >> 12));
		sUTF8[2] = (unsigned char)(0x80 | ((u & 0xFFF) >> 6));
		sUTF8[3] = (unsigned char)(0x80 | (u & 0x3F));
		sUTF8[4] = '\0';
	}
	else
	{
		sUTF8[0] = '\0';
	}
}

void buf_draw_UTF8_str_in_copy_buffer(char *framebuffer_copy,char **pUTF8,int start_x,int end_x,int start_y,int end_y,int offset_x)
{
	ucs4_t u;
        LCD_DRAW_BUF lcd_draw_buf_external;
        
	lcd_draw_buf_external.current_x = start_x+offset_x;
	lcd_draw_buf_external.current_y = start_y+2;
	lcd_draw_buf_external.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
        lcd_draw_buf_external.screen_buf = (unsigned char*)framebuffer_copy;
	lcd_draw_buf_external.line_height = pcfFonts[DEFAULT_FONT_IDX - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
    
	lcd_draw_buf_external.align_adjustment = 0;
        
	while (**pUTF8 > MAX_ESC_CHAR) 
	{
		if ((u = UTF8_to_UCS4((unsigned char**)pUTF8)))
		{
			buf_draw_char_external(&lcd_draw_buf_external,u,start_x,end_x,start_y,end_y);
		}

	}
}

void buf_draw_UTF8_str(unsigned char **pUTF8)
{
	unsigned char c, c2;
	char c3;
	long v_line_bottom;
	ucs4_t u;
	int font_idx;
	
	c = **pUTF8;
	if (c <= MAX_ESC_CHAR)
	{
		(*pUTF8)++;
		switch(c)
		{
			case ESC_0_SPACE_LINE: /* space line */
				c2 = **pUTF8;
				(*pUTF8)++;
				lcd_draw_buf.current_x = 0;
				lcd_draw_buf.current_y += lcd_draw_buf.line_height;
				lcd_draw_buf.line_height = c2;
				lcd_draw_buf.align_adjustment = 0;
				if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
					lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
				break;
			case ESC_1_NEW_LINE_DEFAULT_FONT: /* new line with default font and line space */
				lcd_draw_buf.current_x = 0;
				lcd_draw_buf.current_y += lcd_draw_buf.line_height;
				lcd_draw_buf.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
				lcd_draw_buf.line_height = pcfFonts[DEFAULT_FONT_IDX - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
				lcd_draw_buf.align_adjustment = 0;
				if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
					lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
				break;
			case ESC_2_NEW_LINE_SAME_FONT: /* new line with previous font and line space */
				lcd_draw_buf.current_x = 0;
				lcd_draw_buf.current_y += lcd_draw_buf.line_height;
				lcd_draw_buf.align_adjustment = 0;
				if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
					lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
				break;
			case ESC_3_NEW_LINE_WITH_FONT: /* new line with specified font and line space */
				c2 = **pUTF8;
				(*pUTF8)++;
				lcd_draw_buf.current_x = 0;
				lcd_draw_buf.current_y += lcd_draw_buf.line_height;
				font_idx = c2 & 0x07;
				if (font_idx > FONT_COUNT)
					font_idx = DEFAULT_FONT_IDX;
				lcd_draw_buf.pPcfFont = &pcfFonts[font_idx - 1];
				lcd_draw_buf.line_height = c2 >> 3;
				lcd_draw_buf.align_adjustment = 0;
				if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
					lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
				break;
			case ESC_4_CHANGE_FONT: /* change font */
				c2 = **pUTF8;
				(*pUTF8)++;
				font_idx = c2 & 0x07;
				if (font_idx > FONT_COUNT)
					font_idx = DEFAULT_FONT_IDX;
				lcd_draw_buf.pPcfFont = &pcfFonts[font_idx - 1];
				lcd_draw_buf.align_adjustment = ((signed char)c2 >> 3);
				if (lcd_draw_buf.current_y + lcd_draw_buf.line_height + lcd_draw_buf.align_adjustment >= LCD_BUF_HEIGHT_PIXELS)
					lcd_draw_buf.align_adjustment = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
				if (lcd_draw_buf.current_y + lcd_draw_buf.align_adjustment < lcd_draw_buf.line_height - 1)
					lcd_draw_buf.align_adjustment = lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
				break;
			case ESC_5_RESET_TO_DEFAULT_FONT: /* reset to the default font */
				lcd_draw_buf.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
				break;
			case ESC_6_RESET_TO_DEFAULT_ALIGN: /* reset to the default vertical alignment */
				lcd_draw_buf.vertical_adjustment = 0;
				break;
			case ESC_7_FORWARD: /* forward */
				c2 = **pUTF8;
				(*pUTF8)++;
				lcd_draw_buf.current_x += c2;
				if (lcd_draw_buf.current_x > LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - lcd_draw_buf.vertical_adjustment)
					lcd_draw_buf.current_x = LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - lcd_draw_buf.vertical_adjustment;
				break;
			case ESC_8_BACKWARD: /* backward */
				c2 = **pUTF8;
				(*pUTF8)++;
				if (lcd_draw_buf.current_x < c2)
					lcd_draw_buf.current_x = 0;
				else
					lcd_draw_buf.current_x -= c2;
				break;
			case ESC_9_ALIGN_ADJUSTMENT: /* vertical alignment adjustment */
				c3 = **pUTF8;
				(*pUTF8)++;
				lcd_draw_buf.vertical_adjustment += c3;
				//if (lcd_draw_buf.current_y + lcd_draw_buf.line_height + lcd_draw_buf.align_adjustment >= LCD_BUF_HEIGHT_PIXELS)
				//	lcd_draw_buf.align_adjustment = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
				//if (lcd_draw_buf.current_y + lcd_draw_buf.align_adjustment < lcd_draw_buf.line_height - 1)
				//	lcd_draw_buf.align_adjustment = lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
				break;
			case ESC_10_HORIZONTAL_LINE: /* drawing horizontal line */
				c2 = **pUTF8;
				(*pUTF8)++;
				if ((long)c2 > lcd_draw_buf.current_x)
					c2 = (unsigned char)lcd_draw_buf.current_x;
				buf_draw_horizontal_line(lcd_draw_buf.current_x - (unsigned long)c2 + LCD_LEFT_MARGIN, 
					lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment);
				break;
			case ESC_11_VERTICAL_LINE: /* drawing vertical line */
				c2 = **pUTF8;
				(*pUTF8)++;
				v_line_bottom = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
				v_line_bottom -= lcd_draw_buf.align_adjustment;
				if (v_line_bottom < 0)
					v_line_bottom = 0;
				if ((long)c2 > v_line_bottom)
					c2 = (unsigned char)v_line_bottom;
				buf_draw_vertical_line(v_line_bottom - (unsigned long)c2, v_line_bottom - 1);
				break;
			case ESC_12_FULL_HORIZONTAL_LINE: /* drawing horizontal line from left-most pixel to right-most pixel */
				lcd_draw_buf.current_x = 0;
				lcd_draw_buf.current_y += lcd_draw_buf.line_height;
				lcd_draw_buf.line_height = 1;
				lcd_draw_buf.align_adjustment = 0;
				buf_draw_horizontal_line(LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment, LCD_BUF_WIDTH_PIXELS);
				break;
			case ESC_13_FULL_VERTICAL_LINE: /* drawing vertical line from top of the line to the bottom */
				lcd_draw_buf.current_x += 1;
				buf_draw_vertical_line(lcd_draw_buf.current_y, lcd_draw_buf.current_y + lcd_draw_buf.line_height - 1);
				lcd_draw_buf.current_x += 2;
				break;
			default:
				break;
		}
	}
	
	while (**pUTF8 > MAX_ESC_CHAR) /* stop at end of string or escape character */
	{
		if ((u = UTF8_to_UCS4(pUTF8)))
		{
			buf_draw_char(u);
                        if(display_first_page==0 && lcd_draw_buf.current_y>LCD_HEIGHT_LINES)
                        {
                           repaint_framebuffer(lcd_draw_buf.screen_buf,0);
                           display_first_page = 1;
                        }
		}
	}
}
void repaint_framebuffer(unsigned char *buf,int pos)
{
#ifndef WIKIPCF
	int framebuffersize;
	framebuffersize = framebuffer_size();
	
	guilib_fb_lock();
	guilib_clear();

	memcpy(framebuffer,buf+pos,framebuffersize);
	guilib_fb_unlock();
        link_article_number_cur = -1;
#endif

}
void buf_draw_horizontal_line(unsigned long start_x, unsigned long end_x)
{
        int i;
	long h_line_y;


	h_line_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
	h_line_y -= lcd_draw_buf.align_adjustment + 1;


        for(i = start_x;i<end_x;i++)
        {
            lcd_set_pixel(lcd_draw_buf.screen_buf,i, h_line_y);
        }

}

void buf_draw_vertical_line(unsigned long start_y, unsigned long end_y)
{
	unsigned long idx_in_byte;
	unsigned char *p;

	if (lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment < LCD_BUF_WIDTH_PIXELS)
	{
		idx_in_byte = 7 - ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment) & 0x07);
		p = lcd_draw_buf.screen_buf + start_y * LCD_BUF_WIDTH_BYTES + ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment)>> 3);
		while (start_y <= end_y)
		{
			*p |= 1 << idx_in_byte;
			start_y++;
			p += LCD_BUF_WIDTH_BYTES;
		}
	}
}

void lcd_set_pixel(unsigned char *membuffer,int x, int y)
{		
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;
	
	
	membuffer[byte] |= (1 << (7 - bit));
}
void lcd_set_framebuffer_pixel(int x, int y)
{		
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;
	
	
	framebuffer[byte] |= (1 << (7 - bit));
}

void lcd_set_framebuffer_byte(char c, int x, int y)
{		
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	
	framebuffer[byte] = c;
}

char lcd_get_framebuffer_byte(int x, int y)
{		
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	
	return framebuffer[byte];
}

void lcd_clear_pixel(unsigned char *membuffer,int x, int y)
{		
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
        unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;
		
	membuffer[byte] &= ~(1 << (7 - bit));
}
void buf_draw_char(ucs4_t u)
{
	bmf_bm_t *bitmap;
        charmetric_bmf Cmetrics;
	int bytes_to_process;
	int x_base;
	int y_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index 
	int j; // bitmap bit index 
	unsigned char *p; // pointer to lcd draw buffer 
        


	if(pres_bmfbm(u, lcd_draw_buf.pPcfFont, &bitmap, &Cmetrics)<0)
        {
          return;
        }
        if(u==32)
        {
	   lcd_draw_buf.current_x += Cmetrics.LSBearing + Cmetrics.width + 1;
           return;
        }
  
 	if (bitmap == NULL)
	    return;

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;
	
	x_base = lcd_draw_buf.current_x + Cmetrics.LSBearing + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment;
	if (x_base + Cmetrics.LSBearing + Cmetrics.width < LCD_BUF_WIDTH_PIXELS)
	{ // only draw the chracter if not exceeding the LCD width
		y_base = lcd_draw_buf.current_y + lcd_draw_buf.align_adjustment;
		x_offset = 0;
		y_offset = lcd_draw_buf.line_height - (lcd_draw_buf.pPcfFont->Fmetrics.descent + Cmetrics.ascent);
	        
		x_bit_idx = x_base & 0x07;
		//p = lcd_draw_buf.screen_buf + (y_base + y_offset) * LCD_BUF_WIDTH_BYTES + (x_base >> 3);
	
		for (i = 0; i < bytes_to_process; i++)
		{
			j = 7;
			while (j >= 0)
			{
				if (x_offset >= Cmetrics.widthBits)
				{
					x_offset = 0;
					y_offset++;
					p = lcd_draw_buf.screen_buf + (y_base + y_offset) * LCD_BUF_WIDTH_BYTES + x_base;
				}
				if (x_offset < Cmetrics.width)
				{
					if (bitmap[i] & (1 << j))
	                                {
					    //*p |= 1 << ((x_base + x_offset) & 0x07);
	                                    lcd_set_pixel(lcd_draw_buf.screen_buf,x_base + x_offset, y_base+y_offset);
	                                }
	                                     
				}
				x_offset++;
				x_bit_idx++;
				if (!(x_bit_idx & 0x07))
				{
					x_bit_idx = 0;
					p++;
				}
				j--;
			}
		}
	}
	lcd_draw_buf.current_x += Cmetrics.LSBearing + Cmetrics.width + 1;
}
int get_external_str_pixel_width(char **pUTF8)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
        int width = 0;
        ucs4_t u;

	while (**pUTF8 > MAX_ESC_CHAR) 
	{
		if ((u = UTF8_to_UCS4((unsigned char**)pUTF8)))
		{
			pres_bmfbm(u, &pcfFonts[DEFAULT_FONT_IDX - 1], &bitmap, &Cmetrics);
			if (bitmap != NULL)
			   width += Cmetrics.width;
		}

	}	
        return width;
}
void buf_draw_char_external(LCD_DRAW_BUF *lcd_draw_buf_external,ucs4_t u,int start_x,int end_x,int start_y,int end_y)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int bytes_to_process;
	int x_base;
	int y_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index 
	int j; // bitmap bit index 
	unsigned char *p; // pointer to lcd draw buffer 

	if(pres_bmfbm(u, lcd_draw_buf_external->pPcfFont, &bitmap, &Cmetrics)<0)
          return;
   	if (bitmap == NULL)
	  return;

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;
	
	x_base = lcd_draw_buf_external->current_x + Cmetrics.LSBearing;
        if((x_base +Cmetrics.width) > end_x)
        {
            lcd_draw_buf_external->current_x = start_x+2;
            x_base = 0;
            lcd_draw_buf_external->current_y+=lcd_draw_buf_external->line_height;
        }
	y_base = lcd_draw_buf_external->current_y + lcd_draw_buf_external->align_adjustment;
	x_offset = 0;
	y_offset = lcd_draw_buf_external->line_height - (lcd_draw_buf_external->pPcfFont->Fmetrics.descent + Cmetrics.ascent);
	x_bit_idx = x_base & 0x07;

	for (i = 0; i < bytes_to_process; i++)
	{
		j = 7;
		while (j >= 0)
		{
			if (x_offset >= Cmetrics.widthBits)
			{
				x_offset = 0;
				y_offset++;
			}
			if (x_offset < Cmetrics.width)
			{
				if (bitmap[i] & (1 << j))
                                {
                                    lcd_set_pixel(lcd_draw_buf_external->screen_buf,x_base + x_offset, y_base+y_offset);
                                }
                                     
			}
			x_offset++;
			x_bit_idx++;
			if (!(x_bit_idx & 0x07))
			{
				x_bit_idx = 0;
				p++;
			}
			j--;
		}
	}
	lcd_draw_buf_external->current_x += Cmetrics.LSBearing + Cmetrics.width + 1;
}

int get_UTF8_char_width(int idxFont, char **pContent, long *lenContent, int *nCharBytes)
{
	ucs4_t u;
	char *pBase;
	charmetric_bmf Cmetrics;
	bmf_bm_t *bitmap;
	
	pBase = *pContent;
	u = UTF8_to_UCS4((unsigned char **)pContent);
	*nCharBytes = *pContent - pBase;
	*lenContent -= *nCharBytes;
	
	pres_bmfbm(u, &pcfFonts[idxFont - 1], &bitmap, &Cmetrics);
   	if (bitmap == NULL)
		return 0;
	else
		return  Cmetrics.LSBearing + Cmetrics.width + 1;
}
void init_render_article()
{

	//if(lcd_draw_buf.current_y>0)
	  //  memset(lcd_draw_buf.screen_buf,0,lcd_draw_buf.current_y*LCD_VRAM_WIDTH_PIXELS/8);
	if (lcd_draw_buf.screen_buf)
		memset(lcd_draw_buf.screen_buf, 0, LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);


	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = 0;
	lcd_draw_buf.drawing = 0;
	lcd_draw_buf.pPcfFont = NULL;
	lcd_draw_buf.line_height = 0;
	lcd_draw_buf.align_adjustment = 0;
	lcd_draw_buf.vertical_adjustment = 0;

	display_first_page = 0;

//	is_rendering = 1;
}

#define LICENSE_TEXT_FONT SMALL_FONT_IDX
#define LICENSE_TEXT_LINE_HEIGHT 12
#define SPACE_BEFORE_LICENSE_TEXT 80
#define APACE_AFTER_LICENSE_TEXT 5
#define LICENSE_TEXT_1 "Text is available under the Creative Commons"
#define LICENSE_TEXT_2 "Attribution-ShareAlike License; additional terms may"
#define LICENSE_TEXT_3 "apply.  See Terms of Use for details.  Wikipedia`"
#define LICENSE_TEXT_4 "is a registered trademark of the Wikimedia Foundation,"
#define LICENSE_TEXT_5 "Inc., a non-profit organization.  2009.8.25"
void draw_license_text(unsigned char *s)
{
	ucs4_t u;
	unsigned char **p = &s;

	while (**p && (u = UTF8_to_UCS4(p)))
	{
		buf_draw_char(u);
	}
}

void render_wikipedia_license_text()
{
#ifndef WIKIPCF
	long start_x, start_y, end_x, end_y;
	
	// if not enough space at the end, then skip
	if (lcd_draw_buf.current_y < LCD_BUF_HEIGHT_PIXELS - SPACE_BEFORE_LICENSE_TEXT - LICENSE_TEXT_LINE_HEIGHT * 6 - APACE_AFTER_LICENSE_TEXT)
	{
		lcd_draw_buf.line_height = LICENSE_TEXT_LINE_HEIGHT;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += SPACE_BEFORE_LICENSE_TEXT;
		lcd_draw_buf.pPcfFont = &pcfFonts[LICENSE_TEXT_FONT - 1];
		
		draw_license_text(LICENSE_TEXT_1);
		start_x = 117;
		end_x = 199;
		buf_draw_horizontal_line(start_x + LCD_LEFT_MARGIN, end_x + LCD_LEFT_MARGIN);
		if (article_link_count < ARTICLE_LINK_COUNT)
		{
			start_y = lcd_draw_buf.current_y + 1;
			end_y = lcd_draw_buf.current_y + 1 + LICENSE_TEXT_LINE_HEIGHT;
			articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));;
			articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));;
			articleLink[article_link_count++].article_id = 1;
		}
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += LICENSE_TEXT_LINE_HEIGHT;
		draw_license_text(LICENSE_TEXT_2);
		start_x = 0;
		end_x = 134;
		buf_draw_horizontal_line(start_x + LCD_LEFT_MARGIN, end_x + LCD_LEFT_MARGIN);
		if (article_link_count < ARTICLE_LINK_COUNT)
		{
			start_y = lcd_draw_buf.current_y + 1;
			end_y = lcd_draw_buf.current_y + 1 + LICENSE_TEXT_LINE_HEIGHT;
			articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));;
			articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));;
			articleLink[article_link_count++].article_id = 1;
		}
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += LICENSE_TEXT_LINE_HEIGHT;
		draw_license_text(LICENSE_TEXT_3);
		start_x = 50;
		end_x = 107;
		buf_draw_horizontal_line(start_x + LCD_LEFT_MARGIN, end_x + LCD_LEFT_MARGIN);
		if (article_link_count < ARTICLE_LINK_COUNT)
		{
			start_y = lcd_draw_buf.current_y + 1;
			end_y = lcd_draw_buf.current_y + 1 + LICENSE_TEXT_LINE_HEIGHT;
			articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));;
			articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));;
			articleLink[article_link_count++].article_id = 2;
		}
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += LICENSE_TEXT_LINE_HEIGHT;
		draw_license_text(LICENSE_TEXT_4);
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += LICENSE_TEXT_LINE_HEIGHT;
		draw_license_text(LICENSE_TEXT_5);
		lcd_draw_buf.current_y += LICENSE_TEXT_LINE_HEIGHT + APACE_AFTER_LICENSE_TEXT;
	}
#endif
}

int render_article_with_pcf()
{

	if (!article_buf_pointer)
		return 0;
	
	buf_draw_UTF8_str(&article_buf_pointer);
        if(stop_render_article == 1 && display_first_page == 1)
        {
           article_buf_pointer = NULL;
           stop_render_article = 0;
           return 0;
        }
	if(request_display_next_page > 0 && lcd_draw_buf.current_y > request_y_pos)
	{
		repaint_framebuffer(lcd_draw_buf.screen_buf,(request_y_pos-LCD_HEIGHT_LINES)*LCD_VRAM_WIDTH_PIXELS/8);
		request_display_next_page = 0;
	}
        if(!*article_buf_pointer)
        {
//            is_rendering = 0;
            render_wikipedia_license_text();
            if(display_first_page == 0)
                repaint_framebuffer(lcd_draw_buf.screen_buf,0);

	    article_buf_pointer = NULL;
	    lcd_draw_buf_size = lcd_draw_buf.current_y * LCD_VRAM_WIDTH_PIXELS;
	    lcd_draw_buf_pos  = 0;
	    lcd_draw_cur_y_pos = 0;

            return 0;
        }
        return 1;

}
void display_article_with_pcf(int start_y)
{
	int pos,lcd_draw_cur_y_pos_last;

        is_display_external_link = 0;

        lcd_draw_cur_y_pos_last = lcd_draw_cur_y_pos;
        //sprintf(msg_out,"lcd_draw_cur_y_pos:%d,start_y:%d,lcd_draw_buf.current_y:%ld\n",lcd_draw_cur_y_pos,start_y,lcd_draw_buf.current_y);
        //msg_info(msg_out);
        if(lcd_draw_buf.current_y<=LCD_HEIGHT_LINES)
            return;
        
//        if(is_rendering && (lcd_draw_cur_y_pos+start_y+LCD_HEIGHT_LINES) > lcd_draw_buf.current_y)
        if(article_buf_pointer && (lcd_draw_cur_y_pos+start_y+LCD_HEIGHT_LINES) > lcd_draw_buf.current_y)
        {
            request_display_next_page = 1;
            request_y_pos = lcd_draw_cur_y_pos + start_y + LCD_HEIGHT_LINES;

            display_str("Please wait...");

            return;
        }
        if(lcd_draw_cur_y_pos == 0 && start_y<0)
        {
           return;
        }

	lcd_draw_cur_y_pos += start_y;

        if(lcd_draw_cur_y_pos < 0)
           lcd_draw_cur_y_pos = 0;
        else if((lcd_draw_cur_y_pos+LCD_HEIGHT_LINES)>lcd_draw_buf.current_y)
           lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
	
	pos = (lcd_draw_cur_y_pos*LCD_VRAM_WIDTH_PIXELS)/8;

        //sprintf(msg_out,"current_display_y_pos:%d,sroll to:%d,article height:%d\n",(int)lcd_draw_cur_y_pos_last,(int)lcd_draw_cur_y_pos,(int)lcd_draw_buf.current_y);
        //msg_info(msg_out);
        
        repaint_framebuffer(lcd_draw_buf.screen_buf,pos);
	
	lcd_draw_buf_pos = pos;

}
void display_article_with_pcf_smooth(int start_y)
{
	int pos,lcd_draw_cur_y_pos_last;
        int time,add,delay_time,distance;

        is_display_external_link = 0;

        lcd_draw_cur_y_pos_last = lcd_draw_cur_y_pos;

        if(lcd_draw_buf.current_y<=LCD_HEIGHT_LINES)
            return;

        if(article_buf_pointer && (lcd_draw_cur_y_pos+start_y+LCD_HEIGHT_LINES) > lcd_draw_buf.current_y)
        {
            request_display_next_page = 1;
            request_y_pos = lcd_draw_cur_y_pos + start_y + LCD_HEIGHT_LINES;

            display_str("Please wait...");

            return;
        }
        if(lcd_draw_cur_y_pos == 0 && start_y<0)
        {
           return;
        }
        
        int offset = abs(start_y);
        int increment;
        distance = offset;
        article_distance = offset;
        article_offset = offset;
        if(start_y>0)
        {
           increment = 2;
           article_scroll_increment = 2;
        }
        else
        {
           increment = -2;
           article_scroll_increment = -2;
        }
        time = LCD_UPDATE_MILLISEC;
        article_scroll_delay_time = time;
        add = 100;
        base_delay_time = add;
        pos = 0;
        delay_time = 0;
        article_scroll_delay_time_total = 0;
        /*while(offset--){
		lcd_draw_cur_y_pos += increment;
	
		if(lcd_draw_cur_y_pos < 0)
		lcd_draw_cur_y_pos = 0;
		else if((lcd_draw_cur_y_pos+LCD_HEIGHT_LINES)>lcd_draw_buf.current_y)
		lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES+20;
		
		pos = (lcd_draw_cur_y_pos*LCD_VRAM_WIDTH_PIXELS)/8;
	
		
		repaint_framebuffer(lcd_draw_buf.screen_buf,pos);

                #ifdef INCLUDED_FROM_KERNEL
                delay_us(time);
                if(offset<(distance/2))
                   delay_time = add*4 ;
                else if(offset<(distance/4))
                   delay_time = add*8 ;
                else if(offset<(distance/8))
                   delay_time = add*16 ;
                else
                   delay_time = add;
                time+=delay_time;
                #endif
		
		lcd_draw_buf_pos = pos;
        }*/

}
void scroll_article()
{
        int delay_time,pos;
        if(article_offset<=0)
          return;

	if (!display_first_page)
		return;
        delay_time = 0;
        /*int time_now = get_time_tick();
        if((time_now-time_scroll_article_last)<article_scroll_delay_time*24)
          return;
        time_scroll_article_last = (int)time_now;
        article_scroll_delay_time_total+=article_scroll_delay_time;
        article_scroll_delay_time+=base_delay_time;
        */
        if(article_scroll_delay_time_total>=1000000*3)
            article_offset = 0;

	lcd_draw_cur_y_pos += article_scroll_increment;

	if(lcd_draw_cur_y_pos < 0)
	   lcd_draw_cur_y_pos = 0;
	else if (lcd_draw_cur_y_pos > lcd_draw_buf.current_y)
	   lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
	
	pos = (lcd_draw_cur_y_pos*LCD_VRAM_WIDTH_PIXELS)/8;

	
	repaint_framebuffer(lcd_draw_buf.screen_buf,pos);

	#ifdef INCLUDED_FROM_KERNEL
	delay_us(article_scroll_delay_time);
        article_scroll_delay_time_total+=article_scroll_delay_time;
	//if(article_offset<(article_distance/8))
	  // delay_time = base_delay_time*3 ;
	//else if(article_offset<(article_distance/8))
	  // delay_time = article_scroll_delay_time*8 ;
	//else if(article_offset<(article_distance/16))
	   //delay_time = article_scroll_delay_time*16 ;
	//else
	   delay_time = base_delay_time;
	article_scroll_delay_time+=delay_time;
	#endif
	
	lcd_draw_buf_pos = pos;
        article_offset--;
}
unsigned char * open_article_with_pcf_link(long idx_article)
{
        int i;
	int offset,start_x,start_y,end_x,end_y;
	ARTICLE_HEADER article_header;
		
        file_buffer[0] = '\0';
		
	if (retrieve_article(idx_article))
        {
	    return NULL; // article not exist		
        }

	memcpy(&article_header,file_buffer,sizeof(ARTICLE_HEADER));
	offset = sizeof(ARTICLE_HEADER);
		
	if(article_header.article_link_count>ARTICLE_LINK_COUNT)
	    article_header.article_link_count = ARTICLE_LINK_COUNT;
	if(article_header.external_link_count>EXTERNAL_LINK_COUNT)
	    article_header.external_link_count = EXTERNAL_LINK_COUNT;

	for(i = 0; i< article_header.article_link_count;i++)
	{
	    memcpy(&articleLink[i],file_buffer+offset,sizeof(ARTICLE_LINK));
            offset+=sizeof(ARTICLE_LINK);

            start_y = articleLink[i].start_xy >>8;
	    start_x = articleLink[i].start_xy & 0x000000ff;
	    end_y   = articleLink[i].end_xy  >>8;
	    end_x   = articleLink[i].end_xy & 0x000000ff;
//#ifndef WIKIPCF
//msg(MSG_INFO, " %x, %x, (%d, %d) (%d, %d) %ld\n", articleLink[i].start_xy, articleLink[i].end_xy, start_x, start_y, end_x, end_y, articleLink[i].article_id);
//#endif          

	}
        article_link_count = article_header.article_link_count;
    
        for(i = 0; i< article_header.external_link_count;i++)
        {
            memcpy(&externalLink[i],file_buffer+offset,sizeof(EXTERNAL_LINK));
        
        /*start_y = externalLink[i].start_xy >>8;
	start_x = externalLink[i].start_xy & 0x000000ff;
	end_y   = externalLink[i].end_xy  >>8;
	end_x   = externalLink[i].end_xy & 0x000000ff;

        msg(MSG_INFO,"external link:%d start_x:%d,start_y:%d,end_x:%d,end_y:%d\n",i,start_x,start_y,end_x,end_y);        
        msg(MSG_INFO,"external link str offset:%d,len:%d\n",externalLink[i].offset_link_str,externalLink[i].link_str_len);
        offset_str = sizeof(ARTICLE_HEADER)+article_header.article_link_count*sizeof(ARTICLE_LINK)+ article_header.external_link_count*sizeof(EXTERNAL_LINK)+externalLink[i].offset_link_str;
        //offset_str = offset+sizeof(EXTERNAL_LINK)+externalLink[i].offset_link_str;
        memcpy(external_link_str,file_buffer+offset_str,externalLink[i].link_str_len);
        external_link_str[externalLink[i].link_str_len]='\0';
        msg(MSG_INFO,"external link str absolute offset:%d,str:%s\n",offset_str,external_link_str);*/
       
        offset+=sizeof(EXTERNAL_LINK);

    }		
    external_link_count = article_header.external_link_count;
    
    return file_buffer+article_header.offset_article;
		
}

int isArticleLinkSelected(int x,int y)
{
	  int i;
	  int article_link_start_y_pos;
	  int article_link_start_x_pos;
	  int article_link_end_y_pos;
	  int article_link_end_x_pos;
	  //char msg[1024];

          //sprintf(msg,"article link count:%d,x:%d,y:%d\n",article_link_count,x,y);
          //msg_info(msg);
	  for(i = 0 ; i < article_link_count; i++)
	  {
	  	article_link_start_y_pos = articleLink[i].start_xy >>8;
	  	article_link_start_x_pos = (articleLink[i].start_xy & 0x000000ff) + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment;
	  	article_link_end_y_pos = articleLink[i].end_xy >>8;
	  	article_link_end_x_pos = (articleLink[i].end_xy & 0x000000ff) + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment;
            
                //sprintf(msg,"article link:%d,start_x:%d,end_x:%d,start_y:%d,end_y:%d\n",i,article_link_start_x_pos,article_link_end_x_pos,article_link_start_y_pos,article_link_end_y_pos);
                //msg_info(msg);
	  	
	  	//if((lcd_draw_cur_y_pos+y)>=article_link_start_y_pos && (lcd_draw_cur_y_pos+y)<=article_link_end_y_pos && x>=article_link_start_x_pos && x<=article_link_end_x_pos)
	  	if((lcd_draw_cur_y_pos+y)>=(article_link_start_y_pos-5) && (lcd_draw_cur_y_pos+y)<=(article_link_end_y_pos+5) && x>=(article_link_start_x_pos-10) && x<=(article_link_end_x_pos+10))
	  		return i;
	  		  	
	  }
	  return -1;
}
int isExternalLinkSelected(int x,int y)
{
	  int i;
	  int link_start_y_pos;
	  int link_start_x_pos;
	  int link_end_y_pos;
	  int link_end_x_pos;
	  
	  for(i = 0 ; i < external_link_count; i++)
	  {
	  	link_start_y_pos = externalLink[i].start_xy >>8;
	  	link_start_x_pos = externalLink[i].start_xy & 0x000000ff;
	  	link_end_y_pos = externalLink[i].end_xy >>8;
	  	link_end_x_pos = externalLink[i].end_xy & 0x000000ff;
	  	
	  	if((lcd_draw_cur_y_pos+y)>=link_start_y_pos && (lcd_draw_cur_y_pos+y)<=link_end_y_pos && x>=link_start_x_pos && x<=link_end_x_pos)
	  		return i;
	  		  	
	  }
	  return -1;
}

#ifndef WIKIPCF
#ifndef INCLUDED_FROM_KERNEL
int load_init_article(long idx_init_article)
{
	int fd;
	char file[128];
	long len;
        int i;
	int offset,start_x,start_y,end_x,end_y;
	ARTICLE_HEADER article_header;
	
	if (idx_init_article > 0)
	{
		sprintf(file, "./dat/%ld/%ld/%ld/%ld", (idx_init_article / 1000000), (idx_init_article / 10000) % 100, (idx_init_article / 100) % 100, idx_init_article);
		fd = wl_open(file, WL_O_RDONLY);
		if (fd >= 0)
		{
			len = wl_read(fd, (void *)file_buffer, FILE_BUFFER_SIZE - 1);
			file_buffer[len] = '\0';
			wl_close(fd);
		}
		
		memcpy(&article_header,file_buffer,sizeof(ARTICLE_HEADER));
		offset = sizeof(ARTICLE_HEADER);
			
		if(article_header.article_link_count>ARTICLE_LINK_COUNT)
		    article_header.article_link_count = ARTICLE_LINK_COUNT;
		if(article_header.external_link_count>EXTERNAL_LINK_COUNT)
		    article_header.external_link_count = EXTERNAL_LINK_COUNT;
	
		for(i = 0; i< article_header.article_link_count;i++)
		{
		    memcpy(&articleLink[i],file_buffer+offset,sizeof(ARTICLE_LINK));
	            offset+=sizeof(ARTICLE_LINK);
	
	            start_y = articleLink[i].start_xy >>8;
		    start_x = articleLink[i].start_xy & 0x000000ff;
		    end_y   = articleLink[i].end_xy  >>8;
		    end_x   = articleLink[i].end_xy & 0x000000ff;
	
		}
	        article_link_count = article_header.article_link_count;
	    
	        for(i = 0; i< article_header.external_link_count;i++)
	        {
	            memcpy(&externalLink[i],file_buffer+offset,sizeof(EXTERNAL_LINK));
	            offset+=sizeof(EXTERNAL_LINK);
		}		
		external_link_count = article_header.external_link_count;
	    
		article_buf_pointer = file_buffer+article_header.offset_article;
		init_render_article();
		return 0;
	}
	return -1;
}
#endif
#endif

int display_link_article(long article_link_number)
{
        init_render_article();
	article_buf_pointer = open_article_with_pcf_link(article_link_number);
	if(article_buf_pointer!=NULL)
	{
		init_render_article();
		//render_article_with_pcf(&article_buf_pointer);
		return 0;
	}
	return -1;
}
void get_external_link_str(char *str,int external_link_number)
{
       int offset_str,len;

       offset_str = sizeof(ARTICLE_HEADER)+article_link_count*sizeof(ARTICLE_LINK)+ external_link_count*sizeof(EXTERNAL_LINK)+externalLink[external_link_number].offset_link_str;
       
       if(externalLink[external_link_number].link_str_len>=256)
          len = 255; 
       else
          len = externalLink[external_link_number].link_str_len;

       memcpy(str,file_buffer+offset_str,len);
       str[len]='\0';

}

void display_external_link_str(int external_link_number,char *external_link_str)
{
#ifndef WIKIPCF

       int framebuffersize,start_x,end_x,start_y,end_y;
       int offset_x,offset_y;
       int external_str_width;
       char *p;

       p = external_link_str;

       get_external_link_str(external_link_str,external_link_number);

       framebuffersize = framebuffer_size();
       memcpy(framebuffer_copy,framebuffer,framebuffersize);

       start_x = framebuffer_height()/2-100;
       end_x   = framebuffer_height()/2+100;
       start_y = framebuffer_height()/2-50;
       end_y   = framebuffer_height()/2+50;

       drawline_in_framebuffer_copy(framebuffer_copy,start_x,start_y,end_x,end_y);

       external_str_width = get_external_str_pixel_width(&p);
       offset_x = 500 - external_str_width/2;
       offset_y = div_wiki(offset_x,200);
       offset_x = offset_x - offset_y*200;
       start_y = start_y+15*offset_y;

       buf_draw_UTF8_str_in_copy_buffer(framebuffer_copy,&external_link_str,start_x,end_x,start_y,end_y,offset_x);

       repaint_framebuffer(framebuffer_copy,0);
#endif
 }
void display_str(char *str)
{
       int framebuffersize,start_x,end_x,start_y,end_y;
       int offset_x,offset_y;
       int str_width;
       char *p;

       p = str;

       framebuffersize = framebuffer_size();
       memset(framebuffer_copy,0,framebuffersize);

       start_x = 0;
       end_x   = framebuffer_width();
       start_y = framebuffer_height()/2-9;
       end_y   = framebuffer_height()/2+9;

//       drawline_in_framebuffer_copy(framebuffer_copy,start_x,start_y,end_x,end_y);

       str_width = get_external_str_pixel_width(&p);
       offset_x = (end_x - str_width) / 2 - start_x;
       offset_y = 0;
       buf_draw_UTF8_str_in_copy_buffer((char *)framebuffer_copy,&str,start_x,end_x,start_y,end_y,offset_x);

       repaint_framebuffer(framebuffer_copy,0);
       
 }
int div_wiki(int a,int b)
{
    int c =0,m = 0;

    if(a<b)
       return 0;

    for(;;)
    {
       c += b;
       if(c >= a)
           return m;
       m++;
    }
}
void drawline_in_framebuffer_copy(unsigned char *buffer,int start_x,int start_y,int end_x,int end_y)
{
      int i,m;
      for(i = start_y;i<end_y;i++)
         for(m = start_x;m<end_x;m++)
           lcd_clear_pixel(buffer,m,i);

      for(i = start_x ; i < end_x ; i++)
            lcd_set_pixel(buffer,i, start_y);

      for(i = start_y ; i < end_y ; i++)
            lcd_set_pixel(buffer,start_x,i);

      for(i = start_x ; i < end_x ; i++)
            lcd_set_pixel(buffer,i,end_y);

      for(i = start_y ; i < end_y ; i++)
            lcd_set_pixel(buffer,end_x,i);
}
void invert_link(int article_link_number)
{
#ifndef WIKIPCF
     int start_x,start_y,end_x,end_y;

     if(article_link_number<0)
        return;

     start_y = articleLink[article_link_number].start_xy >>8;
     start_x = (articleLink[article_link_number].start_xy & 0x000000ff) + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment - 1;
     if (start_x < 0)
     	start_x = 0;
     end_y   = articleLink[article_link_number].end_xy  >>8;
     end_x   = (articleLink[article_link_number].end_xy & 0x000000ff) + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment;
     
     guilib_fb_lock();
     guilib_invert_area(start_x, start_y - lcd_draw_cur_y_pos, end_x, end_y - lcd_draw_cur_y_pos);
     guilib_fb_unlock();
#endif
}
void open_article_link(int x,int y)
{
     int article_link_number,external_link_number;
     char str[256];

      article_link_number = isArticleLinkSelected(x,y);
      if(article_link_number >= 0)
      {
         display_link_article(articleLink[article_link_number].article_id);
      }
      else
      {
          external_link_number = isExternalLinkSelected(x,y);
          if(external_link_number >= 0)
          {
             display_external_link_str(external_link_number,str);
             is_display_external_link = 1;
          }
          else if(is_display_external_link)//restore content  from lcd_draw_buf
          {
             repaint_framebuffer(lcd_draw_buf.screen_buf,lcd_draw_buf_pos);
             is_display_external_link = 0;

          }
      }
      
}
void open_article_link_with_link_number(int article_link_number)
{
#ifndef WIKIPCF
 	char title[MAX_TITLE_SEARCH];
 	long idx;
 	
	if (article_link_number < 0 || articleLink[article_link_number].article_id <= 0)
		return;
	display_first_page = 0; // use this to disable scrolling until the first page of the linked article is loaded
	idx = articleLink[article_link_number].article_id;
	if (!display_link_article(idx))
	{
		get_article_title_from_idx(idx, title);
		history_add(idx, title);
	}
#endif
}
void msg_info(char *data)
{
#ifdef WIKIPCF
   printf(data);
#else
   msg(MSG_INFO,data);
#endif
}
int framebuffer_size()
{
#ifdef WIKIPCF
    return 0;
#else
    return guilib_framebuffer_size();
#endif
}
int framebuffer_width()
{
#ifdef WIKIPCF
    return 0;
#else
    return guilib_framebuffer_width();
#endif

}
int framebuffer_height()
{
#ifdef WIKIPCF
    return 0;
#else
    return guilib_framebuffer_height();
#endif

}

int strchr_idx(char *s, char c)
{
	int rc = -1;
	int i = 0;
	
	while (rc < 0 && s[i])
	{
		if (s[i] == c)
			rc = i;
		i++;
	}
	return rc;
}

void get_show_key_xy(char c, int *x, int *y)
{
	int idx;
	
	if ((idx = strchr_idx("QWERTYUIOP", c)) >= 0)
	{
		*y = 86;
		if (idx == 0)
			*x = 0;
		else if (idx == 9)
			*x = 208;
		else
			*x = idx * 24 - 6;
	}
	else if ((idx = strchr_idx("1234567890", c)) >= 0)
	{
		*y = 86;
		if (idx == 0)
			*x = 0;
		else if (idx == 9)
			*x = 208;
		else
			*x = idx * 24 - 6;
	}
	else if ((idx = strchr_idx("ASDFGHJKL", c)) >= 0)
	{
		*y = 110;
		if (idx == 0)
			*x = 0;
		else
			*x = idx * 24 - 6;
	}
	else if ((idx = strchr_idx("*$%#()-+=", c)) >= 0)
	{
		*y = 110;
		if (idx == 0)
			*x = 0;
		else
			*x = idx * 24 - 6;
	}
	else if ((idx = strchr_idx("ZXCV BNM", c)) >= 0)
	{
		*y = 134;
		if (idx == 0)
			*x = 0;
		else if (idx < 4)
			*x = idx * 24 - 6;
		else if (idx == 4)
			*x = 4 * 24 + 8;
		else
			*x = (idx + 1) * 24 - 6;
	}
	else if ((idx = strchr_idx("@?!& ,.'", c)) >= 0)
	{
		*y = 134;
		if (idx == 0)
			*x = 0;
		else if (idx < 4)
			*x = idx * 24 - 6;
		else if (idx == 4)
			*x = 4 * 24 + 8;
		else
			*x = (idx + 1) * 24 - 6;
	}
}

void show_key(char c)
{/*
	static int bKeyShowed = 0;
	static char local_lcd_buf[30][5];
	char c2;
	int i, j, k;
	static int x = 0;
	static int y = 0;
	int x_remainder;
	int width;
	long len;
	char *p;
	int nCharBytes;
		
	if (!c || c < ' ' || c > '~')
	{
		if (bKeyShowed)
		{
			x_remainder = x % 8;
			bKeyShowed = 0;
			for (i=0; i < 30; i++)
			{
				for (j = 0; j < 5; j++)
				{
					if (x_remainder)
						lcd_set_framebuffer_byte(local_lcd_buf[i][j], x -  x_remainder + j * 8, y + i);
					else if (0 <= j && j < 4)
						lcd_set_framebuffer_byte(local_lcd_buf[i][j], x + (j - 1) * 8, y + i);
				}
			}
		}
	}
	else
	{
		if ('a' <= c && c <= 'z')
			c = 'A' + (c - 'a');
		get_show_key_xy(c, &x, &y);
		bKeyShowed = 1;
		x_remainder = x % 8;
		for (i=0; i < 30; i++)
		{
			for (j = 0; j < 5; j++)
			{
				if (x_remainder)
					local_lcd_buf[i][j] = lcd_get_framebuffer_byte(x -  x_remainder + j * 8, y + i);
				else if (0 <= j && j < 4)
					local_lcd_buf[i][j] = lcd_get_framebuffer_byte(x + j * 8, y + i);
			}
		}
		for (i=0; i < 30; i++)
		{
			for (j = 0; j < 5; j++)
			{
				if (i == 1 || i == 28)
				{
					if (x_remainder)
					{
						if (j == 0)
						{
							c2 = local_lcd_buf[i][j];
							c2 &= 0xFE << (8 - x_remainder);
							if (1 <= x_remainder && x_remainder <= 6)
							{
								for (k = 0; k <= (8 - x_remainder - 2); k++)
									c2 |= 0x01 << k;
							}
						}
						else if (j == 4)
						{
							c2 = local_lcd_buf[i][j];
							c2 &= 0xEF >> x_remainder;
							if (1 < i && i < 28 && 2 <= x_remainder)
							{
								for (k = 0; k <= (x_remainder - 2); k++)
									c2 |= 0x80 >> k;
							}
						}
						else
							c2 = 0xFF;
						lcd_set_framebuffer_byte(c2, x -  x_remainder + j * 8, y + i);
					}
					else if (0 <= j && j < 4)
					{
						if (j == 0)
							lcd_set_framebuffer_byte(0x7F, x + j * 8, y + i);
						else if (j == 3)
							lcd_set_framebuffer_byte(0xFE, x + j * 8, y + i);
						else
							lcd_set_framebuffer_byte(0xFF, x + j * 8, y + i);
					}
				}
				else
				{
					if (x_remainder)
					{
						if (j == 0)
						{
							c2 = local_lcd_buf[i][j];
							c2 &= 0xFE << (8 - x_remainder);
							if (1 < i && i < 28 && 1 <= x_remainder && x_remainder <= 6)
								c2 |= 0x01 << (8 - x_remainder - 2);
						}
						else if (j == 4)
						{
							c2 = local_lcd_buf[i][j];
							c2 &= 0xEF >> x_remainder;
							if (1 < i && i < 28 && 2 <= x_remainder)
								c2 |= 0x80 >> (x_remainder - 2);
						}
						else
						{
							c2 = 0;
							if (1 < i && i < 28)
							{
								if (j == 1 && x_remainder > 6)
									c2 |= 0x80;
								else if (j == 4 && x_remainder < 2)
									c2 |= 0x01;
							}
						}
						lcd_set_framebuffer_byte(c2, x -  x_remainder + j * 8, y + i);
					}
					else if (0 <= j && j < 4)
					{
						if (1 < i && i < 28 && j == 0)
							lcd_set_framebuffer_byte(0x40, x + j * 8, y + i);
						else if (1 < i && i < 28 && j == 3)
							lcd_set_framebuffer_byte(0x02, x + j * 8, y + i);
						else
							lcd_set_framebuffer_byte(0, x + j * 8, y + i);
					}
				}
			}
		}
		p = &c;
		len = 1;
		width = get_UTF8_char_width(SEARCH_HEADING_FONT_IDX, &p, &len, &nCharBytes);
		draw_bmf_char((ucs4_t)c, SEARCH_HEADING_FONT_IDX - 1, x + (32 - width) / 2, y + 4);
	}
*/}

int draw_bmf_char(ucs4_t u,int font,int x,int y)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	//pcf_SCcharmet_t sm;
	int bytes_to_process;
	int x_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index 
	int j; // bitmap bit index 
	pcffont_bmf_t *pPcfFont;

	pPcfFont = &pcfFonts[font];
	
	pres_bmfbm(u, pPcfFont, &bitmap, &Cmetrics);
   	if (bitmap == NULL)
        {
            return -1;
        }

        //sprintf(msg,"char:%d,width:%d,LSBearing:%d,RSBearding:%d,widthBytes:%d,height:%d\n",u,Cmetrics.width,Cmetrics.LSBearing,Cmetrics.RSBearing,Cmetrics.widthBytes,Cmetrics.height);
        //msg_info(msg);

        if(u==32)
        {
	   x += Cmetrics.LSBearing + Cmetrics.width + 1;
           return x;
        }
	
	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;
	
	x_base = x + Cmetrics.LSBearing;
	x_offset = 0;
	y_offset = pPcfFont->Fmetrics.linespace - (pPcfFont->Fmetrics.descent + Cmetrics.ascent);

	
	x_bit_idx = x_base & 0x07;
	
	if (x + Cmetrics.LSBearing + Cmetrics.width >= LCD_BUF_WIDTH_PIXELS)
		return -1;

	for (i = 0; i < bytes_to_process; i++)
	{
		j = 7;
		while (j >= 0)
		{
			if (x_offset >= Cmetrics.widthBits)
			{
				x_offset = 0;
				y_offset++;
			}
			if (x_offset < Cmetrics.width)
			{
				if (bitmap[i] & (1 << j))
				{
					lcd_set_framebuffer_pixel(x_base + x_offset, y+y_offset);
				}
				
			}
			x_offset++;
			x_bit_idx++;
			if (!(x_bit_idx & 0x07))
			{
				x_bit_idx = 0;
				
			}
			j--;
		}
	}
	x += Cmetrics.LSBearing + Cmetrics.width + 1;
	return x;
}

int GetFontLinespace(int font)
{
   return pcfFonts[font+1].Fmetrics.linespace;
}
void set_article_link_number(int num)
{
    link_article_number_cur = num;
}
int get_article_link_number()
{
    return link_article_number_cur;
}
int get_time_tick(void)
{
        int clock_ticks;

#ifdef INCLUDED_FROM_KERNEL
	clock_ticks = Tick_get();
#else
	clock_ticks = clock();
#endif

	return clock_ticks;
}

void msg_on_lcd(int x, int y, char *msg)
{
#ifdef INCLUDED_FROM_KERNEL
//	guilib_fb_lock();
	guilib_clear_area(x, y, 239, 18);
	render_string(DEFAULT_FONT_IDX, x, y, msg, strlen(msg)); 
//	guilib_fb_unlock();
#endif
}

void msg_on_lcd_clear(int x, int y)
{
#ifdef INCLUDED_FROM_KERNEL
//	guilib_fb_lock();
	guilib_clear_area(x, y, 239, y+18);
//	guilib_fb_unlock();
#endif
}

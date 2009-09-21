/*
Assumptions:

1. Support up to 9 fonts - the default font and font 0 to font 7.  For example, the normal 12-pixel font is used as the default font. 

2. There is a default line space (e.g. 16 pixels).

3. The characters in the line are horizontally aligned to the bottom of the line by default.
   Most characters got 2 blank pixel space at the bottom with some exceptions.  For example, "_" only got one blank pixel line and 
   "gjpqy" got no blank pixel space.  A 12-pixel character actually takes 14 pixels from top to the bottom on Windows and I think it's a good practice.
   To save the storage, we can still bitmap them in 12 pixels, but the font API should be smart enough to return 14 pixels by padding with 
   blank pixels at the top or the bottom.

4. Each character in the line is adjacent to the previous character without space.
   The font API should pad one blank pixel space at the right of the character.
   (In some Chinese fonts, the blank pixel space is at the left of the character and some characters leave no blank space at all.
   Since we are dealing with English wiki, it is less important.)

5. If characters overlap, the glyphs of all characters retain.
   (For example, space does not overwrite the overlapped character since it got no glyphs.)

6. New line is adjacent to the previous line.

7. The article content is coded in UTF-8.  Some UTF-8 control characters are used as escape characters.

0 - escape character 0 (space line), the byte after it contains the pixels of the space line

1 - escape character 1 (new line with the default font and the default line space)

2 - escape character 2 (new line with the same font and line space as the previous line, not considering the font changes and adjustments)

3 - escape character 3 (new line with non-default font and line space)
        Definition of the byte after it:
                bit0~2 - font id (1 ~ 7)
                bit3~7 - pixels of the line space

4 - escape character 4 (change font)
        The font change (including alignment adjustment) is reset after the escape character 1, 3, 4 or 5.
        Definition of the byte after it:
                bit0~2 - font id (1 ~ 7)
                bit3~7 - adjustment to the default horizontal alignment (0: 0; 0x01~0x0F: 1~15; 0x10~0x1F: -16~-1)

5 - escape character 5 (reset to the default font) 

6 - escape character 6 (reset to the default vertical alignment)

7 - escape character 7 (forward), the byte after it stands for the pixels of move right (as the starting point of the next character).
    The next character or drawing line starts at the position after the movement.
    The rightmost position after the movement is the rightmost pixel of the screen.

8 - escape character 8 (backward), the byte after it stands for the pixels to move left (from the original starting point of the next character).
    The next character or line starts at the position after the movement. 
    The leftmost position after the movement is the leftmost pixel of the screen.

9 - escape character 9 (alignment vertical adjustment), the byte after it stand for the pixels to adjust to the default vertical alignment (0x01~0x7F: 1~127; 0x80~0x8F: -128~-1)
    It takes effect for all lines after that until another excape character 9 or escape character 6 is encountered.
    
10 - escape character 10 (drawing horizontal line from right to left, 1 pixel thick), the next byte after it stands for the length of the line in pixels.
     The line starts at 1 pixel left to the starting position of the next character.
     Move forward/backward or set the alignment adjustment before drawing lines as necessary.
     The horizontal line does not change the horizontal position of the next character.

11 - escape character 11 (drawing vertical line from bottom to top, 1 pixel thick), the next byte after it stands for the length of the line in pixels.
     The vertical line starts at the current vertical position (the starting position of the next character) and
     the bottom of the current horizontal alignment.
     Move forward/backward or set the alignment adjustment before drawing lines as necessary.
     The horizontal line does not change the horizontal position of the next character.

12 - escape character 12 (single pixel horizontal line), the horizontal line pccupies from the left-most pixel to the right-most pixel.
	The line is 1 pixel high.
	
13 - escape character 13 (double pixel horizontal line), the horizontal line pccupies from the left-most pixel to the right-most pixel.
	The line is 2 pixel high.

14 - escape character 14 (single pixel vertical line), the vertical line pccupies from the top to the bottom of the current line space.
	The vertical line pccupies 1 pixel width.

15 - escape character 15 (double pixel vertical line), the vertical line pccupies from the top to the bottom of the current line space.
	The vertical line pccupies 2 pixel width.
*/
#ifndef _LCD_BUF_DRAW_H
#define _LCD_BUF_DRAW_H

#include "bmf.h"

#define ESC_0_SPACE_LINE		1
#define ESC_1_NEW_LINE_DEFAULT_FONT	2
#define ESC_2_NEW_LINE_SAME_FONT        3
#define ESC_3_NEW_LINE_WITH_FONT	4
#define ESC_4_CHANGE_FONT		5 
#define ESC_5_RESET_TO_DEFAULT_FONT	6 
#define ESC_6_RESET_TO_DEFAULT_ALIGN	7 
#define ESC_7_FORWARD			8 
#define ESC_8_BACKWARD			9 
#define ESC_9_ALIGN_ADJUSTMENT		10
#define ESC_10_HORIZONTAL_LINE		11
#define ESC_11_VERTICAL_LINE		12
#define ESC_12_FULL_HORIZONTAL_LINE	13
#define ESC_13_FULL_VERTICAL_LINE	14
#define MAX_ESC_CHAR ESC_13_FULL_VERTICAL_LINE

#define LINE_SPACE_ADDON 1
#define LCD_BUF_WIDTH_PIXELS 240
#define LCD_LEFT_MARGIN 6
#define LCD_BUF_HEIGHT_PIXELS 64 * 1024
#ifdef WIKIPCF
#define LCD_BUF_WIDTH_BYTES LCD_BUF_WIDTH_PIXELS/8
#define LCD_HEIGHT_LINES 208
#define LCD_VRAM_WIDTH_PIXELS 256
#else
#define LCD_BUF_WIDTH_BYTES LCD_VRAM_WIDTH_PIXELS/8
#endif

// The italic fonts may not define the bitmaps for all characters.
// The characters that are minssing in the italic fonts will try to return the bitmaps in the next font file.
// For example, return bitmaps in DEFAULT_FONT_IDX if not found in ITALIC_FONT_IDX.
#define ITALIC_FONT_IDX			1 
#define DEFAULT_FONT_IDX		2 
#define TITLE_FONT_IDX			3
#define SUBTITLE_FONT_IDX		4
// The above are the primary fonts for the article body.  The index of any one of them needs to be under 7. 
#define DEFAULT_ALL_FONT_IDX		5
#define BOLD_FONT_IDX			DEFAULT_FONT_IDX
#define BOLD_ITALIC_FONT_IDX		ITALIC_FONT_IDX
#define SEARCH_HEADING_FONT_IDX		TITLE_FONT_IDX 
#define SEARCH_LIST_FONT_IDX		DEFAULT_FONT_IDX
#define MESSAGE_FONT_IDX		BOLD_FONT_IDX
#define H2_FONT_IDX			SUBTITLE_FONT_IDX
#define H3_FONT_IDX 			SUBTITLE_FONT_IDX
#define H4_FONT_IDX			SUBTITLE_FONT_IDX
#define H5_FONT_IDX			SUBTITLE_FONT_IDX
#define FILE_BUFFER_SIZE                386*1024

typedef struct _LCD_DRAW_BUF
{
	unsigned char *screen_buf;
	long current_x;
	long current_y;
	pcffont_bmf_t *pPcfFont;
	int drawing;
	int line_height;
	int align_adjustment;
	int vertical_adjustment;
} LCD_DRAW_BUF;

/* Structure of a single article in a file with mutiple articles */
/* byte 0~3: (long) offset from the beginning of the article header to the start of article text */
/* byte 4~5: (short) number of ARTICLE_LINK blocks */
/* byte 6~7: (short) number of EXTERNAL_LINK blocks */
/* section for a number of ARTICLE_LINK blocks */
/* section for a number of EXTERNAL_LINK blocks */
/* section for a number of external link strings */
/* section for the article string */
typedef struct _ARTICLE_HEADER
{
	unsigned long offset_article;
	unsigned short article_link_count;
	unsigned short external_link_count;
} ARTICLE_HEADER;

typedef struct _ARTICLE_LINK
{
	unsigned long start_xy; /* byte 0: x; byte 1~3: y */
	unsigned long end_xy;
	unsigned long article_id;
} ARTICLE_LINK;
	
typedef struct _EXTERNAL_LINK
{
	unsigned long start_xy;
	unsigned long end_xy;
	unsigned short offset_link_str;
	unsigned short link_str_len;
} EXTERNAL_LINK;

void init_lcd_draw_buf();
char* FontFile(int id);
ucs4_t UTF8_to_UCS4(unsigned char **);
void buf_draw_UTF8_str(unsigned char **sUTF8);
void buf_draw_horizontal_line(unsigned long start_x, unsigned long end_x);
void buf_draw_vertical_line(unsigned long start_y, unsigned long end_y);
void buf_draw_char(ucs4_t u);
int get_UTF8_char_width(int idxFont, char **pContent, long *lenContent, int *nCharBytes);
int render_article_with_pcf();
unsigned char * open_article_with_pcf_link(long idx_article);
int display_link_article(long idx_article);
void display_article_with_pcf(int start_y);
void open_article_link(int x,int y);
void init_render_article();
void display_str(char *str);
void init_file_buffer();
int div_wiki(int a,int b);
int isArticleLinkSelected(int x,int y);
void invert_link(int article_link_number);
int draw_bmf_char(ucs4_t u,int font,int x,int y);
int GetFontLinespace(int font);
void show_key(char c);
void set_article_link_number(int num);
int  get_article_link_number();
void open_article_link_with_link_number(int article_link_number);
void display_article_with_pcf_smooth(int start_y);
void scroll_article();
#ifndef WIKIPCF
#ifndef INCLUDED_FROM_KERNEL
int load_init_article(long);
#endif
#endif
void msg_on_lcd(int x, int y, char *msg);
void msg_on_lcd_clear(int x, int y);

extern LCD_DRAW_BUF lcd_draw_buf;
extern pcffont_bmf_t pcfFonts[FONT_COUNT];
extern unsigned char *article_buf_pointer;
#endif /* _LCD_BUF_DRAW_H */

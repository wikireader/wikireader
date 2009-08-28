/*
 *  Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
 *  Copyright (c) 2009 Matt Hsu <matt_hsu@openmoko.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <wikilib.h>
#include <guilib.h>
#include <glyph.h>
#include <search.h>
#include <stdlib.h>
#include <file-io.h>
#include "history.h"
#include "search.h"
#include "msg.h"
#include "lcd_buf_draw.h"

#define DBG_HISTORY 0

#define HISTORY_MAX_ITEM	19
#define HISTORY_MAX_DISPLAY_ITEM	18U
#define MAX_HISTORY 256

HISTORY history_list[MAX_HISTORY];
int history_count;
int history_base;  // index of the top of the histories displayed
int history_changed = 0;
int history_current = 0;  // currently selected history relative to history_base

static inline unsigned int history_modulus(int modulus) {
	return modulus % HISTORY_MAX_DISPLAY_ITEM;
}

//static void __invert_selection(int pos, enum step_direction direction)
//{
//	int start = HISTORY_RESULT_START - HISTORY_RESULT_HEIGHT + 2;
//
//	guilib_fb_lock();
//
//	if (pos == 0) {
//		if (direction == step_down) {
//			guilib_invert(start + pos * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//		}
//		else {
//			guilib_invert(start + pos * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//			guilib_invert(start + (pos + 1) * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//		}
//	} else if (pos == 17) {
//		if (direction == step_up) {
//			guilib_invert(start + pos * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//		}
//		else {
//			guilib_invert(start + pos * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//			guilib_invert(start + (pos - 1) * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//		}
//	}
//	else if (direction == step_down) {
//		guilib_invert(start + pos * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//		guilib_invert(start + (pos - 1) * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//	}
//	else if (direction == step_up) {
//		guilib_invert(start + pos * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//		guilib_invert(start + (pos + 1) * HISTORY_RESULT_HEIGHT, HISTORY_RESULT_HEIGHT);
//	}
//
//	guilib_fb_unlock();
//}

void history_select_down(void)
{
msg(MSG_INFO, "history select down\n");
//	if (history_current == (int)(list_size - 1))
//		return;
//
//	++history_current;
//	display_current = history_modulus(history_current);
//
//	/* bottom reached, not wrapping around */
//	if (display_current == 0 && history_current != 0){
//		history_page_down_display(history_current);
//		__invert_selection(display_current, step_down);
//		return;
//	}
//
//	__invert_selection(display_current, step_down);
}

void history_select_up(void)
{
msg(MSG_INFO, "history select up\n");
//	if (history_current <= 0)
//		return;
//
//	--history_current;
//	display_current = history_modulus(history_current);
//
//	if ( display_current+1 == HISTORY_MAX_DISPLAY_ITEM ){
//		history_page_up_display(history_current);
//		__invert_selection(display_current, step_up);
//		return;
//	}
//
//	__invert_selection(display_current, step_up);
}

//static void history_page_down_display(int current_item)
//{
//	unsigned int i;
//	unsigned int y_pos = HISTORY_RESULT_START;
//
//	guilib_fb_lock();
//
//	guilib_clear();
//	render_string(0, 1, 14, "History", 7);
//
//	for (i = current_item; i < list_size && y_pos < guilib_framebuffer_height(); i++) {
//		const char *p = history_get_item_title(i);
//		render_string(0, 1, y_pos, p, strlen(p)- (TARGET_SIZE));
//		y_pos += HISTORY_RESULT_HEIGHT;
//	}
//
//	guilib_fb_unlock();
//}

//static void history_page_up_display(int current_item)
//{
//	unsigned int i;
//	unsigned int y_pos = HISTORY_RESULT_START;
//
//	guilib_fb_lock();
//
//	guilib_clear();
//	render_string(0, 1, 14, "History:", 8);
//
//	for (i = ((current_item + 1) - HISTORY_MAX_DISPLAY_ITEM); i < list_size && y_pos < guilib_framebuffer_height(); i++) {
//		const char *p = history_get_item_title(i);
//		render_string(0, 1, y_pos, p, strlen(p)- (TARGET_SIZE));
//		y_pos += HISTORY_RESULT_HEIGHT;
//	}
//
//	guilib_fb_unlock();
//}

void history_display(int index)
{
	int i,linespace;

	guilib_fb_lock();

	guilib_clear();
	render_string(SEARCH_HEADING_FONT_IDX, 3, 5, "History", 7);

	if (history_count == 0) {
		render_string(SEARCH_LIST_FONT_IDX, 85, 90, "No history", 10);
	} else {
		unsigned int y_pos = HISTORY_RESULT_START;
                msg(MSG_INFO,"history_count:%d\n",history_count);
		//for (i = index; i <= history_count && y_pos < guilib_framebuffer_height(); i++) {
		//for (i = history_count-index-1; i >=0 && y_pos < guilib_framebuffer_height(); i--) {
		for (i = index; i < history_count && y_pos < guilib_framebuffer_height(); i++) {
			const char *p = history_list[i].title;
			render_string(SEARCH_LIST_FONT_IDX, 3, y_pos, p, strlen(p));
                        linespace = GetFontLinespace(0);
                        //y_pos += linespace;
			y_pos += HISTORY_RESULT_HEIGHT;
                        if((y_pos+linespace)>guilib_framebuffer_height())
                           break;
		}
                history_set_selection(0);//set top
		//invert_selection(history_current, -1, HISTORY_PIXEL_START, HISTORY_RESULT_HEIGHT);
		invert_selection(history_current, -1, HISTORY_RESULT_START, HISTORY_RESULT_HEIGHT);
		history_base = index;
	}

	guilib_fb_unlock();
}

void history_reload()
{
	int i,linespace;

	guilib_fb_lock();

	guilib_clear();
	render_string(SEARCH_HEADING_FONT_IDX, 3, 5, "History", 7);

	if (history_count == 0) {
		render_string(SEARCH_LIST_FONT_IDX, 95, 90, "No history", 10);
	} else {
		unsigned int y_pos = HISTORY_RESULT_START;
                msg(MSG_INFO,"history_count:%d\n",history_count);
		for (i = 0; i < history_count && y_pos < guilib_framebuffer_height(); i++) {
			const char *p = history_list[i].title;
			render_string(SEARCH_LIST_FONT_IDX, 3, y_pos, p, strlen(p));
			y_pos += HISTORY_RESULT_HEIGHT;
                        linespace = GetFontLinespace(0);
                        //y_pos += linespace;
                        if((y_pos+linespace)>guilib_framebuffer_height())
                           break;

		}
		//invert_selection(history_current, -1, HISTORY_PIXEL_START, HISTORY_RESULT_HEIGHT);
		invert_selection(history_current, -1, HISTORY_RESULT_START, HISTORY_RESULT_HEIGHT);
	}

	guilib_fb_unlock();
        
}

void history_reset(void)
{
	history_base = 0;
}

//const char *history_current_target(void)
//{
//	return history_get_item_target(history_current);
//}

//static int history_item_comp(const void *value, unsigned int offset, const struct wl_list *node)
//{
//	char *p = (void *)(node)+sizeof(struct wl_list)+offset;
//
//	return strcmp(p, (char *)value);
//}

//struct history_item *history_find_item_title(const char *title)
//{
//	return (struct history_item *)
//		(wl_list_search(&head.list, title, 0, history_item_comp));
//}

//struct history_item *history_find_item_target(const char *target)
//{
//	return (struct history_item *)
//		(wl_list_search(&head.list, target, (sizeof(char)*MAXSTR), history_item_comp));
//}

void history_add(const long idx_article, const char *title)
{
	int i = 0;
	int bFound = 0;
	
	history_base = 0;
	history_current = 0;
	while (!bFound && i < history_count)
	{
		if (idx_article == history_list[i].idx_article)
                {
                    HISTORY history_tmp;
                    history_tmp = history_list[i];
                    memrcpy((void*)&history_list[1],(void*)&history_list[0],sizeof(HISTORY)*i);
                    history_list[0]=history_tmp;
		    bFound = 1;
                }
		else
			i++;
	}
	
	if(bFound)
           return;
        
        if (history_count == MAX_HISTORY)
        	history_count--;
        memrcpy((void*)&history_list[1],(void*)&history_list[0],sizeof(HISTORY)*history_count);
        msg(MSG_INFO,"idx_article:%d,title:%s\n",idx_article,title);
        history_list[0].idx_article = idx_article;
        strcpy(history_list[0].title, title);
        history_count++;
        
	history_changed = 1;
}

//void history_move_current_to_top(const char *target)
//{
//	struct history_item *node = NULL;
//
//	if ((node = history_find_item_target(target)))
//		wl_list_move2_first(&head.list, &node->list);
//}
//
//const char *history_get_top_target(void)
//{
//	return history_get_item_target(0);
//}
//
//static struct history_item * __history_get_item(unsigned int index)
//{
//	if (index > list_size)
//		return NULL;
//
//	return (struct history_item *)(wl_list_find_nth_node(&head.list, index));
//}
//
//const char *history_get_item_title(unsigned int index)
//{
//	struct history_item *p = __history_get_item(index);
//
//	if (p)
//		return p->title;
//	else
//		return NULL;
//}
//
//const char *history_get_item_target(unsigned int index)
//{
//	struct history_item *p = __history_get_item(index);
//
//	if (p)
//		return p->target;
//	else
//		return NULL;
//}
//
//unsigned int history_item_size(void)
//{
//	return wl_list_size(&head.list);
//}
//
//unsigned int history_free_item_size(void)
//{
//	return wl_list_size(&free_list.list);
//}
//
//const char *history_release(int y)
//{
//	unsigned int i;
//	int start = HISTORY_RESULT_START - HISTORY_RESULT_HEIGHT + 2;
//
//	for (i = 0; i < HISTORY_MAX_DISPLAY_ITEM; ++i, start += HISTORY_RESULT_HEIGHT) {
//		if (y >= start && y < start + HISTORY_RESULT_HEIGHT) {
//				return history_get_item_target(history_current == -1 ? i :
//						(history_current/HISTORY_MAX_DISPLAY_ITEM)*HISTORY_MAX_DISPLAY_ITEM + i);
//		}
//	}
//
//	return NULL;
//}
//
const int history_get_selection()
{
	return history_current;
}

void history_set_selection(int selection)
{
	history_current = selection;
}

unsigned int history_get_count()
{
	return history_count;
}
int history_get_base()
{
	return history_base;
}

void history_list_init(void)
{
	int len;
	int fd_hst;

	fd_hst = wl_open("pedia.hst", WL_O_RDONLY);
	if (fd_hst >= 0)
	{
		len = wl_read(fd_hst, (void *)history_list, sizeof(history_list));
		history_count = len / sizeof(HISTORY);
		wl_close(fd_hst);
	}
	else
		history_count = 0;
	history_base = 0;
}

void history_list_save(void)
{
	int fd_hst;
	int rc;
	
	if (history_changed)
	{
		fd_hst = wl_open("pedia.hst", WL_O_WRONLY);
		if (fd_hst < 0)
			fd_hst = wl_open("pedia.hst", WL_O_CREATE);
		if (fd_hst >= 0)
		{
			rc = wl_write(fd_hst, (void *)history_list, sizeof(HISTORY) * history_count);
			wl_close(fd_hst);
		}
		history_changed = 0;
	}
}

int set_history_list_base(int offset,int offset_count)
{
   int first_item_count;
   int screen_display_count = 9;
   int base_last;
    
   base_last = history_base;

   if(history_count <= screen_display_count)
        return 0;

   if(offset>=0)
      first_item_count = offset_count + history_base;
   else
      first_item_count = history_base -offset_count;


   if(first_item_count<0)
       history_base = 0;

   else if(first_item_count > (history_count - screen_display_count))
   {
       history_base = history_count - screen_display_count;
       if(history_base < 0)
           history_base = 0;

   }
   else
       history_base = first_item_count;

   msg(MSG_INFO,"base from %d to %d,offset:%d\n",base_last,history_base,offset);
   if(base_last!=history_base)
      return 1;
   else
      return 0;
}

void history_open_article(int new_selection)
{
        long idx_article;
        char title[MAX_TITLE_SEARCH];
        
        msg(MSG_INFO,"history open article index:%d\n",new_selection);
	history_current = new_selection;
	idx_article = history_list[history_current + history_base].idx_article;
	strcpy(title, history_list[history_current + history_base].title);
	display_link_article(idx_article);
	history_add(idx_article, title);
}

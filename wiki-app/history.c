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
#ifndef INCLUDED_FROM_KERNEL
#include <stdio.h>
#include <errno.h>
#endif

#define DBG_HISTORY 0

#define HISTORY_MAX_ITEM	19
#define HISTORY_MAX_DISPLAY_ITEM	18U

HISTORY history_list[MAX_HISTORY];
int history_count = 0;
int rendered_history_count = -1;
int history_base;  // Not used, to be cleaned up
int history_changed = 0;
int history_current = 0;  // currently selected history relative to history_base

static inline unsigned int history_modulus(int modulus) {
	return modulus % HISTORY_MAX_DISPLAY_ITEM;
}

void history_reload()
{
	rendered_history_count = 0;
	render_history_with_pcf();
}

void history_reset(void)
{
	history_base = 0;
}

void history_add(const long idx_article, const char *title)
{
	int i = 0;
	int bFound = 0;
	
	history_changed = 2;
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
        history_list[0].idx_article = idx_article;
        strcpy(history_list[0].title, title);
        history_list[0].last_y_pos = 0;
        history_count++;
}

void history_log_y_pos(const long y_pos)
{
	history_changed = 1;
	history_list[0].last_y_pos = y_pos;
}

long history_get_y_pos(const long idx_article)
{
	int i = 0;
	
	while (i < history_count)
	{
		if (idx_article == history_list[i].idx_article)
                {
     			return history_list[i].last_y_pos;
                }
		else
			i++;
	}
	return 0;
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
int history_get_selection()
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

void history_clear()
{
	history_count = 0;
	history_changed = 1;
}

int history_get_base()
{
	return history_base;
}

void history_list_init(void)
{
	int len;
	int fd_hst;

	history_count = 0;
	fd_hst = wl_open("pedia.hst", WL_O_RDONLY);
	if (fd_hst >= 0)
	{
		while ((len = wl_read(fd_hst, (void *)&history_list[history_count], sizeof(HISTORY))) >= sizeof(HISTORY))
		{
			history_count++;
		}
		wl_close(fd_hst);
	}
	history_base = 0;
}

int history_list_save(int level)
{
	int fd_hst;
	int rc = 0;
	
	if (history_changed >= level)
	{
		fd_hst = wl_open("pedia.hst", WL_O_CREATE);
		if (fd_hst >= 0)
		{
			wl_write(fd_hst, (void *)history_list, sizeof(HISTORY) * history_count);
			wl_close(fd_hst);
		}
		history_changed = 0;
		rc = 1;
	}
	return rc;
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

   if(base_last!=history_base)
      return 1;
   else
      return 0;
}

void history_open_article(int new_selection)
{
        long idx_article;
        char title[MAX_TITLE_SEARCH];
        
	history_current = new_selection;
	idx_article = history_list[history_current + history_base].idx_article;
	strcpy(title, history_list[history_current + history_base].title);
	display_link_article(idx_article);
}

//
// Authors:	Daniel Mack <daniel@caiaq.de>
// 		Holger Hans Peter Freyther <zecke@openmoko.org>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#include <stdlib.h>
#include <inttypes.h>
#include <wikilib.h>
#include <article.h>
#include <guilib.h>
#include <glyph.h>
#include <fontfile.h>
#include <history.h>
#include <wl-keyboard.h>
#include <input.h>
#include <msg.h>
#include <file-io.h>
#include <search.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <perf.h>
#include <profile.h>
#include <malloc-simple.h>
#include "wom_reader.h"
#include <bmf.h>
#include <lcd_buf_draw.h>
#include <lcd.h>
#include <tick.h>
#include "delay.h"

//#include <t_services.h>
//#include <kernel.h>
//#include <wl-time.h>

unsigned int get_time(void);

#define DBG_WL 0

enum display_mode_e {

	DISPLAY_MODE_INDEX,
	DISPLAY_MODE_ARTICLE,
	DISPLAY_MODE_HISTORY,
	DISPLAY_MODE_IMAGE,
};

struct pos {
	unsigned int x;
	unsigned int y;
};

static int last_display_mode = 0;
static int display_mode = DISPLAY_MODE_INDEX;
static struct keyboard_key * pre_key= NULL;
static unsigned int article_touch_down_handled = 0;
static unsigned int touch_down_on_keyboard = 0;
static unsigned int touch_down_on_list = 0;
static struct pos article_touch_down_pos;
static unsigned int touch_y_last_unreleased = 0;
static unsigned int start_move_time = 0;
int    last_index_y_pos;
int    enter_touch_y_pos = -1;
int    last_history_y_pos;
wom_file_t * g_womh = 0;
char * articleBuffer = 0;
int articleLength = 0;
unsigned char * membuffer = 0;
int membuffersize = 0;
int curBufferPos  = 0;
unsigned char *article_buf_pointer;
//int is_rendering = 0;
int last_selection = 0;
int start_search_time,end_search_time,start_search_selection_time;
int last_article_link_number = -1;
int last_article_move_time,touch_y_last_article;
int touch_y_last_article_list[10],touch_time_last_article_list[10];
int article_touch_count = 0;
int touch_history = 0;
int start_history_selection_time = 0;
extern int article_offset;


static void repaint_search(void)
{
	guilib_fb_lock();
	search_reload();
	keyboard_paint();
	guilib_fb_unlock();
}

/* this is only called for the index page */
static void toggle_soft_keyboard(void)
{
	//guilib_fb_lock();

	/* Set the keyboard mode to what we want to change to. */
	if (keyboard_get_mode() == KEYBOARD_NONE) {
		keyboard_set_mode(KEYBOARD_CHAR);
		search_reload();
		keyboard_paint();
	} else {
		keyboard_set_mode(KEYBOARD_NONE);
		search_reload();
	}

	//guilib_fb_unlock();
}

static void print_intro()
{
        //keyboard_set_mode(KEYBOARD_CHAR);

	guilib_fb_lock();
	guilib_clear();
	render_string(MESSAGE_FONT_IDX, 40, 55, "Type a word or phrase", 21);
    
	//membuffer = malloc_simple(1024*1024,MEM_TAG_ARTICLE_F1);
	keyboard_paint();
	guilib_fb_unlock();
}

static unsigned int s_article_y_pos;
static uint32_t s_article_offset = 0;

void invert_selection(int old_pos, int new_pos, int start_pos, int height)
{
	guilib_fb_lock();

	if (old_pos != -1)
		guilib_invert(start_pos + old_pos * height, height);
	if (new_pos != -1)
		guilib_invert(start_pos + new_pos * height, height);

	guilib_fb_unlock();
}
void invert_area(int start_x, int start_y, int end_x, int end_y)
{
	guilib_fb_lock();
        guilib_invert_area(start_x,start_y,end_x,end_y);
	guilib_fb_unlock();
}

int article_open(const char *article)
{
	DP(DBG_WL, ("O article_open() '%s'\n", article));
	s_article_offset = strtoul(article, 0 /* endptr */, 16 /* base */);
	s_article_y_pos = 0;
	return 0;
}

void article_display(enum article_nav nav)
{
	unsigned int screen_height = guilib_framebuffer_height();

	DP(DBG_WL, ("O article_display() %i article_offset %u article_y_pos %u\n", nav, s_article_offset, s_article_y_pos));
	if (nav == ARTICLE_PAGE_NEXT)
		s_article_y_pos += screen_height;
	else if (nav == ARTICLE_PAGE_PREV)
		s_article_y_pos = (s_article_y_pos <= screen_height) ? 0 : s_article_y_pos - screen_height;
//	wom_draw(g_womh, s_article_offset, framebuffer, s_article_y_pos, screen_height);
}
/*int article_open_pcf(const char *filename)
{

	FILE * pFile;
	long lSize;
	size_t result;
	
	pFile = fopen (filename , "rb" );

	if (pFile==NULL) 
		return -1;

	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);

	rewind (pFile);
	
	// allocate memory to contain the whole file:
	articleBuffer = (char*) malloc (sizeof(char)*lSize);
	if (articleBuffer == NULL) 
	{
		fclose(pFile);
		return -1;
	}
	
	// copy the file into the buffer:
	result = fread (articleBuffer,1,lSize,pFile);
	if (result != lSize) 
	{
		fclose(pFile);
		return -1;
	}
	
	// terminate
	fclose (pFile);

	framebuffer_article=(char*)malloc(guilib_framebuffer_width()*nLine);
	memset(framebuffer_article,0,guilib_framebuffer_width()*nLine);

	render_article(articleBuffer,articleLength,framebuffer_article);
    
	return lSize;
}*/
void article_display_pcf(int yPixel)
{
	int pos;
    int copysize;
    int framebuffersize;
	
	framebuffersize  = 	guilib_framebuffer_size();
	msg(MSG_INFO,"framebuffersize is:%d\n",framebuffer);

	pos = curBufferPos+((yPixel*LCD_VRAM_WIDTH_PIXELS)/8);
	if(pos<0 || pos>membuffersize)
	{
		msg(MSG_INFO,"pos is:%d,membuffersize is:%d\n",pos,membuffersize);
		return;
	}

	copysize = membuffersize-pos;

	
	if(copysize>framebuffersize)
		copysize = framebuffersize;

	else
	{
		msg(MSG_INFO,"copysize:%d<=framebuffersize:%d\n",copysize,framebuffersize);
		return;
	}

	msg(MSG_INFO,"pos is:%d,membuffersize is:%d,copysize is:%d\n",pos,membuffersize,copysize);

	guilib_fb_lock();
	guilib_clear();
	
	memcpy(framebuffer,membuffer+pos,copysize);
	
	guilib_fb_unlock();
	
	curBufferPos = pos;
}

/*void open_article_pcf(char* filename)
{
	int len = render_text(membuffer,filename,73,70);
	membuffersize = len;
	
	msg(MSG_INFO,"render len is:%d\n",len);

    int framebuffersize = 	guilib_framebuffer_size();

	msg(MSG_INFO,"framebuffersize  is:%d\n",framebuffersize);

	display_mode = DISPLAY_MODE_ARTICLE;
	
	last_display_mode = DISPLAY_MODE_INDEX;

	if(len > framebuffersize)
		len = framebuffersize;

	if(len>0)
	{
		guilib_fb_lock();
		guilib_clear();
		
		memcpy(framebuffer,membuffer,len);

		guilib_fb_unlock();

		curBufferPos = 0;
	}
	
	

}*/

void open_article(const char* target, int mode)
{
/*	DP(DBG_WL, ("O open_article() target '%s' mode %i\n", target, mode));
	if (!target)
		return;

	if (article_open(target) < 0)
		print_article_error();
	display_mode = DISPLAY_MODE_ARTICLE;
	article_display(ARTICLE_PAGE_0);

	if (mode == ARTICLE_NEW) {
		last_display_mode = DISPLAY_MODE_INDEX;
//		history_add(search_current_selection(), target);
	} else if (mode == ARTICLE_HISTORY) {
		last_display_mode = DISPLAY_MODE_HISTORY;
//		history_move_current_to_top(target);
	}
*/
}


static void handle_search_key(char keycode)
{
        int mode;

	if (keycode == WL_KEY_BACKSPACE) {
		search_remove_char();
	} else if (is_supported_search_char(keycode)) {
		search_add_char(tolower(keycode));
	} else {
                if(keycode == -42)
                {
                  mode = keyboard_get_mode();
                  if(mode == KEYBOARD_CHAR)
                      keyboard_set_mode(KEYBOARD_NUM);                  
                  else if(mode == KEYBOARD_NUM)
                      keyboard_set_mode(KEYBOARD_CHAR);                  
                }
		else
                {
                   msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
		   return;
                }
	}

	guilib_fb_lock();
	search_reload();
	keyboard_paint();
	guilib_fb_unlock();
}

static void handle_cursor(struct wl_input_event *ev)
{
	DP(DBG_WL, ("O handle_cursor()\n"));
	msg(MSG_INFO,"handle_cursor,ev->key_event.keycode:%d\n",ev->key_event.keycode);
	if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			//article_display_pcf(50);
                        display_article_with_pcf(50);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			//article_display_pcf(-50);
                        display_article_with_pcf(-50);
	} else if (display_mode == DISPLAY_MODE_INDEX) {
//                is_rendering = 0;
		article_buf_pointer = NULL;
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			search_select_down();
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			search_select_up();
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
//                is_rendering = 0;
		article_buf_pointer = NULL;
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			history_select_down();
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			history_select_up();
	}
}
/*static void handle_cursor(struct wl_input_event *ev)
{
	DP(DBG_WL, ("O handle_cursor()\n"));
	if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			article_display(ARTICLE_PAGE_NEXT);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			article_display(ARTICLE_PAGE_PREV);
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			search_select_down();
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			search_select_up();
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			history_select_down();
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			history_select_up();
	}
}*/

static void handle_key_release(int keycode)
{
//	static long idx_article = 0;

	DP(DBG_WL, ("O handle_key_release()\n"));
	msg(MSG_INFO,"handle_key_release,keycode:%d\n",keycode);
	if (keycode == WL_INPUT_KEY_SEARCH) {
//                is_rendering = 0;
                article_offset = 0;
		article_buf_pointer = NULL;
		/* back to search */
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
			display_mode = DISPLAY_MODE_INDEX;
			repaint_search();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
//                is_rendering = 0;
                article_offset = 0;
		article_buf_pointer = NULL;
		display_mode = DISPLAY_MODE_HISTORY;
		//history_display(0);
                history_reload();
	} else if (keycode == WL_INPUT_KEY_RANDOM) {
                article_offset = 0;
                display_mode = DISPLAY_MODE_ARTICLE;
	        last_display_mode = DISPLAY_MODE_INDEX;
	        random_article();
               //void *p =open_article_with_pcf_link("1.wki");
 //               article_buf_pointer = open_article_with_pcf_link("1.wki");
 //               if(article_buf_pointer!=NULL)
 //               {
 //                   init_render_article();
 //                   render_article_with_pcf(&article_buf_pointer);
 //               }
                
	} else if (display_mode == DISPLAY_MODE_INDEX) {
//                is_rendering = 0;
		article_buf_pointer = NULL;
		if (keycode == WL_KEY_RETURN) {
			int cur_selection = search_current_selection();
			retrieve_article(cur_selection);
#ifdef PROFILER_ON
		} else if (keycode == WL_KEY_HASH) {
			/* activate if you want to run performance tests */
			/* perf_test(); */
			malloc_status_simple();
			prof_print();
#endif
		} else {
			handle_search_key(keycode);
		}
	} else if (display_mode == DISPLAY_MODE_ARTICLE) {
//                is_rendering = 0;
		article_buf_pointer = NULL;
		if (keycode == WL_KEY_BACKSPACE) {
			if (last_display_mode == DISPLAY_MODE_INDEX) {
				display_mode = DISPLAY_MODE_INDEX;
				repaint_search();
			} else if (last_display_mode == DISPLAY_MODE_HISTORY) {
				display_mode = DISPLAY_MODE_HISTORY;
				history_reset();
				//history_display(0);
                                history_reload();
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (keycode == WL_KEY_RETURN) {
//			open_article(history_current_target(), ARTICLE_HISTORY);
		}
	}
}
/*static void handle_key_release(int keycode)
{
	DP(DBG_WL, ("O handle_key_release()\n"));
	if (keycode == WL_INPUT_KEY_SEARCH) {
		// back to search 
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
			display_mode = DISPLAY_MODE_INDEX;
			repaint_search();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
		display_mode = DISPLAY_MODE_HISTORY;
		history_display();
	} else if (keycode == WL_INPUT_KEY_RANDOM) {
		random_article();
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		if (keycode == WL_KEY_RETURN) {
			const char* target = search_current_title();
			if (target && strlen(target) >= TARGET_SIZE)
				open_article(&target[strlen(target)-TARGET_SIZE], ARTICLE_NEW);
#ifdef PROFILER_ON
		} else if (keycode == WL_KEY_HASH) {
			// activate if you want to run performance tests 
			// perf_test(); 
			malloc_status_simple();
			prof_print();
#endif
		} else {
			handle_search_key(keycode);
		}
	} else if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (keycode == WL_KEY_BACKSPACE) {
			if (last_display_mode == DISPLAY_MODE_INDEX) {
				display_mode = DISPLAY_MODE_INDEX;
				repaint_search();
			} else if (last_display_mode == DISPLAY_MODE_HISTORY) {
				display_mode = DISPLAY_MODE_HISTORY;
				history_reset();
				history_display();
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (keycode == WL_KEY_RETURN) {
			open_article(history_current_target(), ARTICLE_HISTORY);
		}
	}
}*/

static void handle_touch(struct wl_input_event *ev)
{
        int offset,offset_count,article_link_number=-1;
        int enter_touch_y_pos_record,time_diff_search;

	DP(DBG_WL, ("%s() touch event @%d,%d val %d\n", __func__,
		ev->touch_event.x, ev->touch_event.y, ev->touch_event.value));

	if (display_mode == DISPLAY_MODE_INDEX) {
		struct keyboard_key * key;
//                is_rendering = 0;
		article_buf_pointer = NULL;
                msg(MSG_INFO,"handle_touch,display_mode==INDEX\n");
		key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
		if (ev->touch_event.value == 0) {
			show_key(0);
                        enter_touch_y_pos_record = enter_touch_y_pos;
                        enter_touch_y_pos = -1;
                        msg(MSG_INFO,"ev->touch_event.value == 0\n");
			pre_key = NULL;
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}

				handle_search_key(key->key);
			}
			else {
				if (!touch_down_on_list || ev->touch_event.y < RESULT_START - RESULT_HEIGHT) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}
                                if(search_result_count()==0)
                                   goto out;

                                end_search_time = get_time();

                                if(start_search_selection_time<0)
                                    time_diff_search = 0;
                                else
                                   time_diff_search = (end_search_time - start_search_selection_time)/24/1000;



                                if(time_diff_search<500 && enter_touch_y_pos_record>=0 && abs(enter_touch_y_pos_record-ev->touch_event.y)>20)
                                {
                                    offset = ev->touch_event.y-enter_touch_y_pos_record;
                                    
                                    msg(MSG_INFO,"start y:%d,end y:%d\n",enter_touch_y_pos_record,ev->touch_event.y);
                                    msg(MSG_INFO,"offset>20,repaint search,pixel offset:%d,RESULT_HEIGHT:%d\n",offset,RESULT_HEIGHT);

                                    offset_count = div_wiki(abs(offset),RESULT_HEIGHT);
                                    msg(MSG_INFO,"offset result list:%d\n",offset);
                                    if(set_result_list_base(offset,offset_count)>0)
                                    {
                                    repaint_search();
                                    last_selection = 0;
                                        msg(MSG_INFO,"last_selection set to -0\n");
                                    }
                                 }
                                 else
                                 {
                                    display_mode = DISPLAY_MODE_ARTICLE;
			            last_display_mode = DISPLAY_MODE_INDEX;
				    search_set_selection(last_selection);
				    search_open_article(last_selection);
                                 }
                                
                                
                              
//				int cur_selection = search_current_selection();
//					retrieve_article(cur_selection);
//				else
//					repaint_search();
			}
			touch_down_on_keyboard = 0;
			touch_down_on_list = 0;
		} else {
                        msg(MSG_INFO,"ev->touch_event.value != 0\n");
                        if(enter_touch_y_pos<0)  //record first touch y pos
                           enter_touch_y_pos = ev->touch_event.y;
                        last_index_y_pos = ev->touch_event.y;
                        start_search_time = get_time();
			if (key) {
				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_keyboard = 1;

				if (pre_key && pre_key->key == key->key) goto out;

				if (pre_key) {
					guilib_invert_area(pre_key->left_x, pre_key->left_y, pre_key->right_x, pre_key->right_y);
					show_key(0);
				}
				if (touch_down_on_keyboard) {
					guilib_invert_area(key->left_x, key->left_y, key->right_x, key->right_y);
					show_key(key->key);
					pre_key = key;
				}
			} else {
                                msg(MSG_INFO,"key is null\n");
				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_list = 1;
				if (pre_key) {
					guilib_invert_area(pre_key->left_x, pre_key->left_y, pre_key->right_x, pre_key->right_y);
					show_key(0);
					pre_key = NULL;
				}

				if (!search_result_count()) goto out;

				int new_selection;
                                if((ev->touch_event.y - RESULT_START)<0)
                                    new_selection = 0;
                                else
                                    new_selection = ((unsigned int)ev->touch_event.y - RESULT_START) / RESULT_HEIGHT;
                                
                                msg(MSG_INFO,"ev->touch_event.y - RESULT_START:%d,new_selection:%d\n",ev->touch_event.y - RESULT_START,new_selection);

				if (new_selection == search_result_selected()) goto out;

				unsigned int avail_count = keyboard_get_mode() == KEYBOARD_NONE ? NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
				avail_count = search_result_count()-search_result_first_item() > avail_count ? avail_count : search_result_count()-search_result_first_item();
				if (new_selection >= avail_count) goto out;
				if (touch_down_on_keyboard) goto out;
                                
                                //repaint_search();
                                msg(MSG_INFO,"invert last_select:%d,new selection:%d\n",search_result_selected(),new_selection);
				//invert_selection(search_result_selected(), -1, PIXEL_START, RESULT_HEIGHT);
                                
				//invert_selection(search_result_selected(), new_selection, PIXEL_START, RESULT_HEIGHT);

				invert_selection(search_result_selected(), new_selection, RESULT_START, RESULT_HEIGHT);
                                
                                
                                 last_selection = new_selection ;
                                msg(MSG_INFO,"last_selection set to:%d\n",last_selection);
		                //display_mode = DISPLAY_MODE_ARTICLE;
			        //last_display_mode = DISPLAY_MODE_INDEX;
				search_set_selection(new_selection);
                                start_search_selection_time = get_time();
                                
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
//                is_rendering = 0;
                msg(MSG_INFO,"history touch event\n");
		article_buf_pointer = NULL;
                int end_history_time,time_diff_history,offset,offset_count;
                if (ev->touch_event.value == 0) 
                {
                     touch_history = 0;

                     end_history_time = get_time();

                     if(start_history_selection_time<0)
                         time_diff_history = 0;
                     else
                         time_diff_history = (end_history_time - start_history_selection_time)/24/1000;

                     if(time_diff_history<500 && last_history_y_pos>=0 && abs(last_history_y_pos-ev->touch_event.y)>20)
                     //if(abs(last_history_y_pos - ev->touch_event.y)>20)
                     {
                          offset = ev->touch_event.y-last_history_y_pos;

                          offset_count = div_wiki(abs(offset),HISTORY_RESULT_HEIGHT);
                          //msg(MSG_INFO,"historyoffset result list:%d\n",offset);
                          if(set_history_list_base(offset,offset_count)>0)
                          {
                            int base;
                            base  = history_get_base();

                            history_display(base);
                          }
                     }
                     else
                     {
                          display_mode = DISPLAY_MODE_ARTICLE;
	                  last_display_mode = DISPLAY_MODE_HISTORY;
			  history_open_article(history_get_selection());
                     }
                }
                else
                {
                    unsigned int new_selection;
                    if(touch_history == 0)
                    {
                       last_history_y_pos = ev->touch_event.y;
                       touch_history = 1;
                    }

		    if((ev->touch_event.y - HISTORY_RESULT_START)<0)
			new_selection = 0;
		    else
			new_selection = ((unsigned int)ev->touch_event.y - HISTORY_RESULT_START ) / HISTORY_RESULT_HEIGHT ;
                    
                    if(new_selection == history_get_selection())
                         goto out;
	
		    if (new_selection >= history_get_count()) goto out;
	
	            invert_selection(history_get_selection(), new_selection, HISTORY_RESULT_START, HISTORY_RESULT_HEIGHT);
	
		    history_set_selection(new_selection);
		    start_history_selection_time = get_time();
		    

                }
//		unsigned int new_selection = ((unsigned int)ev->touch_event.y - HISTORY_RESULT_START - 2) / HISTORY_RESULT_HEIGHT;
//		if (new_selection >= history_get_count()) goto out;
//
//		if (ev->touch_event.value == 0) {
//			const char *target = history_get_item_target(history_get_selection());
//			if (target)
//				open_article(target, ARTICLE_NEW);
//		} else {
//			if (ev->touch_event.y < HISTORY_PIXEL_START) goto out;
//			if (new_selection == history_get_selection()) goto out;
//			invert_selection(history_get_selection(), new_selection, HISTORY_PIXEL_START, HISTORY_RESULT_HEIGHT);
//			history_set_selection(new_selection);
//		}
	} else {
		if (ev->touch_event.value == 0) {
                        int end_move_time,time_diff;
                        int speed = 0,count_next=0,distance=0,i=0;


                        end_move_time = get_time();
                        time_diff = end_move_time-start_move_time;
                        //msg(MSG_INFO,"start_move_time:%d,end_move_time:%d\n",start_move_time,end_move_time);
                        
                        touch_y_last_unreleased    = 0;
                        start_move_time = 0;

                        #ifdef INCLUDED_FROM_KERNEL
                        //if((time_diff/24/1000)>500)
                          //  return;

                        if(article_touch_count>=9)
                            count_next = 0;
                        else
                            count_next = article_touch_count+1;
                        while(true)
                        {
                            if(touch_y_last_article_list[count_next]>0)
                              break;
                            count_next++;
                            if(count_next == article_touch_count)
                              break;
                            if(count_next>=9)
                             count_next = 0;
                        }
                        distance = abs(touch_y_last_article_list[count_next]-ev->touch_event.y);
                        //if(abs(touch_y_last_article_list[count_next]-ev->touch_event.y)!=0)
                            //return;
                        speed= distance*1000000*24/(end_move_time-touch_time_last_article_list[count_next]);
                        for(i=0;i<10;i++)
                        {
                          touch_y_last_article_list[i]=0;
                          touch_time_last_article_list[i]=0;
                        }
                        if(speed<40)
                           return;

                        //extra_scroll = (100-speed)*2;
                        
                        /*if(time_diff>0)
                        { 
                           speed =(article_touch_down_pos.y-ev->touch_event.y)*24/(end_move_time-start_move_time);

                           //msg(MSG_INFO,"move speed:%d,pix:%d,time:%d\n",speed,article_touch_down_pos.y-ev->touch_event.y,end_move_time-start_move_time);
                        }
                        if((article_touch_down_pos.y-ev->touch_event.y)>0)
                           y_move = -5;
                        else
                           y_move = 5;
                        count = 0;
		        sleep = 10;
                        for(;;)
                        {
                            
                            display_article_with_pcf(y_move);
                            if(abs(y_move*count)>(article_touch_down_pos.y-ev->touch_event.y+speed))
                              break;
                            count++;
		            sleep+=1;
                            
		            wl_input_wait(&wie, sleep);


                        }*/
                        #endif
                        if(article_link_number>=0)
                        {
                          invert_link(article_link_number);
                          article_link_number = -1;
                        }

			if (article_touch_down_pos.y > ev->touch_event.y &&
					abs(article_touch_down_pos.y - ev->touch_event.y) > 150)
				//article_display(ARTICLE_PAGE_NEXT);
                                display_article_with_pcf_smooth(LCD_HEIGHT_LINES+speed*2);
			else if (article_touch_down_pos.y < ev->touch_event.y &&
					abs(article_touch_down_pos.y - ev->touch_event.y) > 150)
				//article_display(ARTICLE_PAGE_PREV);
                                display_article_with_pcf_smooth(-LCD_HEIGHT_LINES-speed*2);
                        else if(get_article_link_number()>=0)
                        {
                             open_article_link_with_link_number(get_article_link_number());
                        }
                        else if(abs(article_touch_down_pos.y - ev->touch_event.y)<10 && abs(article_touch_down_pos.x - ev->touch_event.x)<10)
                        {
                              open_article_link(article_touch_down_pos.x,article_touch_down_pos.y);                           
                        }
                        else if(abs(article_touch_down_pos.y - ev->touch_event.y) > 10)
                        {
                              if(article_touch_down_pos.y<ev->touch_event.y)
                                 display_article_with_pcf_smooth(article_touch_down_pos.y-ev->touch_event.y-speed*2);
                              else
                                 display_article_with_pcf_smooth(article_touch_down_pos.y-ev->touch_event.y+speed*2);

                        }
                        
				
			article_touch_down_handled = 0;
		} else {
                        article_offset = 0;
                        msg(MSG_INFO,"article_offset set to 0:%d\n",article_offset);
                        //if(abs(touch_y_last_article-ev->touch_event.y)>=5)
                        {
			   last_article_move_time = get_time();
		           touch_y_last_article = ev->touch_event.y;

                           if(article_touch_count>=10)
                               article_touch_count = 0;
                           touch_y_last_article_list[article_touch_count] = ev->touch_event.y;
                           touch_time_last_article_list[article_touch_count] = last_article_move_time;
                           article_touch_count++;
                        }
                        if(touch_y_last_unreleased == 0)
                        {
                           touch_y_last_unreleased = ev->touch_event.y;
                           start_move_time = get_time();
                        }
                        else if(abs(touch_y_last_unreleased - ev->touch_event.y) >10)
                        {
                              display_article_with_pcf(touch_y_last_unreleased - ev->touch_event.y);
                              touch_y_last_unreleased = ev->touch_event.y;
                        }
                        else
                        {
			      article_link_number =isArticleLinkSelected(ev->touch_event.x,ev->touch_event.y);
			      //msg(MSG_INFO,"article_link_number:%d\n",article_link_number);
                              last_article_link_number = get_article_link_number();
			      if(article_link_number>=0 && article_link_number!=last_article_link_number)
                              {
				invert_link(article_link_number);
				invert_link(last_article_link_number);
                                set_article_link_number(article_link_number);
                              }
                              else if(article_link_number<0 && last_article_link_number>=0)
                              {
                                invert_link(last_article_link_number);
                                set_article_link_number(-1);
                              }
                        }

			if (!article_touch_down_handled) {
				article_touch_down_pos.x = ev->touch_event.x;
				article_touch_down_pos.y = ev->touch_event.y;
				article_touch_down_handled = 1;
			}
		}
	}
out:
	return;
}

int wikilib_init (void)
{
	// tbd: name more than 8.3 could not be found on the device (fatfs)
	//g_womh = wom_open("wiki.dat");
        init_lcd_draw_buf();
        init_file_buffer();
	return 0;
}

#ifndef INCLUDED_FROM_KERNEL
extern long idx_init_article;
#endif

int wikilib_run(void)
{
        int sleep;
        struct wl_input_event ev;

	print_intro();

	/*
	 * test searching code...
	 */
	article_buf_pointer = NULL;
	search_init();
	history_list_init();
	malloc_status_simple();
#ifndef INCLUDED_FROM_KERNEL
	if (!load_init_article(idx_init_article))
	{
		display_mode = DISPLAY_MODE_ARTICLE;
		last_display_mode = DISPLAY_MODE_INDEX;
	}
#endif

	for (;;) {
		
                if(render_article_with_pcf())
//                if(is_rendering)
                  sleep = 0;
                else
                {
                  history_list_save();
		  sleep = 1;
		}
                if(article_offset>0)
                {
                  scroll_article();
                  sleep = 0;
                }

		wl_input_wait(&ev, sleep);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_CURSOR:
			handle_cursor(&ev);
			break;
		case WL_INPUT_EV_TYPE_KEYBOARD:
			if (ev.key_event.value != 0)
				handle_key_release(ev.key_event.keycode);
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			handle_touch(&ev);
			break;
		}
                
//                if(is_rendering)
//                {
//                  //msg(MSG_INFO,"is rendering article:%d\n",m); 
//                  render_article_with_pcf(&article_buf_pointer);
//                }
	}

	/* never reached */
	return 0;
}

unsigned int get_time(void)
{
        long clock_ticks;

#ifdef INCLUDED_FROM_KERNEL
	clock_ticks = Tick_get();
#else
	clock_ticks = clock();
#endif

	return (unsigned int)clock_ticks;
}

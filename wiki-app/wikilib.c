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
#include <guilib.h>
#include <glyph.h>
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
#include <bmf.h>
#include <lcd_buf_draw.h>
#include <lcd.h>
#include <tick.h>
#include "delay.h"
#include "search_hash.h"

//#include <t_services.h>
//#include <kernel.h>
//#include <wl-time.h>

unsigned int get_time(void);

#define DBG_WL 0
#define SEARCH_FETCH_DELAY 1000000*24

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
char * articleBuffer = 0;
int articleLength = 0;
unsigned char * membuffer = 0;
int membuffersize = 0;
int curBufferPos  = 0;
unsigned char *article_buf_pointer;
//int is_rendering = 0;
int last_selection = 0;
int start_search_time,end_search_time,start_search_selection_time, last_delete_time;
int last_article_link_number = -1;
int last_article_move_time,touch_y_last_article;
int touch_y_last_article_list[10],touch_time_last_article_list[10];
int article_touch_count = 0;
int touch_history = 0;
int start_history_selection_time = 0;
extern int article_offset;
bool random_press = false;
extern int stop_render_article;
int time_random_last = 0;
extern bool search_string_changed;
extern unsigned int time_search_last;
bool press_delete_button = false;
extern bool search_string_changed_remove;
int history_touch_pos_y_last;
int touch_search = 0,search_touch_pos_y_last=0;
bool article_moved = false;
#define INITIAL_ARTICLE_SCROLL_PIXEL 5
int  article_scroll_pixel = INITIAL_ARTICLE_SCROLL_PIXEL;

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
	if (keyboard_get_mode() == KEYBOARD_NONE || search_result_count()==0) {
		keyboard_set_mode(KEYBOARD_CHAR);
//		search_reload_ex();
		keyboard_paint();
	} else {
		keyboard_set_mode(KEYBOARD_NONE);
		search_reload_ex(SEARCH_RELOAD_KEEP_RESULT);
	}

	//guilib_fb_unlock();
}

static void print_intro()
{
        //keyboard_set_mode(KEYBOARD_CHAR);

	guilib_fb_lock();
	guilib_clear();
    
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


#if 0
// this is no longer used
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
#endif

#if 0
// this is no longer used
int article_open_pcf(const char *filename)
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
}
#endif

void article_display_pcf(int yPixel)
{
	int pos;
    int copysize;
    int framebuffersize;
	
	framebuffersize  = 	guilib_framebuffer_size();

	pos = curBufferPos+((yPixel*LCD_VRAM_WIDTH_PIXELS)/8);
	if(pos<0 || pos>membuffersize)
	{
		return;
	}

	copysize = membuffersize-pos;

	
	if(copysize>framebuffersize)
		copysize = framebuffersize;

	else
	{
		return;
	}

	guilib_fb_lock();
	guilib_clear();
	
	memcpy(framebuffer,membuffer+pos,copysize);
	
	guilib_fb_unlock();
	
	curBufferPos = pos;
}

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
//        int mode;
	int rc = 0;

	if (keycode == WL_KEY_BACKSPACE) {
		rc = search_remove_char(1);
	} else if (is_supported_search_char(keycode)) {
		rc = search_add_char(tolower(keycode));
	} else {
                if(keycode == -42)
                {
//                  mode = keyboard_get_mode();
//                  if(mode == KEYBOARD_CHAR)
//                      keyboard_set_mode(KEYBOARD_NUM);                  
//                  else if(mode == KEYBOARD_NUM)
//                      keyboard_set_mode(KEYBOARD_CHAR);   
			rc = -1;               
                }
		else
                {
		   return;
                }
	}

	guilib_fb_lock();
	//search_reload();
	if (!rc)
		search_reload_ex(SEARCH_RELOAD_NORMAL);
//	keyboard_paint();
	guilib_fb_unlock();
}

static void handle_cursor(struct wl_input_event *ev)
{
	DP(DBG_WL, ("O handle_cursor()\n"));
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

	keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW); // reset invert immediately
	DP(DBG_WL, ("O handle_key_release()\n"));
	if (keycode == WL_INPUT_KEY_SEARCH) {
                article_offset = 0;
		article_buf_pointer = NULL;
		/* back to search */
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
                        search_set_selection(-1);
			display_mode = DISPLAY_MODE_INDEX;
                        keyboard_set_mode(KEYBOARD_CHAR);
			repaint_search();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
		if (display_mode != DISPLAY_MODE_HISTORY) {
	                article_offset = 0;
			article_buf_pointer = NULL;
			display_mode = DISPLAY_MODE_HISTORY;
	                history_reload();
	                keyboard_set_mode(KEYBOARD_NONE);
	        } else {
	        	if (keyboard_get_mode() == KEYBOARD_CLEAR_HISTORY)
	        	{
	        		keyboard_set_mode(KEYBOARD_NONE);
	                	history_reload();
	        	} else if (history_get_count() > 0) {
	        		keyboard_set_mode(KEYBOARD_CLEAR_HISTORY);
				guilib_fb_lock();
				keyboard_paint();
				guilib_fb_unlock();
			}
	        }
	} else if (keycode == WL_INPUT_KEY_RANDOM) {
                
                article_offset = 0;
                article_buf_pointer = NULL;
                display_mode = DISPLAY_MODE_ARTICLE;
	        last_display_mode = DISPLAY_MODE_INDEX;
	        random_article();
 
	} else if (display_mode == DISPLAY_MODE_INDEX) {
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

static void handle_touch(struct wl_input_event *ev)
{
        //int offset,offset_count,
        int article_link_number=-1;
        int enter_touch_y_pos_record;
        //int time_diff_search;
        int mode;
	struct keyboard_key * key;

	DP(DBG_WL, ("%s() touch event @%d,%d val %d\n", __func__,
		ev->touch_event.x, ev->touch_event.y, ev->touch_event.value));

	if (display_mode == DISPLAY_MODE_INDEX) {
		article_buf_pointer = NULL;
		key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
		if (ev->touch_event.value == 0) {
			//show_key(0);
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
                        enter_touch_y_pos_record = enter_touch_y_pos;
                        enter_touch_y_pos = -1;
                        touch_search = 0;
                        press_delete_button = false;
			pre_key = NULL;
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}
                                int search_time_press_delete;
                                search_time_press_delete = get_time();
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

				 //search_set_selection(last_selection);
				 //search_open_article(last_selection);
                                 if(search_result_selected()>=0)
                                 {
				    display_mode = DISPLAY_MODE_ARTICLE;
			            last_display_mode = DISPLAY_MODE_INDEX;
                                    search_open_article(search_result_selected());
                                 }
			}
			touch_down_on_keyboard = 0;
			touch_down_on_list = 0;
		} else {
                        if(enter_touch_y_pos<0)  //record first touch y pos
                           enter_touch_y_pos = ev->touch_event.y;
                        last_index_y_pos = ev->touch_event.y;
                        start_search_time = get_time();
                        last_delete_time = start_search_time;
			if (key) {
                                if(key->key==8)//press "<" button
                                {
                                    press_delete_button = true;
                               }
				else if(key->key == -42)
				{
					mode = keyboard_get_mode();
					if(mode == KEYBOARD_CHAR)
						keyboard_set_mode(KEYBOARD_NUM);                  
					else if(mode == KEYBOARD_NUM)
						keyboard_set_mode(KEYBOARD_CHAR);                  
					guilib_fb_lock();
					keyboard_paint();
					guilib_fb_unlock();
				}

				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_keyboard = 1;

				if (pre_key && pre_key->key == key->key) goto out;

				//if (pre_key) {
					//guilib_invert_area(pre_key->left_x, pre_key->left_y, pre_key->right_x, pre_key->right_y);
					//show_key(0);
					//keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY);
				//}
				if (touch_down_on_keyboard) {
					//guilib_invert_area(key->left_x, key->left_y, key->right_x, key->right_y);
					//show_key(key->key);
					keyboard_key_invert(key);
					pre_key = key;
				}
			} else {
				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_list = 1;
				//if (pre_key) {
					//guilib_invert_area(pre_key->left_x, pre_key->left_y, pre_key->right_x, pre_key->right_y);
					//show_key(0);
					keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
					pre_key = NULL;
				//}

				if (!search_result_count()) goto out;

				if(touch_search == 0)
				{
				    //last_search_y_pos = ev->touch_event.y;
				    touch_search = 1;
				}
				else
				{ 
				    if(search_result_selected()>=0 && abs(ev->touch_event.y-search_touch_pos_y_last)>5)
				    {
					invert_selection(search_result_selected(),-1, RESULT_START, RESULT_HEIGHT);
					search_set_selection(-1);
				    }
				    goto out;
				}

				int new_selection;
                                if((ev->touch_event.y - RESULT_START)<0)
                                    new_selection = -1;
                                else
                                    new_selection = ((unsigned int)ev->touch_event.y - RESULT_START) / RESULT_HEIGHT;
                                
				if (new_selection == search_result_selected()) goto out;

				unsigned int avail_count = keyboard_get_mode() == KEYBOARD_NONE ? NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
				avail_count = search_result_count()-search_result_first_item() > avail_count ? avail_count : search_result_count()-search_result_first_item();
				if (new_selection >= avail_count) goto out;
				if (touch_down_on_keyboard) goto out;
                                
				//invert_selection(search_result_selected(), new_selection, RESULT_START, RESULT_HEIGHT);
				invert_selection(-1, new_selection, RESULT_START, RESULT_HEIGHT);

                                 last_selection = new_selection ;
				search_set_selection(new_selection);
                                search_touch_pos_y_last = ev->touch_event.y;
                                start_search_selection_time = get_time();
                                
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (keyboard_get_mode() == KEYBOARD_CLEAR_HISTORY) {
			key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
			if (ev->touch_event.value == 0) {
		                #ifdef INCLUDED_FROM_KERNEL
		                delay_us(100000 * 2);
		                #endif
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW);
	                        enter_touch_y_pos_record = enter_touch_y_pos;
	                        enter_touch_y_pos = -1;
	                        touch_search = 0;
	                        press_delete_button = false;
				pre_key = NULL;
				if (key) {
					if (!touch_down_on_keyboard) {
						touch_down_on_keyboard = 0;
						touch_down_on_list = 0;
						goto out;
					}
					if (key->key == 'Y') {
						history_clear();
						keyboard_set_mode(KEYBOARD_NONE);
						history_reload();
					} else if (key->key == 'N') {
						keyboard_set_mode(KEYBOARD_NONE);
						history_reload();
					}
				}
				else {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}
			} else {
	                        if(enter_touch_y_pos<0)  //record first touch y pos
	                           enter_touch_y_pos = ev->touch_event.y;
	                        last_index_y_pos = ev->touch_event.y;
				if (key) {
					if (!touch_down_on_keyboard)
						touch_down_on_keyboard = 1;
	
					if (pre_key && pre_key->key == key->key) goto out;
	
					if (touch_down_on_keyboard) {
						keyboard_key_invert(key);
						pre_key = key;
					}
				} else {
					touch_down_on_keyboard = 0;
					keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
					pre_key = NULL;
	
				}
			}
		} else {
			article_buf_pointer = NULL;
	                int end_history_time,time_diff_history;
	                //int offset,offset_count;
	                if (ev->touch_event.value == 0) 
	                {
	                     touch_history = 0;
	
	                     end_history_time = get_time();
	
	                     if(start_history_selection_time<0)
	                         time_diff_history = 0;
	                     else
	                         time_diff_history = (end_history_time - start_history_selection_time)/24/1000;
	
	                    
	                     if(history_get_selection()<0)
	                         goto out;
	                     display_mode = DISPLAY_MODE_ARTICLE;
		             last_display_mode = DISPLAY_MODE_HISTORY;
		             history_open_article(history_get_selection());
	                     
	                }
	                else
	                {
	                    unsigned int new_selection;
	                    if(touch_history == 0)
	                    {
	                       last_history_y_pos = ev->touch_event.y;
	                       touch_history = 1;
	                    }
	                    else
	                    { 
	                       if(history_get_selection()>=0 && abs(ev->touch_event.y-history_touch_pos_y_last)>5)
	                       {
		                   invert_selection(history_get_selection(),-1, HISTORY_RESULT_START, HISTORY_RESULT_HEIGHT);
	                           history_set_selection(-1);
	                       }
	                       goto out;
	                    }
	
			    if((ev->touch_event.y - HISTORY_RESULT_START)<0)
				new_selection = 0;
			    else
				new_selection = ((unsigned int)ev->touch_event.y - HISTORY_RESULT_START ) / HISTORY_RESULT_HEIGHT ;
	
			    if (new_selection >= history_get_count()) goto out;
		
		            invert_selection(-1, new_selection, HISTORY_RESULT_START, HISTORY_RESULT_HEIGHT);
		
			    history_set_selection(new_selection);
			    start_history_selection_time = get_time();
			    history_touch_pos_y_last = ev->touch_event.y;
	
	                }
		}
	} else {
		if (ev->touch_event.value == 0) {
                        article_moved = false;
                        article_scroll_pixel = INITIAL_ARTICLE_SCROLL_PIXEL;
                        int end_move_time,time_diff;
                        //int speed = 0,count_next=0,distance=0,i=0;


                        end_move_time = get_time();
                        time_diff = end_move_time-start_move_time;

                        touch_y_last_unreleased    = 0;
                        start_move_time = 0;


                        if(article_link_number>=0)
                        {
                          invert_link(article_link_number);
                          article_link_number = -1;
                        }
                        if(get_article_link_number()>=0)
                        {
                             open_article_link_with_link_number(get_article_link_number());
                             return;
                        }

                        /*#ifdef INCLUDED_FROM_KERNEL

                        if(article_touch_count>=9)
                            count_next = 0;
                        else
                            count_next = article_touch_count+1;
                        while(true)
                        {
                            if(touch_y_last_article_list[count_next]>0)
                              break;
                            count_next++;
                            if(count_next>9)
                             count_next = 0;
                            if(count_next == article_touch_count)
                              break;
                        }
                        distance = abs(touch_y_last_article_list[count_next]-ev->touch_event.y);
                        if((end_move_time-touch_time_last_article_list[count_next])==0)
                        {
                             msg(MSG_INFO,"time diff is 0\n");
                             return;
                        }
                        speed= distance*1000000*24/(end_move_time-touch_time_last_article_list[count_next]);
                        msg(MSG_INFO,"scroll speed:%d\n",speed);
                        for(i=0;i<10;i++)
                        {
                          touch_y_last_article_list[i]=0;
                          touch_time_last_article_list[i]=0;
                        }
                        if(speed<40)
                           return;

                        #endif*/

			/*if (article_touch_down_pos.y > ev->touch_event.y &&
					abs(article_touch_down_pos.y - ev->touch_event.y) > 150)
                                display_article_with_pcf_smooth(LCD_HEIGHT_LINES);
			else if (article_touch_down_pos.y < ev->touch_event.y &&
					abs(article_touch_down_pos.y - ev->touch_event.y) > 150)
                                display_article_with_pcf_smooth(-LCD_HEIGHT_LINES);
                        else if(abs(article_touch_down_pos.y - ev->touch_event.y) > 10)
                        {
                              if(article_touch_down_pos.y<ev->touch_event.y)
                                 display_article_with_pcf_smooth(article_touch_down_pos.y-ev->touch_event.y-speed*2);
                              else
                                 display_article_with_pcf_smooth(article_touch_down_pos.y-ev->touch_event.y);

                        }*/
                        
				
			article_touch_down_handled = 0;
		} else {
                        article_offset = 0;
                        //if(abs(touch_y_last_article-ev->touch_event.y)>=5)
                        {
			   last_article_move_time = get_time();
		           touch_y_last_article = ev->touch_event.y;

                           if(article_touch_count>=10)
                               article_touch_count = 0;
                           touch_y_last_article_list[article_touch_count] = ev->touch_event.y;
                           touch_time_last_article_list[article_touch_count] = last_article_move_time;

                           article_touch_count++;
                           if(article_touch_count>=10)
                              article_touch_count = 0;
                        }
                        if(touch_y_last_unreleased == 0)
                        {
                           touch_y_last_unreleased = ev->touch_event.y;
                           start_move_time = get_time();
                        }
                        else if(abs(touch_y_last_unreleased - ev->touch_event.y) >=article_scroll_pixel)
                        {
                              display_article_with_pcf(touch_y_last_unreleased - ev->touch_event.y);
                              touch_y_last_unreleased = ev->touch_event.y;
                              article_moved = true;
                              article_scroll_pixel = 1;
                        }
                        if(article_moved)
                           goto out;

                        {
			      article_link_number =isArticleLinkSelected(ev->touch_event.x,ev->touch_event.y);
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
        int sleep,time_now;
        struct wl_input_event ev;


	/*
	 * test searching code...
	 */
	article_buf_pointer = NULL;
	search_init();
	history_list_init();
	malloc_status_simple();
	print_intro();
#ifndef INCLUDED_FROM_KERNEL
	if (!load_init_article(idx_init_article))
	{
		display_mode = DISPLAY_MODE_ARTICLE;
		last_display_mode = DISPLAY_MODE_INDEX;
	}
#endif
	render_string(SUBTITLE_FONT_IDX, -1, 55, MESSAGE_TYPE_A_WORD, strlen(MESSAGE_TYPE_A_WORD));

	for (;;) {
                sleep = 1;
                if(render_article_with_pcf())
			sleep = 0;
                if (article_offset>0)
                {
			scroll_article();
			sleep = 0;
                }

                #ifdef INCLUDED_FROM_KERNEL
		/*if (display_mode==DISPLAY_MODE_INDEX && search_string_changed)
                {
                   sleep = 0;
                   delay_us(10000);
                   time_now = get_time();
                   if((time_now-time_search_last)>SEARCH_FETCH_DELAY)
                   {
                       time_search_last = time_now;
                       search_fetch();

                       guilib_fb_lock();
                       search_result_display();
                       keyboard_paint();
                       guilib_fb_unlock();

                       search_string_changed = false;
                   }
                }*/

               if(display_mode == DISPLAY_MODE_INDEX)
                {
                    if (press_delete_button && get_search_string_len()>0)
                    {
	                 sleep = 0;
	                 time_now = get_time();
	                 if((time_now-start_search_time)>1000000*12*7)
	                 {
	                     if (!clear_search_string())
	                     {
	                     	search_string_changed_remove = true;
	                     	search_reload_ex(SEARCH_RELOAD_NORMAL);
	                     }
	                     press_delete_button = false;
	                 }
	                 else if ((time_now-start_search_time)>1000000*12 && (time_now-last_delete_time)>1000000*7)
	                 {
	                     if (!search_remove_char(0))
	                     {
		                     search_string_changed_remove = true;
		                     search_reload_ex(SEARCH_RELOAD_NO_POPULATE);
		             }
	                     last_delete_time = time_now;
	                 }
	            }
	            
                }
                #endif

                if (display_mode == DISPLAY_MODE_INDEX && fetch_search_result(0, 0, 0))
	        {
	            sleep = 0;
                }	            	

		if (sleep)
			history_list_save();
                
		if (keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_CHECK)) // reset invert is timeout
                	sleep = 0;

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

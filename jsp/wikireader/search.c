/*
 * binary search 
 * (C) Copyright 2008 OpenMoko, Inc.
 * Author: xiangfu liu <xiangfu@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <t_services.h>
#include <string.h>
#include <wikilib.h>
#include <guilib.h>
#include <file-io.h>
#include <input.h>                                                              
#include <msg.h>  
#include "search.h"

int file_object;
char g_key[TITLECHARS];
int g_key_count = 0;

/*
 * split the line with split_char to get title and hash
 * */
int split(char *source, char *word, char *sha1, char split_char)
{
	if (*source == 0 )
		return -1;

	while (*source != split_char)
                *(word++) = *(source++);
	*word='\0';

	source++;
        while (*source != 0)
                *(sha1++) = *(source++);
	*sha1='\0';

        return 0;
}

int scomp(const void *p, const void *q )
{
	return strcmp( (char*) p, (char*) q );
}

int display_array(char array[][LINECHARS], int n)
{
	int i = 0;
	while (i < n - 2 && array[i][0] != '\0') {
		msg(MSG_INFO,"%s", array[i]);
		i++;
	}
	msg(MSG_INFO, "---------------------\n");
	return 0;
}

void init_g_result()
{
	int i=0;
	for (i=0; i<RESULTCOUNT; i++) 
		g_result[i][0] = '\0';
}
/*
 * the fprt is not at the begin of the line
 * the function get the whole line
 * */
int get_line_from_pos(int fp, long pos, char *line, int length)
{
	char c = '\0';

	wl_seek(fp, pos);
	wl_read(fp, &c, 1);
	while (c != '\n') {
		pos -= 2;
		if (pos <= 0) {
			pos = 0;
			wl_seek(fp, pos);
			wl_fgets(line, length, fp);
			break;
		}

		wl_seek(fp, pos);
		wl_read(fp, &c, 1);
	}
	wl_fgets(line, length, fp);

	return 0;
}

int get_char_offset(char c) 
{
	int i = 0;
	for (i = 0; i < 36; i++) {
		if (g_offset_char[i] > c)
			break;
	}
	msg(MSG_INFO, "offset index is:%d", i);
	return i;
}
/*
 * return the file's fptr value -1 mean not found
 * */
int binary_search (int fp, char *key)
{
	char line[LINECHARS], title[TITLECHARS], hash[SHA1CHARS];

	int offset_index = get_char_offset(key[0]);
	long left = g_offset[offset_index];
	long right = g_offset[offset_index + 1];
	msg(MSG_INFO, "left is:%d\t right is:%d", left, right);
	long middle;
	int comp;

	if (wl_seek(fp, 0) != 0) {
		msg(MSG_INFO, "wl_seek to 0 error");
		return -1;
	}

	while (left <= right) { 
		middle = (left + right) / 2;

		if (wl_seek(fp, middle) != 0) {
			msg(MSG_INFO, "wl_seek to %d error", middle);
			return -1;
		}
		get_line_from_pos(fp, middle, line, LINECHARS);

		split(line, title, hash, '-');
		comp = scomp(key, title);
		if (comp == 0) {
			return middle;
		}
		if (comp > 0)
			left = middle + 1;
		else 
			right = middle - 1;
	}
	return -1;
}
/*
 * the linear seach is in this function
 * */
char ** lookup(char *key, char *p_hash)
{
	char line[LINECHARS], title[TITLECHARS], hash[SHA1CHARS];
	int rt = 0;
	switch (g_algorithm) {
	case 'L': /* here is linear search */
		while (wl_eof(file_object)) {
			if (wl_fgets(line, LINECHARS, file_object) != NULL) {
				split(line, title, hash, '-');
				int comp = scomp(key, title);
				if (comp == 0) {
					rt = wl_ftell(file_object);
					int comp_hash = scomp(p_hash, hash); 
					comp_hash ? msg(MSG_INFO, "%s\n%s", p_hash, hash)
						: msg(MSG_INFO, "true\n");
					break;
				}
			}
		}
		msg(MSG_INFO, "linear search result:%d", rt);

#if 1	
		strcpy(g_result[0], line);
		char line_temp[LINECHARS];
		int k = 0;
		for (k = 1; k< RESULTCOUNT; k++) {
			if (wl_fgets(line_temp, LINECHARS, file_object) != NULL) {
				strcpy(g_result[k], line_temp);
			} else {
				break;
			}
		}
#endif
		break;
	case 'B':
		rt = binary_search(file_object, key);
		msg(MSG_INFO, "binary search result:%d", rt);
		break;
	default :
		msg(MSG_INFO, "not select algorithm");
		break;
	}

	return (char **)g_result;
}
/*
 * set the search title from the serial input
 * */
int set_key_and_search(char c)
{
	if (c != SERAIL_ENTER) {
		g_key[g_key_count] = c;
		g_key_count ++;
		msg(MSG_INFO,"key is:%s", g_key);
		return 1;
	}

	init_g_result();

	SYSTIM begin_time;
	SYSTIM end_time;
	get_tim(&begin_time);
	lookup(g_key, "\0");
	get_tim(&end_time);
	msg(MSG_INFO, "search time is: %d", end_time - begin_time);

	display_array(g_result, RESULTCOUNT);

	int i = 0;
	for (i=0; i< TITLECHARS; i++)
		g_key[i] = '\0';
	g_key_count = 0;

	msg(MSG_INFO,"Done! Enter Title:");
	return 0;
}

int time_test()
{
	SYSTIM  start_time_1;
	SYSTIM  start_time_2;
	SYSTIM  stop_time_1;
	SYSTIM  stop_time_2;
	int i = 0;

	get_tim(&start_time_1);
	for (i = 0; i < g_titles_count; i++) {
		msg(MSG_INFO, "title is:%s", g_titles[i]);
		get_tim(&start_time_2);
		lookup(g_titles[i], g_hash[i]);
		get_tim(&stop_time_2);
		msg(MSG_INFO, "search time is:%d\n", stop_time_2 - start_time_2);
	}
	get_tim(&stop_time_1);
	msg(MSG_INFO, "time all is:%d", stop_time_1 - start_time_1);

	return 0;
}

/*
 * this is some kind of main() for the search.c
 * */
int search()
{
	/* wikilib_init(); */
	/* guilib_init(); */
	/* wikilib_run(); */

	ena_tex();
	while (1) {
		/*
		 * get command for the serial
		 */
		struct wl_input_event ev; 
                wl_input_wait(&ev);                                             
                msg(MSG_INFO, "%s() got key: %c", __func__, ev.val_a);   

		char c = ev.val_a;
		switch (c) {
		case 'H': {
			msg(MSG_INFO, "comand 'H':  display help");
			msg(MSG_INFO, "comand 'E':  exit task");
			msg(MSG_INFO, "comand 'L':  linear search");
			msg(MSG_INFO, "comand 'B':  binary search");
			msg(MSG_INFO, "comand 'T':  get the time of search all 60 titles");
			msg(MSG_INFO, "comand 'D':  test read");
			msg(MSG_INFO, "comand 'F':  test fgets");
			msg(MSG_INFO, "comand 'C':  test the lookup function");
			break;
		}
			/* test f_open and f_read function */
		case 'D': {
			char tmp[512];
			int n, total = 0;

			file_object = wl_open("/foo", WL_O_RDONLY);
			msg(MSG_INFO, "wl_open result = %d", file_object);

			msg(MSG_INFO, "benchmark starting ...");
			do {
				n = wl_read (file_object, tmp, sizeof(tmp));
				msg(MSG_INFO, "wl_read bytes: %d", n);
				total += n;
			} while (n == sizeof(tmp));
			wl_close(file_object);

			msg(MSG_INFO, "done. %d bytes read", total);
       			break; 
		}
			/* test fgets function  */
		case 'F': {
			file_object = wl_open("/foo", WL_O_RDONLY);
			msg(MSG_INFO, "wl_open result = %d", file_object);

			char line[LINECHARS], title[TITLECHARS], hash[SHA1CHARS];
			msg(MSG_INFO, "test fgets function");
			while (wl_eof(file_object)) {
				if (wl_fgets(line, LINECHARS, file_object) != NULL) {
					split(line, title, hash, '-');
					msg(MSG_INFO, "line:%s", line);
					msg(MSG_INFO, "title:%s", title);
					msg(MSG_INFO, "hash:%s", hash);
				}
			}
			wl_close(file_object);
			msg(MSG_INFO, "test over!");
       			break; 
		}
		case 'E':
			msg(MSG_INFO, "exit search function.");
			return 0;
			/*
			 * set whick index file use
			 * */
		case 'T':
			file_object = wl_open("/index", WL_O_RDONLY);
			msg(MSG_INFO, "index f_open result = %d", file_object);
			time_test();
			wl_close(file_object);
			break;
			/*
			 * test the lookup function
			 * */
		case 'C':
			init_g_result();
			file_object = wl_open("/foo", WL_O_RDONLY);
			msg(MSG_INFO, "index f_open result = %d", file_object);
			lookup("ou", "4d9ccb5331dea3aab31487e513c2fe11c46055da");
			display_array(g_result, RESULTCOUNT);
			wl_close(file_object);
			break;
			/* switch the algorithms */
		case 'L':
		case 'B':
			g_algorithm = c;
			msg(MSG_INFO, "algorithm is %c", g_algorithm);
			break;
			/*
			 * set the search title must lowcase work
			 * */
		default:
			set_key_and_search(c);
			break;
		}
	}

	return 0;
}

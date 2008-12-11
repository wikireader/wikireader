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
#include <tff.h>
#include <string.h>
#include <wikilib.h>
#include <guilib.h>
#include "search.h"
#include "sample1.h"

#if 0
FIL file_object;
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
		syslog(LOG_INFO,"%s", array[i]);
		i++;
	}
	syslog(LOG_INFO, "---------------------\n");
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
int get_line_from_pos(FIL *fp, long pos, char *line, int length)
{
	char c = '\0';
	f_lseek(fp, pos);
	int temp = 0;
	f_read(fp, &c, 1, &temp);
	while (c != '\n') {
		pos -= 2;
		if (pos <= 0) {
			pos = 0;
			f_lseek(fp, pos);
			fgets(line, length, fp);
			break;
		}

		f_lseek(fp, pos);
		f_read(fp, &c, 1, &temp);
	}
	fgets(line, length, fp);

	return 0;
}
/*
 * return the file's fptr value -1 mean not found
 * */
int binary_search (FIL *fp, char *key)
{
	char line[LINECHARS], title[TITLECHARS], hash[SHA1CHARS];
	long left = 0;
	long right = (*fp).fsize;
	long middle;
	int comp;

	f_lseek(fp, 0);
	while (left <= right) { 
		middle = (left + right) / 2;

		f_lseek(fp, middle);
		get_line_from_pos(fp, middle, line, LINECHARS);

		split(line, title, hash, '-');
		comp = scomp(key, title);
		if (comp == 0) {
			strcpy(g_result[0], line);
			int i = 1;
			for (i = 1; i < RESULTCOUNT; i++) {
				fgets(line, LINECHARS, fp);
				strcpy(g_result[i], line);
			}
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
	int k = 0, rt = 0;
	switch (g_algorithm) {
	case 'L': /* here is linear search */
		while (file_object.fptr < file_object.fsize) {
			if (fgets( line, LINECHARS, &file_object) != 0) {
				split(line, title, hash, '-');
				int comp = scomp(key, title);
				if (comp == 0) {
					rt = file_object.fptr;
					int comp_hash = scomp(p_hash, hash); 
					comp_hash ? syslog(LOG_INFO, "%s\n%s", p_hash, hash)
						: syslog(LOG_INFO, "true\n");
					break;
				}
			}
		}
		syslog(LOG_INFO, "linear search result:%d", rt);

		strcpy(g_result[0], line);
		char line_temp[LINECHARS];
		for (k = 1; k< RESULTCOUNT; k++) {
			if (fgets(line_temp, LINECHARS, &file_object) != NULL) {
				strcpy(g_result[k], line_temp);
			} else {
				break;
			}
		}
		break;
	case 'B':
		rt = binary_search(&file_object, key);
		syslog(LOG_INFO, "binary search result:%d", rt);
		break;
	default :
		syslog(LOG_INFO, "not select algorithm");
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
		syslog(LOG_INFO,"key is:%s", g_key);
		return 1;
	}

	init_g_result();

	SYSTIM begin_time;
	SYSTIM end_time;
	get_tim(&begin_time);
	lookup(g_key, "\0");
	get_tim(&end_time);
	syslog(LOG_INFO, "search time is: %d", end_time - begin_time);

	display_array(g_result, RESULTCOUNT);

	int i = 0;
	for (i=0; i< TITLECHARS; i++)
		g_key[i] = '\0';
	g_key_count = 0;

	syslog(LOG_INFO,"Done! Enter Title:");
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
		syslog(LOG_INFO, "title is:%s", g_titles[i]);
		get_tim(&start_time_2);
		lookup(g_titles[i], g_hash[i]);
		get_tim(&stop_time_2);
		syslog(LOG_INFO, "search time is:%d", stop_time_2 - start_time_2);
	}
	get_tim(&stop_time_1);
	syslog(LOG_INFO, "time all is:%d", stop_time_1 - start_time_1);

	return 0;
}
#endif

/*
 * this is some kind of main() for the search.c
 * */
int search()
{
	char c = 'H';
        FRESULT result;

	wikilib_init();
	guilib_init();
	wikilib_run();

#if 0
	ena_tex();
	while (1) {
		/*
		 * get command for the serial
		 */
		syscall(serial_rea_dat(TASK_PORTID, &c, 1));
		syslog(LOG_INFO, "%c", c);
		switch (c) {
		case 'H': {
			syslog(LOG_INFO, "comand 'D':  test read");
			syslog(LOG_INFO, "comand 'F':  test fgets");
			syslog(LOG_INFO, "comand 'E':  exit task");
			syslog(LOG_INFO, "comand 'S':  test index");
			syslog(LOG_INFO, "comand 'T':  get the time of search all 60 titles");
			syslog(LOG_INFO, "comand 'C':  test the lookup function");
			syslog(LOG_INFO, "comand 'L':  linear search");
			syslog(LOG_INFO, "comand 'B':  binary search");
			syslog(LOG_INFO, "comand 'H':  display help");
			break;
		}
			/* test f_open and f_read function */
		case 'D': {
			char tmp[512];
			int n, total = 0;

			result = f_open(&file_object, "/foo", FA_READ);
			syslog(LOG_INFO, "f_open result = %d", result);
			if (result != 0)
				break;

			syslog(LOG_INFO, "benchmark starting ...");
			do {
				result = f_read (&file_object, tmp, sizeof(tmp), &n);
				syslog(LOG_INFO, "f_read result = %d, n = %d", result, n);
				total += n;
			} while (result == 0 && n == sizeof(tmp));
			f_close(&file_object);

			syslog(LOG_INFO, "done. %d bytes read", total);
       			break; 
		}
			/* test fgets function  */
		case 'F': {
			result = f_open(&file_object, "/foo", FA_READ);
			syslog(LOG_INFO, "f_open result = %d", result);
			if (result != 0)
				break;
			char line[LINECHARS], title[TITLECHARS], hash[SHA1CHARS];
			syslog(LOG_INFO, "test fgets function");
			while (file_object.fptr < file_object.fsize) {
				if (fgets( line, LINECHARS, &file_object) != NULL) {
					split(line, title, hash, '-');
					syslog(LOG_INFO, "line:%s", line);
					syslog(LOG_INFO, "title:%s", title);
					syslog(LOG_INFO, "hash:%s", hash);
				}
			}
			f_close(&file_object);
			syslog(LOG_INFO, "test over!");
       			break; 
		}
		case 'E':
			syslog(LOG_INFO, "exit search function.");
			return 0;
			/*
			 * set whick index file use
			 * */
		case 'S':
			result = f_open(&file_object, "/index", FA_READ);
			syslog(LOG_INFO, "index f_open result = %d", result);
			break;
		case 'T':
			time_test();
			f_close(&file_object);
			break;
			/*
			 * test the lookup function
			 * */
		case 'C':
			init_g_result();
			lookup("ou", "4d9ccb5331dea3aab31487e513c2fe11c46055da");
			display_array(g_result, RESULTCOUNT);
			f_close(&file_object);
			break;
			/* switch the algorithms */
		case 'L':
		case 'B':
			g_algorithm = c;
			syslog(LOG_INFO, "algorithm is %c", g_algorithm);
			break;
			/*
			 * set the search title must lowcase work
			 * */
		default:
			set_key_and_search(c);
			break;
		}
	}
#endif

	return 0;
}

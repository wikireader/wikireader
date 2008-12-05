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
#include <stdio.h>
#include <string.h>
#include "search.h"
#include "sample1.h"

#define TITLECOUNT 60 	/* line count of the index file */
#define SHA1CHARS 40		/* sha1 char count */
#define TITLECHARS 40		/* the max chars of the title */
#define LINECHARS 80
#define RESULTCOUNT 10

int g_titles_count = 0; 
int g_result_index = 0;	/* use this store the result index */
char g_titles[TITLECOUNT][TITLECHARS];
char g_hash[TITLECOUNT][SHA1CHARS];
char g_result[RESULTCOUNT][TITLECHARS];
char g_key[TITLECHARS];
int g_key_count = 0;

char g_algorithm = 'L';
char g_line_temp[LINECHARS], g_title_temp[TITLECHARS], g_hash_temp[SHA1CHARS];
int g_titles_temp_count = 60;
char *g_titles_temp[]= {
	". .\0",
	"1\0",
	"2 am club\0",
	"3 car garage\0",
	"4 aquilae\0",
	"5 alive\0",
	"6 april\0",
	"7 as\0",
	"8 ball\0",
	"9 brigade\0",
	"0 k\0",
	"a 44\0",
	"bch formula\0",
	"bchb\0",
	"bche\0",
	"bchl\0",
	"bchs\0",
	"bchydro\0",
	"d arae\0",
	"e boats\0"
	"egf\0",
	"egfa\0",
	"egfe\0",
	"egff\0",
	"egfh\0",
	"egfl7\0",
	"egfp\0",
	"egfr\0",
	"egg project\0",
	"eggd\0",
	"eggp\0",
	"eggr 239\0",
	"eggr 265\0",
	"eggr 572\0",
	"f w olin\0",
	"f w olin foundation\0",
	"f w pomeroy\0",
	"f w s craig\0",
	"george a. loud\0",
	"octavian paler\0",
	"ou\0",
	"ou andromedae\0",
	"ou center for public management\0",
	"ou chant\0",
	"ou sooners\0",
	"ou band\0",
	"ou812\0",
	"ou812 tour\0",
	"oua\0",
	"ouabc\0",
	"ouac\0",
	"ouat\0",
	"ouatic\0",
	"ouatitw\0",
	"yzosse\0",
	"z\0",
	"z\0",
	"zyzzyxdonta\0",
	"zyzzyxdonta alata\0",
	"zyzzyzus\0"
	};

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

int display_array(char ** array, int n)
{
	syslog(LOG_INFO, "----------begin dump-----------");
	int i=0;
	while (i<n && array[i][0] != '\0') {
		syslog(LOG_INFO, "%s\n", array[i]);
		i++;
	}
	syslog(LOG_INFO, "----------dump over------count: %d", i);
	return 0;
}

void init_g_result()
{
	int i=0;
	for (i=0; i<RESULTCOUNT; i++) 
		g_result[i][0] = '\0';
}

int binary_search (char *array[], int size, char *key)
{
	int left = 0;
	int right = size - 1;
	int middle, comp;
	while (left <= right) { 
		middle = (left + right) / 2;
		comp = scomp(key, array[middle]);
		if (comp == 0) 
			return middle;
		if (comp > 0)
			left = middle + 1;
		else 
			right = middle - 1;
	}
	return -1;
}

int linear_search (char *array[], int size, char *key)
{
	int i=0;
	for (i = 0; i < size; i++)
		if (scomp(array[i], key) >=0)
			return i;
	return -1;
}

char ** lookup(char *key)
{
	int count = 0;
	int index = 1;
	switch (g_algorithm) {
	case 'L':
		syslog(LOG_INFO, "algorithm is linear search");
		index = linear_search(g_titles_temp, g_titles_temp_count, key);
		break;
	case 'B':
		syslog(LOG_INFO, "algorithm is binary search");
		index = binary_search(g_titles_temp, g_titles_temp_count, key);
		break;
	default :
		syslog(LOG_INFO, "not select algorithm");
		break;
	}
	g_result_index = index;
	/* syslog(LOG_INFO, "g_result_index is = %d",g_result_index); */

	int i = 0;
	while (i<RESULTCOUNT && g_titles_temp[index] != NULL) {
		strcpy(g_result[i++], g_titles_temp[index++]);
	}
	return g_result;
}

int search_start(char *fname)
{
	FIL file_object;
	FRESULT result;
	SYSTIM begin_time;
	SYSTIM end_time;

	result = f_open(&file_object, fname, FA_READ);
	syslog(LOG_INFO, "%s FIL name is:%s\t f_open result = %d",__func__, fname, result);
	if (result != 0)
		return -1;

	syslog(LOG_INFO, "%s search starting ...", __func__);
	get_tim(&begin_time);
	g_titles_count = 0;

	int i;
	for (i=0; i < LINECHARS; i++)
		g_line_temp[i] = 0;
	while (fgets(g_line_temp, LINECHARS, &file_object) != 0) {
		int k = 0;
		for (k = 0;k<500000; k++);
		syslog(LOG_INFO, "%s lines:%d\t%s", __func__, 1 + g_titles_count, g_line_temp);

/*		split(g_line_temp, g_title_temp, g_hash_temp, SPLIT_CHAR);

		syslog(LOG_INFO, "%s\n%s\n%s", g_line_temp, g_title_temp, g_hash_temp);

		strcpy(g_titles[g_titles_count], g_title_temp);
		strcpy(g_hash[g_titles_count], g_hash_temp);*/
		g_titles_count ++;
		for (i=0; i < LINECHARS; i++)
			g_line_temp[i] = 0;
	}
	get_tim(&end_time);
	syslog(LOG_INFO, "%s read time is:%d", __func__, end_time - begin_time);

	f_close(&file_object);

/*	display_array(g_hash, g_titles_count);
	display_array(g_titles, g_titles_count);
	syslog(LOG_INFO,"%s Enter title:", __func__);*/
	return 0;
}

int set_key_and_search(char c)
{
	if (c != SERAIL_ENTER) {
		g_key[g_key_count] = c;
		g_key_count ++;
		syslog(LOG_INFO,"key is:%s", g_key);
		return 1;
	}
	syslog(LOG_INFO,"the search title is:%s", g_key);

	init_g_result();

	SYSTIM begin_time;
	SYSTIM end_time;
	get_tim(&begin_time);
	lookup(g_key);
	get_tim(&end_time);
	syslog(LOG_INFO, "time is :%d", end_time - begin_time);

	int i = 0;
	while (g_titles_temp[g_result_index][0] != '\0' && i<RESULTCOUNT) {
		syslog(LOG_INFO, "%d\t%s", i+1, g_titles_temp[g_result_index]);
		g_result_index++;
		i++;
	}

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
	SYSTIM  start_time_3;
	SYSTIM  stop_time_1;
	SYSTIM  stop_time_2;
	SYSTIM  stop_time_3;
	int i = 0;
	char t[2];
	t[0] = t[1] = '\0';
	get_tim(&start_time_1);
	for (i = 0; i < g_titles_temp_count; i++) {
		char *title = g_titles_temp[i];
		syslog(LOG_INFO, "time 2 title is:%s", title);
		get_tim(&start_time_2);
		t[0] = *title++;
		while (t[0] != NULL){
			get_tim(&start_time_3);
			lookup(t);
			get_tim(&stop_time_3);
			syslog(LOG_INFO, "time_3 is:%d", stop_time_3 - start_time_3);
			t[0] = *title++;
		}
		get_tim(&stop_time_2);
		syslog(LOG_INFO, "time_2 over value is:%d", stop_time_2 - start_time_2);
	}
	get_tim(&stop_time_1);
	syslog(LOG_INFO, "time_all is:%d", stop_time_1 - start_time_1);
}

int search()
{
	char c = 'H';
        FRESULT result;

	ena_tex();
	while (1) {
		syscall(serial_rea_dat(TASK_PORTID, &c, 1));
		syslog(LOG_INFO, "%c", c);
		switch (c) {
		case 'H': {
			syslog(LOG_INFO, "search task starts.");
			syslog(LOG_INFO, "comand 'D':  test read");
			syslog(LOG_INFO, "comand 'E':  exit task");
			syslog(LOG_INFO, "comand 'F':  test index60");
			syslog(LOG_INFO, "comand 'A':  test index1");
			syslog(LOG_INFO, "comand 'B':  test index2");
			syslog(LOG_INFO, "comand 'C':  test index5");
			syslog(LOG_INFO, "comand 'H':  display help");
			syslog(LOG_INFO, "lowcase lettle:  title");
			break;
		}
		case 'D': {
			FIL file_object;
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

			syslog(LOG_INFO, "done. %d bytes read\n", total);
       			break; 
		}
		case 'E':
			syslog(LOG_INFO, "exit search function.");
			return 0;
		case 'F':
			search_start("/index60");
			break;
		case 'A':
			search_start("/index2");
			break;
		case 'C':
			search_start("/index5");
			break;
		case 'T':
			time_test();
			break;
		case 'L':
			g_algorithm = 'L';
			break;
		case 'B':
			g_algorithm = 'B';
			break;
		default:
			set_key_and_search(c);
			break;
		}
	}
	return 0;
}

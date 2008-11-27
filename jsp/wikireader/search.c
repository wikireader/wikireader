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
#include "search.h"
#include "sample1.h"
#include <tff.h>
#include <stdio.h>
#include <string.h>

#define TITLECOUNT 60 	/* line count of the index file */
#define SHA1CHARS 40		/* sha1 char count */
#define TITLECHARS 40		/* the max chars of the title */
#define LINECHARS 80
#define RESULTCOUNT 10

int g_titles_count = 0; 
int g_result_index = -1;	/* use this store the result index. not value */
char g_titles[TITLECOUNT][TITLECHARS];
char g_hash[TITLECOUNT][SHA1CHARS];
char g_result[RESULTCOUNT][TITLECHARS];
char g_key[TITLECHARS];

int split(char *source, char *word, char *sha1, char split_char)
{
	if (*source == 0 )
		return -1;

	if (*source == '\n') 
		return -1;

	char *temp = source;
	int position = 0;
	for ( ; *temp != 0; temp++) {
		if (*temp == SPLIT_CHAR)
			break;
		position++;
	}

	int i=0;
	for(i = 0; i <= position; i++){
                *(word++) = *(source++);
	}
	*word='\0';
	source++;
        while (*source != '\n')
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

int binary_search (char *array[], int low, int high, char *key, int *count)
{
	if (low>=high) {
		return -1;
	} else {
		int mid = (low + high)/2;
		*count = *count + 1;
		int comp = scomp(key, array[mid]);
		if (comp == 0)
			return mid;
		else if (comp < 0) {
			return binary_search(array, low, mid-1, key, count);
		}
		else {
			return binary_search(array, mid+1, high, key, count);
		}
	}
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
	int index = linear_search(g_titles, g_titles_count, key);
	g_result_index = index;
	int i = 0;
	while (i<RESULTCOUNT && g_titles[index] != NULL) {
		strcpy(g_result[i++], g_titles[index++]);
	}
	return g_result;
}

int getline(char *des, int des_length, FIL *file)
{
	char *tmp;
	int n, result;
	int count = 0;

	while (count <des_length) {
		result = f_read (file, tmp, sizeof(char), &n);

		if (result != 0)
			return 0;
		if (n != 1)
			return 0;

		syslog(LOG_INFO, "getline() char is :%c", *tmp);
		if (*tmp == '\n') {
			des[count] = '\0';
			syslog(LOG_INFO, "%d\tgetline() gets des:%s", count, &des[0]);
			return count;	
		}

		des[count++] = *tmp;
	}
	return 0;
}

int search_start(char *fname)
{
	FIL file_object;
	FRESULT result;
	SYSTIM begin_time;
	SYSTIM end_time;

	char line[LINECHARS], title[TITLECHARS], hash[SHA1CHARS];

	result = f_open(&file_object, fname, FA_READ);
	syslog(LOG_INFO, "search_start() FIL name is:%s\t f_open result = %d",fname, result);
	if (result != 0)
		return -1;

	syslog(LOG_INFO, "search_start() benchmark search starting ...");
	get_tim(&begin_time);
	g_titles_count = 0;
	while (getline(line, LINECHARS, &file_object) != 0) {
		split(line, title, hash, SPLIT_CHAR);
		strcpy(g_titles[g_titles_count], title);
		strcpy(g_hash[g_titles_count], hash);
		syslog(LOG_INFO, "search_start() read lines: %d", g_titles_count++);
		syslog(LOG_INFO, "search_start() line:--%s--",line); 
		syslog(LOG_INFO, "search_start() title:--%s--",title); 
		syslog(LOG_INFO, "search_start() hash:--%s--",hash); 
	}
	get_tim(&end_time);
	syslog(LOG_INFO, "search_start() read time is :%d", end_time - begin_time);

	display_array(g_hash, g_titles_count);
	display_array(g_titles, g_titles_count);
	syslog(LOG_INFO,"search_start() Enter title:");
	return 0;
}
int set_key_and_search(char c)
{
	SYSTIM begin_time;
	SYSTIM end_time;
	int count = 0;
	if ( c != '\n'){
		g_key[count] = c;
		count ++;
	}
	g_key[count] = '\0';
	count = 0;

	init_g_result();

	get_tim(&begin_time);
	lookup(g_key);
	get_tim(&end_time);

	syslog(LOG_INFO, "time is :%d\n", end_time - begin_time);

	int i = 0;
	while (g_titles[g_result_index][0] != '\0' && i<RESULTCOUNT) {
		syslog(LOG_INFO, "%d\t%s---%s\n", i+1, g_titles[g_result_index], 
				       g_hash[g_result_index]);
		g_result_index++;
		i++;
	}

	syslog(LOG_INFO,"Done!\n");
	return 0;
}

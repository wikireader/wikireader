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
int g_result_index = 0;	/* use this store the result index */
char g_titles[TITLECOUNT][TITLECHARS];
char g_hash[TITLECOUNT][SHA1CHARS];
char g_result[RESULTCOUNT][TITLECHARS];
char g_key[TITLECHARS];
int g_key_count = 0;

char g_line_temp[LINECHARS], g_title_temp[TITLECHARS], g_hash_temp[SHA1CHARS];

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

int scomp_binary_search(const char *s1, const char *s2)
{
    while (*s1++ == *s2)
        if (*s2++ == 0)
            return (0);
    return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
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
		int comp = scomp_binary_search(array[mid], key);
		if (comp >= 0)
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

char* fgets (                                                                                  
        char* buff,     /* Pointer to the string buffer to read */                             
        int len,        /* Size of string buffer */                                            
        FIL* fil        /* Pointer to the file object */                                       
	)                                                                                              
{                                                                                              
        int i = 0;                                                                             
        char *p = buff;                                                                        
        UINT rc;                                                                               
	int k = 0;
                                                                                               
        while (i < len - 1) {                   /* Read bytes until buffer gets filled */      
                f_read(fil, p, 1, &rc);                                                        
		for (k = 0; k < 50000; k++);
                if (rc != 1) break;                     /* Break when no data to read */       

                if (*p == '\r') continue;       /* Strip '\r' */                               

                i++;                                                                           
		syslog(LOG_INFO, "char num:%d\tchar:%c", i, *p);
		syslog(LOG_INFO, "bufferis:%s", buff);
                if (*p++ == '\n') {
			p--;
			break;        /* Break when reached end of line */           
		}
        }                                                                                      
        *p = 0;                                                                                
	syslog(LOG_INFO, "%s", buff);
        return i ? buff : 0;
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
	g_key[g_key_count] = '\0';
	g_key_count = 0;
	syslog(LOG_INFO,"key is:%s", g_key);

	init_g_result();

	SYSTIM begin_time;
	SYSTIM end_time;
	get_tim(&begin_time);
	lookup(g_key);
	get_tim(&end_time);

	syslog(LOG_INFO, "time is :%d", end_time - begin_time);

	int i = 0;
	while (g_titles[g_result_index][0] != '\0' && i<RESULTCOUNT) {
		syslog(LOG_INFO, "%d\t%s---%s", i+1, g_titles[g_result_index], 
		       g_hash[g_result_index]);
		g_result_index++;
		i++;
	}
	for (i=0; i< TITLECHARS; i++)
		g_key[i] = '\0';
	syslog(LOG_INFO,"Done! Enter Title:");
	return 0;
}

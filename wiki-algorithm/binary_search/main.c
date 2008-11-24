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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAXWORDS 100 	/* line count of the index file */
#define SHA1CHARS 40		/* sha1 char count */
#define MAXCHARS 40		/* the max chars of the title */
#define LINECHARS 80
#define RESULTCOUNT 10

int	g_titles_count = 0; 
char *g_titles[MAXWORDS] , *g_hash[MAXWORDS], *g_result[RESULTCOUNT];

int split(char *source, char *word, char *sha1, char split_char)
{
	if(*source == 0){
		*word = 0;
		*sha1 = 0;
		return 0;
	}
	char *p = strrchr(source, split_char);
	int i=0;
	int split_char_pos = p - source;
	for(i = 0; i < split_char_pos; i++){
                *(word++) = *(source++);
	}
	*word='\0';
	source++;		/* eat the blank */
        while(*source != '\n' && *source != EOF)
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
	printf("----------begin dump-----------\n");
	int i=0;
	while (i<n && array[i] != NULL) {
		printf("%s\n", array[i]);
		i++;
	}
	printf("----------dump over------count: %d\n", i);
	return 0;
}

void init_g_result()
{
	int i=0;
	for (i=0; i<RESULTCOUNT; i++) 
		g_result[i] = NULL;
}

int binary_search (char *array[], int low, int high, char *key, int *count)
{
	if (low>=high) {
		return -1;
	} else {
		int mid = (low + high)/2;
		*count = *count + 1;
		int comp = scomp(key, array[mid]);
		if (comp = 0)
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
/*	int count = 0;
	int low = 0;
	int high = datasize-1;
	int query = binary_search(array, low, high, key, &count);
*/
	int index = linear_search(g_titles, g_titles_count, key);
	int i = 0;//index;
	while (i<RESULTCOUNT && g_titles[index] != NULL) {
		g_result[i] = malloc(MAXCHARS* sizeof(char));
		strcpy(g_result[i++], g_titles[index++]);
	}
	return g_result;
}

int main(int argc, char *argv[])
{
	int i=0, globalstarttime=0, n=0;
	char *fname;
	FILE *fp;
	if (argc != 2)/* no args */
		printf("one arg: file name \n");
	else /* at least one arg: file name */
		fname = argv[1];
	
	setbuf(stdout, 0);
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "  Can't open file\n");
		exit(1);
	}

	char *hash, *title;
	char line[LINECHARS];
	while (!feof(fp)){
		if (fgets(line, LINECHARS, fp) != NULL){
			title = (char *) malloc(MAXCHARS * sizeof(char));
			hash = (char *) malloc(SHA1CHARS * sizeof(char));
			split(line, title, hash, '-');
			g_titles[g_titles_count] = title;
			g_hash[g_titles_count] = hash;
			printf("read lines: %d\n", g_titles_count++);
		}
	}

	display_array(g_hash, g_titles_count);
	display_array(g_titles, g_titles_count);

	printf("Enter title:");
	while (scanf("%s", title) != EOF) {
		init_g_result();

		globalstarttime = clock();
		lookup(title);
		i = clock() - globalstarttime;
		printf("Total clicks\t%d\nTotal secs\t%4.3f\n",
		       i, (double) i / CLOCKS_PER_SEC);
		i=0;
		while (g_result[i] != NULL && i<RESULTCOUNT)
			printf("%d\t%s\n", i, g_result[i++]);

		printf("\nEnter title:");
	}

	return 0;
}


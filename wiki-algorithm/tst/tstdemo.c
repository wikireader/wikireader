/*
 * tst tree 
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

/* Ternary Search Trees 
 * Dr. Dobb's Journal April 1998 By Jon Bentley and Bob Sedgewick
 * Jon is a Member of Technical Staff at Bell Labs.
 * Bob is the William O. Baker Professor of Computer Science at
 * Princeton University. They can be reached at jlb@research.bell-labs.com
 * and rs@cs.princeton.edu, respectively.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* TERNARY SEARCH TREE ALGS */
#define MAX_LEN 100

typedef struct tnode *Tptr;
typedef struct tnode {
	char splitchar[MAX_LEN];
	char sha1[MAX_LEN];
	Tptr lokid, eqkid, hikid;
} Tnode;

Tptr root;
char *insertstr;

Tptr insert(Tptr *p, char *s, char *sha1) /* used */
{
	if (*p == 0) {
		*p = (Tptr) malloc(sizeof(Tnode));
		strcpy((*p)->splitchar, s);
		strcpy((*p)->sha1 , sha1);
		(*p)->lokid = (*p)->eqkid = (*p)->hikid = 0;
	}
	int cmp = strcmp(s , (*p)->splitchar);
	if ( cmp < 0 )
		(*p)->lokid = insert(&((*p)->lokid), s, sha1);
	else if (cmp == 0) {
/*		if (*s == 0)
			p->eqkid = (Tptr) insertstr;
		else
		p->eqkid = insert(p->eqkid, ++s); */
	} else
		(*p)->hikid = insert(&((*p)->hikid), s,sha1);
	return *p;
}

void cleanup(Tptr p)
{
	if (p) {
		cleanup(p->lokid);
		if (p->splitchar) cleanup(p->eqkid);
		cleanup(p->hikid);
		free(p);
	}
}


char* search(char *s) /* change for index */
{
	Tptr p;
	p = root;
	while (p) {
		int cmp = strcmp(s,p->splitchar);
		if (cmp < 0)
			p = p->lokid;
		else if (cmp == 0)  {
			return p->sha1;
		} else
			p = p->hikid;
	}
	return 0;
}

int split(char *source, char *word, char*sha1)/* used */
{
        while(*source != ' ')
                *(word++) = *(source++);
	*word='\0';
        while(*source != '\n')
                *(sha1++) = *(source++);
	*sha1='\0';
        return 0;
}

int createtst(char * fname)/* used */
{
	FILE *fp;
	char line[MAX_LEN];
	char word[MAX_LEN];
	char sha1[MAX_LEN];
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "  Can't open file\n");
		exit(1);
	}

	while (!feof(fp)){
		if (fgets(line, MAX_LEN, fp) != NULL){
			split(line, word, sha1);
			insert(&root, word, sha1);
		}
	}

	return 0;
}

int a(Tptr *p){
	*p = (Tptr) malloc(sizeof(Tnode)); 
	Tptr pp=*p;
	strcpy(pp->sha1,"asdf");
	return 0;
}

int main(int argc, char *argv[]) /* used */
{
/*	a(&root);
	printf("%s\n",root->sha1);*/
	createtst(argv[1]);
	char *s = search(argv[2]);
	if( s != 0){
		printf("%s\n",s);
		return 0;
	}
	printf("not find\n");
	return 1;
}

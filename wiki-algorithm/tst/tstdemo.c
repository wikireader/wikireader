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

/* tstdemo.c -- Demo program for ternary search trees
  This code is described in "Ternary Search Trees" by Jon
Bentley and Robert Sedgewick in the April, 1998, Dr. Dobb's Journal.
  Usage
	tstdemo dictfile		Basic experiments on dictionary
	tstdemo dictfile trysearch	Exercise search routines
	tstdemo dictfile traverse	Test tree traversal
  The first form runs a set of basic experiments comparing
ternary search trees to hash tables using the dictionary
in the file dictfile (one word per line).
  The second form allows one to conduct a sequence of searches.  
The input line	3 elephant reports words within Hamming distance 3 of 
"elephant", and -1 .i.i.i
reports partial matches (words whose 2nd, 4th and 6th letters are i).
  The third form tests the tree traversal routine by printing
the input file in sorted order.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* TERNARY SEARCH TREE ALGS */

typedef struct tnode *Tptr;
typedef struct tnode {
	char splitchar;
	Tptr lokid, eqkid, hikid;
} Tnode;
Tptr root;

char *insertstr, *insertsha1;

#define MAXWORDS 400000 	/* line count of the index file */
#define SHA1CHARS 40		/* sha1 char count */
#define MAXCHARS 40		/* the max chars of the title */
#define LINECHARS 80

int	n = 0, starttime,  rptctr;
char space[MAXCHARS], *a[MAXWORDS] , *address[MAXWORDS];

Tptr insert(Tptr p, char *s)
{   if (p == 0) {
        p = (Tptr) malloc(sizeof(Tnode));
        p->splitchar = *s;
        p->lokid = p->eqkid = p->hikid = 0;
    }
    if (*s < p->splitchar)
        p->lokid = insert(p->lokid, s);
    else if (*s == p->splitchar) {
        if (*s == 0)
            p->eqkid = (Tptr) insertstr;
		else
            p->eqkid = insert(p->eqkid, ++s);
    } else
        p->hikid = insert(p->hikid, s);
    return p;
}

void cleanup(Tptr p)
{   if (p) {
		cleanup(p->lokid);
		if (p->splitchar) cleanup(p->eqkid);
		cleanup(p->hikid);
		free(p);
    }
}

#define BUFSIZE 1000
Tptr buf;
int bufn, freen;
void * freearr[10000];

int split(char *source, char *word, char*sha1)
{
	if(*source == 0){
		*word = 0;
		*sha1 = 0;
		return 0;
	}
	char *p = strrchr(source, ' ');
	int i=0;
	int split_char_pos = p - source + 1;
	for(i = 0; i < split_char_pos; i++){
                *(word++) = *(source++);
	}
	*word='\0';

        while(*source != '\n' && *source != EOF)
                *(sha1++) = *(source++);
	*sha1='\0';

        return 0;
}

Tptr insert2(Tptr p, char *s)
{
	if (p == 0) {
		p = (Tptr) malloc(sizeof(Tnode));
/*		if (bufn-- == 0) {
			buf = (Tptr) malloc(BUFSIZE *
					    sizeof(Tnode));
			freearr[freen++] = (void *) buf;
			bufn = BUFSIZE-1;
		}
		p = buf++;
*/		p->splitchar = *s;
		p->lokid = p->eqkid = p->hikid = 0;
	}
	if (*s < p->splitchar)
		p->lokid = insert2(p->lokid, s);
	else if (*s == p->splitchar) {
		if (*s == 0){
			p->lokid = (Tptr) insertsha1;
			p->eqkid = (Tptr) insertstr; 
		}else{
			p->eqkid = insert2(p->eqkid, ++s);
		}
	} else
		p->hikid = insert2(p->hikid, s);
	return p;
}

void cleanup2()
{   int i;
	for (i = 0; i < freen; i++)
		free(freearr[i]);
}

void insert3(char *s)
{   int d;
	char *instr = s;
	Tptr pp, *p;
	p = &root;
	while (pp = *p) {
		if ((d = *s - pp->splitchar) == 0) {
			if (*s++ == 0) return;
			p = &(pp->eqkid);
		} else if (d < 0)
			p = &(pp->lokid);
		else
			p = &(pp->hikid);
	}
	for (;;) {
		/* *p = (Tptr) malloc(sizeof(Tnode)); */
		if (bufn-- == 0) {
			buf = (Tptr) malloc(BUFSIZE *
					    sizeof(Tnode));
			freearr[freen++] = (void *) buf;
			bufn = BUFSIZE-1;
		}
		*p = buf++;
		pp = *p;
		pp->splitchar = *s;
		pp->lokid = pp->eqkid = pp->hikid = 0;
		if (*s++ == 0) {
			pp->eqkid = (Tptr) instr;
			return;
		}
		p = &(pp->eqkid);
	}
}

char * search(char *s)
{   Tptr p;
	p = root;
	while (p) {
		if (*s < p->splitchar)
			p = p->lokid;
		else if (*s == p->splitchar)  {
/*			if (*s++ == 0)
			return 1;
*/
			if(*s == 0)
				return p->eqkid;
			p = p->eqkid;
		} else
			p = p->hikid;
	}
	return 0;
}

char *rsearch(Tptr p, char *s)
{
	if (!p) return 0;
	if (*s < p->splitchar)
		return rsearch(p->lokid, s);
	else if (*s > p->splitchar)
		return rsearch(p->hikid, s);
	else {
		if (*s == 0) return p->eqkid;
		return rsearch(p->eqkid, ++s);
	}
}

int rsearch2(Tptr p, char *s)
{   return (!p ? 0 : (
		    *s == p->splitchar
		    ? (*s ? rsearch2(p->eqkid, ++s) : 1)
		    : (rsearch2(*s < p->splitchar ?
				p->lokid : p->hikid, s))
		    ));
}

char *srcharr[100000];
int srchtop, nodecnt;

void pmsearch(Tptr p, char *s)
{ 
	if (!p) return;
	nodecnt++;
	if (*s == '.' || *s < p->splitchar)
		pmsearch(p->lokid, s);
	if (*s == '.' || *s == p->splitchar)
		if (p->splitchar && *s)
			pmsearch(p->eqkid, s+1);
	if (*s == 0 && p->splitchar == 0){
		srcharr[srchtop++] = (char *) p->eqkid;
	}
	if (*s == '.' || *s > p->splitchar)
		pmsearch(p->hikid, s);
}

void nearsearch(Tptr p, char *s, int d)
{   if (!p || d < 0) return;
	nodecnt++;
	if (d > 0 || *s < p->splitchar)
		nearsearch(p->lokid, s, d);
	if (p->splitchar == 0) {
		if ((int) strlen(s) <= d){
			srcharr[srchtop++] = (char *)p->eqkid;
			srcharr[srchtop++] = (char *)p->lokid;
		}
	} else
		nearsearch(p->eqkid, *s ? s+1:s,
			   (*s == p->splitchar) ? d:d-1);
	if (d > 0 || *s > p->splitchar)
		nearsearch(p->hikid, s, d);
}

void traverse(Tptr p)
{   if (!p) return;
	traverse(p->lokid);
	if (p->splitchar)
		traverse(p->eqkid);
	else
		printf("%s\n", (char *) p->eqkid);
	traverse(p->hikid);
}


/* HASH TABLES */

typedef struct hnode *Hptr;
typedef struct hnode {
	char	*str;
	Hptr	next;
} Hnode;
Hptr *hashtab;
int	tabsize;

int hashfunc(char *s)
{   unsigned n = 0;
	for ( ; *s; s++)
		n = 31 * n + *s;
	return n % tabsize;
}

void hbuild(char **a, int n)
{   int	i, j;
	Hptr	p;
	tabsize = n;
	hashtab = (Hptr *) malloc(tabsize * sizeof(Hptr));
	for (i = 0; i < tabsize; i++)
		hashtab[i] = 0;
	for (i = 0; i < tabsize; i++) {
		j = hashfunc(a[i]);
		p = (Hptr) malloc(sizeof(Hnode));
		p->str = a[i];
		p->next = hashtab[j];
		hashtab[j] = p;
	}
}

int hsearch(char *ins)
{   Hptr p;
	char *s, *t;
	for (p = hashtab[hashfunc(ins)]; p; p = p->next) {
		/* if (strcmp(ins, p->str) == 0) return 1; */
		for (s = ins, t = p->str; *s == *t; s++, t++)
			if (*s == 0)
				return 1;
	}
	return 0;
}


/* TIMING */

int qscomp(const void *p, const void *q )
{   return strcmp(* (char**) p, * (char**) q );
}
#define DOQSORT qsort((void *) a, (size_t) n, sizeof(char *), qscomp)

int instype;

void rinsall(char *a[], char *address[], int n)
{   int m;
	if (n < 1) return;
	m = n/2;		/* because here the index file must sort */
	switch (instype) {
	case 1: insertstr = a[m]; root = insert(root,a[m]); break;
	case 2: {
		insertstr = a[m];
		insertsha1 = address[m];
		root = insert2(root,a[m]); 
		break;
	}

	case 3: insert3(a[m]); break;
	}
	rinsall(a,address, m);
	rinsall(a+m + 1,address+m + 1, n-m-1);
}

void insall(char *a[],char *address[], int n)
{ 
	root = 0; bufn = freen = 0;
	rinsall(a, address, n);
}

int searchtype;

void searchall(char *a[], int n)
{   int i, j;
	char s[100]; 
	for (i = 0; i < n; i++) {
		strcpy(s, a[i]);
		/* s[0]++;  Uncomment for unsuccessful searches */
		switch(searchtype) {
		case 1: j = search(s); break;
		case 2: srchtop = 0; pmsearch(root, s); j = srchtop; break;
		case 3: srchtop = 0; nearsearch(root, s, 0); j = srchtop; break;
		case 4: j = hsearch(s); break;
		case 5: j = rsearch(root, s); break;
		case 9: j = 1; break;
		}
		if (j != 1)  /* Uncomment for unsuccessful searches */
			printf("search bug at %d, val %d\n", i, j);
	}	
}

#define TASK(s)	printf("%s", s);
#define CIN		starttime = clock();
#define COUT	printf("\t%d", clock()-starttime);
#define NL	printf("\n")
#define REPEAT(s)	for (rptctr=0; rptctr<5; rptctr++) { s }; NL;

void timeall()
{
	TASK("Build TST");
	instype = 1; REPEAT(CIN; insall(a,address, n); COUT; cleanup(root);)
			     TASK("Build TST 2");
	instype = 2; REPEAT(CIN; insall(a,address, n); COUT; cleanup2(root);)
			     TASK("Build TST 3");
	instype = 3; REPEAT(CIN; insall(a,address, n); COUT; cleanup2(root);)
			     TASK("Build TST for searches");
	CIN; instype = 2; insall(a,address, n); COUT; NL;
	TASK("Null Search");
	searchtype = 9; REPEAT(CIN; searchall(a, n); COUT;)
				TASK("TST Search");
	searchtype = 1; REPEAT(CIN; searchall(a, n); COUT;)
				TASK("Rec TST Search");
	searchtype = 5; REPEAT(CIN; searchall(a, n); COUT;)
				TASK("PM TST Search");
	searchtype = 2; REPEAT(CIN; searchall(a, n); COUT;)
				TASK("Near TST Search");
	searchtype = 3; REPEAT(CIN; searchall(a, n); COUT;)
				TASK("Remove TST")
				CIN; cleanup2(root); COUT; NL;
	TASK("Build Hash");
	CIN; hbuild(a, n); COUT; NL;
	TASK("Hash Search");
	searchtype = 4; REPEAT(CIN; searchall(a, n); COUT;)
				/* Hash table is still using space */
				}

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

void trysearch()
{
	char s[100];
	int i, d;
	instype = 2; insall(a,address, n);
	for (i = 0; i <  10; i++)
		srcharr[i] = NULL;
	printf("Enter searches: <distance> <word>\n");
	while (scanf("%d %s", &d, s) != EOF) {
		nodecnt = srchtop = 0;
		CIN;
		if (d < 0)
			pmsearch(root, s);
		else
			nearsearch(root, s, d);
		printf(" matches=%d nodes=%d clicks=", srchtop, nodecnt);
		COUT; NL;
		printf("------------result----------\n");
		for (i = 0; i < min(srchtop, 20); ){
			printf("%s\t", srcharr[i++]);
			printf("%s\n", srcharr[i++]);
		}
		NL;
		printf("Enter searches: <distance> <word>\n");
	}
}

int display_sha1(char ** sha1, int n)
{
	printf("----------begin dump sha1-----------\n");
	int i=1;
	for(i=1;i<=n;i++){
		printf("%s\n", sha1[i]);
	}
	printf("----------dump sha1 over------------\n");
	return 0;
}
int main(int argc, char *argv[])
{
	int i, globalstarttime;
	char *s = space, *fname;
	FILE *fp;
	if (argc == 1) { /* no args */
		fname = "/usr/jlb/data/words"; /* default dict file */
	} else /* at least one arg: file name */
		fname = argv[1];
	
	setbuf(stdout, 0);
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "  Can't open file\n");
		exit(1);
	}

	globalstarttime = clock();

	TASK("Reading Input");
	CIN;
	a[0] = s;
	char *sha1;
	char line[LINECHARS];
	while (!feof(fp)){
		if (fgets(line, LINECHARS, fp) != NULL){
			s = (char *) malloc(MAXCHARS * sizeof(char));
			sha1 = (char *) malloc(SHA1CHARS * sizeof(char));
			split(line, s, sha1);
			a[++n] = s;
			address[n] = sha1;
/*			printf("line number:%d\n", n);
			s = (char *) malloc(LINECHARS * sizeof(char));
			a[++n] = s;*/
		}
	}
	COUT; NL;

/*	TASK("System Qsort"); CIN; DOQSORT; COUT; NL; */

	if (argc < 3) { /* at most one arg: file name */
		timeall();
	} else {
		if (strcmp(argv[2], "trysearch") == 0) {
			trysearch();
		} else if (strcmp(argv[2], "traverse") == 0) {
			instype = 2;
			insall(a,address, n);
			traverse(root);
		} else
			printf("Unrecognized option\n");
	}

	i = clock() - globalstarttime;
	printf("Total clicks\t%d\nTotal secs\t%4.3f\n",
	       i, (double) i / CLOCKS_PER_SEC);
	return 0;
}


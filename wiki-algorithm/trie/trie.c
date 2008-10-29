/*

Copyright (c) 2005, Simon Howard
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

 * Redistributions of source code must retain the above copyright 
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
   notice, this list of conditions and the following disclaimer in 
   the documentation and/or other materials provided with the 
   distribution.
 * Neither the name of the C Algorithms project nor the names of its 
   contributors may be used to endorse or promote products derived 
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

*/

/* Trie: fast mapping of strings to values */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "trie.h"

typedef struct _TrieNode TrieNode;

struct _TrieNode {
	void *data;
	unsigned int use_count;
	TrieNode *next[256];
};

struct _Trie {
	TrieNode *root_node;
};


static void trie_free_node(TrieNode *node)
{
	int i;

	if (node == NULL)
		return;
	
	/* First, free all subnodes */

	for (i=0; i<256; ++i) {
		trie_free_node(node->next[i]);
	}

	/* Free this node */

	free(node);
}

Trie trie_new(void)
{
	Trie new_trie;

	new_trie = (Trie) malloc(sizeof(Trie));
	(new_trie)->root_node = NULL;

	return new_trie;
}

void trie_free(Trie trie)
{
	/* Free the subnode, and all others by implication */
	trie_free_node(trie->root_node);

	/* Free the trie */
	free(trie);
}

void trie_insert(Trie trie, char *key, void *value)
{
	TrieNode **rover;
	TrieNode *node;
	char *p;
	int c;

	/* Cannot insert NULL values */
	if (value == NULL) {
		return;
	}

	/* Search down the trie until we reach the end of string,
	 * creating nodes as necessary */
	rover = &(trie->root_node);
	p = key;

	for (;;) {
		node = *rover;
		if (node == NULL) {
			/* Node does not exist, so create it */
			node = (TrieNode *) malloc(sizeof(TrieNode));
			memset(node, 0, sizeof(TrieNode));

			/* Link in to the trie */
			*rover = node;
		}

		/* One more use of this node */
		++node->use_count;

		/* Current character */
		c = *p;

		/* Reached the end of string?  If so, we're finished. */
		if (c == '\0') {
			/* Set the data at the node we have reached */
			node->data = value;
			break;
		}

		/* Advance to the next node in the chain */
		rover = &node->next[c];
		++p;
	}
}

void trie_remove(Trie *trie, char *key)
{
	TrieNode *node;
	TrieNode *next;
	TrieNode **last_next_ptr;
	char *p;
	int c;
	
	/* First, search down to the ending node so that the data can
	 * be removed. */

	/* Search down the trie until the end of string is reached */

	node = (*trie)->root_node;

	for (p=key; *p != '\0'; ++p) {

		if (node == NULL) {
			/* Not found in the tree. Return. */

			return;
		}

		/* Jump to the next node */

		c = *p;
		node = node->next[c];
	}

	/* Remove the data at this node */

	node->data = NULL;

	/* Now traverse the tree again as before, decrementing the use
	 * count of each node.  Free back nodes as necessary. */

	node = (*trie)->root_node;
	last_next_ptr = &(*trie)->root_node;
	p = key;

	for (;;) {

		/* Find the next node */
		
		c = *p;
		next = node->next[c];

		/* Free this node if necessary */

		--node->use_count;

		if (node->use_count <= 0) {
			free(node);

			/* Set the "next" pointer on the previous node to NULL,
			 * to unlink the free'd node from the tree.  This only
			 * needs to be done once in a remove.  After the first
			 * unlink, all further nodes are also going to be
			 * free'd. */

			if (last_next_ptr != NULL) {
				*last_next_ptr = NULL;
				last_next_ptr = NULL;
			}
		}
		
		/* Go to the next character or finish */

		if (c == '\0') {
			break;
		} else {
			++p;
		}

		/* If necessary, save the location of the "next" pointer
		 * so that it may be set to NULL on the next iteration if
		 * the next node visited is freed. */

		if (last_next_ptr != NULL) {
			last_next_ptr = &node->next[c];
		}
		
		/* Jump to the next node */

		node = next;
	}
}

void *trie_lookup(Trie trie, char *key)
{
	TrieNode *node;
	char *p;
	int c;

	/* Search down the trie until the end of string is found */
	
	node = trie->root_node;
	p = key;

	while (*p != '\0') {
		if (node == NULL) {
			/* Not found - reached end of branch */

			return NULL;
		}

		/* Advance to the next node in the chain, next character */

		c = *p;
		node = node->next[c];
		++p;
	}

	return node->data;
}

int trie_num_entries(Trie *trie)
{
	/* To find the number of entries, simply look at the use count
	 * of the root node. */

	if ((*trie)->root_node == NULL) {
		return 0;
	} else {
		return (*trie)->root_node->use_count;
	}
}

#define TASK(s)	printf("%s", s);
#define CIN		starttime = clock();
#define COUT	printf("%g", clock()-starttime);
#define NL	printf("\n")
#define SHA1CHARS 40		/* sha1 char count */
#define MAXCHARS 100		/* the max chars of the title */
#define LINECHARS 80
#define MAXWORDS 1000
char space[MAXCHARS], *a[MAXWORDS] , *address[MAXWORDS];
int starttime = 0, n = 0;
Trie root;

int split(char *source, char *word, char*sha1)
{
	if(*source == 0){
		*word = 0;
		*sha1 = 0;
		return 0;
	}
	char *p = strrchr(source, ' ');
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

int generate_trie(char *a[])
{
	int i = 0;
	for(i = 1; i< MAXWORDS; i++)
		if(a[i] != NULL){
			printf("%s---%s\n",a[i],address[i]);
			trie_insert(root,a[i],address[i]);
		}
	return 0;
}

void trysearch()
{
	char *sha1;
	char * title;
	root = trie_new();
	generate_trie(a);
	printf("Enter searches: <word>\n");
	while (scanf("%[^\n]%*c", title) != EOF) {
                CIN;
		sha1 = trie_lookup(root, title);
		COUT; NL;

		printf("sha1 is :%s\n", sha1);
		printf("Enter searches: <word>\n");
	}
	trie_free(root);
}			       
int main(int argc, char *argv[])
{
	double i, globalstarttime;
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
			printf("read lines: %d\n", n);
		}
	}
	COUT; NL;

/*	TASK("System Qsort"); CIN; DOQSORT; COUT; NL; */

	if (argc < 3) { /* at most one arg: file name */

	} else {
		if (strcmp(argv[2], "trysearch") == 0) {
			trysearch();
		} else if (strcmp(argv[2], "traverse") == 0) {

		} else
			printf("Unrecognized option\n");
	}

	i = clock() - globalstarttime;
	printf("Total clicks\t%g\nTotal secs\t%g\n",
	       i, (double) i / CLOCKS_PER_SEC);
	return 0;
}


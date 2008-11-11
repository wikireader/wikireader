/*
 * Trie: fast mapping of strings to values 
 *
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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "trie.h"

/* if the offset == 0 means NULL */
#define TASK(s)	printf("%s", s);
#define CIN		starttime = clock();
#define COUT	printf("%f", clock()-starttime);
#define NL	printf("\n")
#define SHA1CHARS 40		/* sha1 char count */
#define MAXCHARS 100		/* the max chars of the title */
#define LINECHARS 140		/* the max chars of one line*/
#define INDEX_NAME ".\\index.bin"
#define CHILD_COUNT  26		/* 'z' - 'a' */

int starttime = 0, n = 0;
char space[MAXCHARS];

typedef unsigned short uint16;
typedef struct _OnDiskNodeVariable  *OnDiskNode;
struct _OnDiskNodeVariable {
	unsigned char character;	/* current node character */
	uint16 sha1_offset;		/*use another file to store the sha1,
						 * if sha1_offset=0 means this node is not a title .*/
	uint16 parent_offset;		/* 0 means no parent */
	uint16 children_offset[CHILD_COUNT];	/* 0 means no this child */
	char padding; 
};
OnDiskNode g_node;	 /* i use a globle variable store the node
					  * read from the index.file */

struct _OnDiskRoot {
	uint16 children_offset[CHILD_COUNT];
};
OnDiskRoot root;

OnDiskNode trie_init_node(OnDiskNode node)
{
	node->character = 0;
	node->sha1_offset = 0;
	node->parent_offset = 0;
	int i = 0;
	for(i=0;i<CHILD_COUNT;i++)
		node->children_offset[i] = 0;
	return node;
}

OnDiskRoot trie_new(FILE *fp_index)
{
	OnDiskRoot root = (OnDiskRoot)malloc(sizeof(struct _OnDiskRoot));
	int i = 0;
	for(i=0;i<CHILD_COUNT;i++)
		root->children_offset[i] = 0;

	write(fp_index, root, sizeof(struct _OnDiskRoot));
//	write(fp_index, root->children_offset, sizeof(struct _OnDiskRoot));
	if(write(fp_index, "a", sizeof(char)) <=0)
		perror("Write error");

	OnDiskNode g_node = (OnDiskNode)malloc(sizeof(struct _OnDiskNodeVariable)); 
	trie_init_node(g_node);

	return root;
}

int write_node_to_disc(struct OnDiskNodeVariable * node)
{
	off_t current_offset = 0;
/*    while (last) {
      n_bytes = last->payload ? strlen(last->payload) : 0; 
      off_t offset = current_offset + sizeof(offset);*/
	FILE *fp;
	setbuf(stdout, 0);
	return 0;
}

int trie_clean()
{
	free(root);
	free(g_node);
	return 0;
}
/*==================================*/
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

/* ===================================== */
int split(char *source, char *word, char*sha1)
{
	if(*source == 0){
		*word = 0;
		*sha1 = 0;
		return 1;
	}
	char *p = strrchr(source, ' ');
	int i=0;
	int split_char_pos = p - source;
	for(i = 0; i < split_char_pos; i++){
                *(word++) = *(source++);
	}
	*word='\0';
	source++;		/* eat the blank, ther is a blank between title and sha1 */
        while(*source != '\n' && *source != EOF)
                *(sha1++) = *(source++);
	*sha1='\0';

        return 0;
}

static void read_node(char *findex)
{
	char *sha1, *title;
	char line[LINECHARS];
	int n=0;
	FILE *fp;
	setbuf(stdout, 0);

	if ((fp = fopen(findex, "r")) == NULL) {
		fprintf(stderr, "Can't create file!\n");
		exit(1);
	}

	while (!feof(fp)){
		if (fgets(line, LINECHARS, fp) != NULL){
			title = (char *) malloc(MAXCHARS * sizeof(char));
			sha1 = (char *) malloc(SHA1CHARS * sizeof(char));
			split(line, title, sha1);
			printf("read lines: %d\n", ++n);
			/* TODO: create the ondiskstructure file */
		}
	}
}


void trysearch()
{
	char *sha1;
	char * title;
/*	root = trie_new(); */
/*	generate_trie(a); */
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
	FILE *fp_index;
	char *fname;
	if ((fp_index = fopen(INDEX_NAME, "w")) == NULL) {
		fprintf(stderr, "Can't create file!\n");
		exit(1);
	}

	if (argc < 3) { /* at most one arg: file name */
		trie_new(fp_index);	
		trie_clean();
		return 1;
	} else {
		fname = argv[1];

		if (strcmp(argv[2], "trysearch") == 0) {
			trysearch();
		} else if (strcmp(argv[2], "traverse") == 0) {

		} else
			printf("Unrecognized option\n");
	}
	fclose(fp_index);
	return 0;
}


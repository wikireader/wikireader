/*
 * index creation applet
 * Copyright Openmoko, Inc 2009
 * Author: Andy Green <andy@openmoko.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "indexer.h"

#define MAX_INDEXABLE_LENGTH 256

#define MAX_STACK_DEPTH (8 * MAX_INDEXABLE_LENGTH)

/*
 * wordlist is just cr terminated list of words.  They need to be in STRICT
 * binary-sorted order with no exceptions for language customs
 *
 * this will make a suitable list
 * cat /usr/share/dict/words | grep -v "\." | grep -v "\," |
 *  grep -v "\-" | grep -v "'" | grep -v "/" | grep -v "\&" > wordlist
 *
 * you also need to convert it to lowercase, and make sure there are no dupes
 * caused by that by eg, uniq
 */

struct creation_stack {
	int level;
	unsigned long long file_offset;
};


int main(int argc, char ** argv)
{
//	FILE *results;
	char *line;
	char buffer[256];
	int stack = 0;
//		int depth = 0;
	int same = 0;
	int bits;
	int max;
	int more;
	unsigned long pos;
	int count_words = 0;
	struct node node;
	int idx;
	struct creation_stack creation_stack[MAX_STACK_DEPTH];
	int dbg = 0;

	if (argc < 2) {
		printf("Usage: %s <filepath of wordlist>\n", argv[0]);
		return 1;
	}

	/* create the index */

	FILE *words = fopen(argv[1], "r");

	if (words == NULL) {
		perror("Unable to open wordlist");
		return 1;
	}

	idx = open("./index", O_RDWR | O_CREAT | O_TRUNC);
	if (idx < 0) {
		perror("Unable to open index for write\n");
		return 1;
	}

	printf("sizeof(node) = %zu\n", sizeof(struct node));

	do {
		line = fgets(buffer, sizeof(buffer), words);
		if (line == NULL)
			continue;

		bits = (strlen(line) - 1) << 3;
		if (bits == 0)
			continue;

		line[bits >> 3] = '\0';

		if (dbg)
			printf("%s ", line);

		line = buffer;

		/* re-use as much of current node stack as possible */

		same = 0;
		max = bits;
		if (max > stack)
			max = stack;
		more = 1;
		while (same < max && more) {
			if (!!(line[same >> 3] & (1 << ((~same) & 7))) ==
				!!creation_stack[same].level)
				same++;
			else
				more = 0;
		}

		if (dbg)
			printf("matched %d on stack of %d, bits = %d\n", same, stack, bits);

		if (more && same == bits) {
			printf("%s duplicate input!\n", line);
			return 1;
		}

		/*
		 * is there a point at which we differed or did we
		 * run out of stack?
		 */

		if (!more && same < stack && same < bits) {
			unsigned long offset;
			int ret;

			if (creation_stack[same].level) {
				/*
				 * it would mean the incoming sorting is
				 * broken
				 */
				printf("broken flip of mismatched bit %d\n", same);
				return 1;
			}

			/*
			 * go back to that node, and rewrite the '1' path to
			 * point to the new nodes we are about to write.  Or,
			 * if this was the last bit of the string, put the
			 * 1 payload there.
			 */

			pos = lseek(idx, 0, SEEK_CUR);
			offset = pos - creation_stack[same].file_offset;
			if (offset & ~NODE_OFFSET_MASK)
				printf("node offset mask exceeded\n");

			if (dbg) {
				printf("    pos=0x%lx, same = %d\n", pos, same);
				printf("   rewriting 0x%lX to have 1 set\n", (long)creation_stack[same].file_offset);
			}

			if (lseek(idx, creation_stack[same].file_offset, SEEK_SET) < 0) {
				perror("failed to seek\n");
				return 1;
			}
			/* it may be a node, but our interest ends at common
			 * flags_and_one_skip at start
			 */
			ret = read(idx, &node, sizeof(struct node));
			if (ret != sizeof(struct node)) {
				printf("ret = %d\n", ret);
				perror("failed to read\n");
				return 1;
			}
			if (dbg)
				printf("     old=%X\n", node.flags_and_one_skip);


			if ((same + 1) == bits) {
				/*
				* we only differ by last bit, so we need to deal with
				* adding leaf payload now since we won't add any nodes
				*/
				if (node.flags_and_one_skip && node.result[1]) {
					printf("%s node rewrite tries to reassign leaf\n", line);
					return 1;
				}
				node.result[1] = 0xbbbbbbbb;
			} else {
				node.flags_and_one_skip = (node.flags_and_one_skip &
						(NRT_MASK << ZERO_NRT_SHIFT)) |
						(NRT_NODE << ONE_NRT_SHIFT) |
						offset;
				creation_stack[same].level = 1;
			}

			if (dbg)
				printf("     new=%X\n", node.flags_and_one_skip);
			lseek(idx, creation_stack[same].file_offset, SEEK_SET);
			write(idx, &node, sizeof(struct node));
			lseek(idx, pos, SEEK_SET);

			same++;

			if (same >= MAX_STACK_DEPTH) {
				printf("  exceeded node stack\n");
				return 1;
			}
		}

		/*
		 * now we must create nodes for all bits between
		 * same and bits
		 */

		while (same < bits) {
			int level = !!(line[same >> 3] & (1 << ((~same) & 7)));

			node.result[0] = 0;
			node.result[1] = 0;

			if ((same + 1) == bits)
				node.result[level] = 0xaaaaaaaa;

			if (level) {
				/* a 1, it means zero must be NULL */
				node.flags_and_one_skip =
					(NRT_NULL << ZERO_NRT_SHIFT) |
					(NRT_NODE << ONE_NRT_SHIFT) |
					sizeof(struct node); /* set 1 offset */

				creation_stack[same].level = 1;
			} else {
				/* a 0, 1 as NRT_NULL but we may come back */
				node.flags_and_one_skip =
					(NRT_NODE << ZERO_NRT_SHIFT) |
					(NRT_NULL << ONE_NRT_SHIFT) |
					0;
				creation_stack[same].level = 0;
			}
			if (dbg)
				printf("  writing 0x%x, bit %d at 0x%lx bit=%d\n", node.flags_and_one_skip, same, lseek(idx, 0, SEEK_CUR), level);

			creation_stack[same].file_offset = lseek(idx, 0, SEEK_CUR);
			write(idx, &node, sizeof(struct node));

			same++;
		}

		stack = same;

		count_words++;

	} while (line != NULL);

//	fclose(results);
	close(idx);
	fclose(words);

	printf("%d words indexed\n", count_words);

	return 0;

}

/*
 * indexer parser
 * Copyright Openmoko, Inc 2009
 * Author: Andy Green <andy@openmoko.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "indexer.h"

int main(int argc, char ** argv)
{
	FILE *idx;
//	FILE *results;
	char *line;
	int bits;
	int bit;
	int more;
	int last_offset = -1;
	int count_same_512 = 0;
	int count_reads = 0;

	idx = fopen("./index", "r");
	if (idx == NULL) {
		perror("Unable to open index for read\n");
		return 1;
	}

	line = argv[1];
	bits = strlen(line) << 3;

	while (*line) {
		*line = tolower(*line);
		line++;
	}

	line = argv[1];
	bit = 0;
	more = 1;

	while (bit < bits && more) {
		enum node_result_type type;
		struct node node;
		int pos = ftell(idx);
		int level = !!(line[bit >> 3] & (1 << ((~bit) & 7)));

		printf("reading node at 0x%X %d\n", pos, line[bit >> 3] & (1 << ((~bit) & 7)));
		if ((pos & ~(512 - 1)) == (last_offset & ~(512 - 1)))
			count_same_512++;
		last_offset = pos;
		count_reads++;

		fread(&node, sizeof(struct node), 1, idx);

		if (level) {
			/* 1 path */
			type = (node.flags_and_one_skip >> ONE_NRT_SHIFT) & NRT_MASK;
			if (type == NRT_NODE)
				fseek(idx, pos +
					(node.flags_and_one_skip & NODE_OFFSET_MASK),
								SEEK_SET);
		} else
			/* 0 path */
			type = (node.flags_and_one_skip >> ZERO_NRT_SHIFT) & NRT_MASK;

		if (node.result[level]) { // leaf
			if ((bit + 1) == bits) {
				printf("Result found at bit %d\n", bit);
				more = 0;
				continue;
			} else {
				printf("Saw subresult at bit %d\n", bit);
			}
		}

		if (type == NRT_NULL) {/* no valid results exist */
			printf("There are no valid results at bit %d: path failed on %d\n", bit, line[bit >> 3] & (1 << ((~bit) & 7)));
			more = 0;
			continue;
		}

		bit++;
	}

	if (more) {
		printf("Ambiguity, did not reach final node bit = %d\n", bit);

		/* finding matches on the tree */

	}

	if (count_reads)
		printf("Reads: %d, same block: %d (%d%%)\n", count_reads,
			  count_same_512, (count_same_512 * 100) / count_reads);

	fclose(idx);

	return 0;

}

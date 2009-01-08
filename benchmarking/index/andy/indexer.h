/*
 * indexer node types
 * Copyright Openmoko, Inc 2009
 * Author: Andy Green <andy@openmoko.com>
 */

enum node_result_type {
	NRT_NULL = 0,
	NRT_NODE = 1, /* search continues with another node */
	NRT_MASK = 1
};

#define ZERO_NRT_SHIFT 31
#define ONE_NRT_SHIFT 30
#define NODE_OFFSET_MASK 0x3fffffff

/*
 * nodes tell us where to go if the bit we look for is '0' or '1'.
 * if '0', and the NRT bit for 0 is not NULL, you just continue directly after
 *   this node.
 * if '1', and the NRT bit for 1 is not NULL, you add the offset part of
 *   flags_and_one_skip to the file offset of the start of this node, and
 *   continue from there.
 *
 * Additionally, if at any point result[0] or result[1] is nonzero, that point
 *   in the tree has a result, ie, represents a word from the wordlist
 */

struct node {
	/*
	 * The node or leaf for 0 case follows immediately after if it is not
	 * NRT_NULL for 0.  The node or leaf for 1 case is at this offset from
	 * the current node.
	 *
	 * 30-bit field allows > 1GByte index file
	 */
	unsigned int flags_and_one_skip;
	/* valid when NRT_NULL for 0 and 1 above */
	unsigned int result[2];
};


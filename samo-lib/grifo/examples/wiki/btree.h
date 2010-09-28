/*
 * Copyright (c) 2010 Openmoko Inc.
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

#ifndef BTREE_H
#define BTREE_H

#define BTREE_MAX_ELEMENTS 10
#define BTREE_INVALID_NODE_IDX -1
#define BTREE_INVALID_ELEMENT_IDX -1
#define BTREE_INVALID_ENTRY_IDX -1
#define BTREE_IS_VALID_NODE_IDX(a) (a != BTREE_INVALID_NODE_IDX)
#define BTREE_IS_VALID_ELEMENT_IDX(a) (a != BTREE_INVALID_ELEMENT_IDX)
#define BTREE_IS_VALID_ENTRY_IDX(a) (a != BTREE_INVALID_ENTRY_IDX)

typedef struct _BTREE_ELEMENT {
// contains a key value, a payload, and a pointer toward the subtree
// containing key values greater than this->m_key but lower than the
// key value of the next element to the right
    long key;
    int subtree_node_idx;
    int data_entry_idx;
} BTREE_ELEMENT, *PBTREE_ELEMENT;

typedef struct _BTREE_NODE {
    int element_count;
    int parent_node_idx;
    int next_free_node_idx; // for free nodes only
    BTREE_ELEMENT elements[BTREE_MAX_ELEMENTS];
} BTREE_NODE, *PBTREE_NODE;

typedef struct _BTREE {
    int max_nodes;
    int first_free_node_idx;
    int root_node_idx;
    PBTREE_NODE nodes;
	long invalid_key;
} BTREE, *PBTREE;

int btree_minimum_keys ();
void btree_dump (PBTREE btree, int node_idx, int depth);
int btree_init(PBTREE btree, int max_entries, long invalid_key);
PBTREE_ELEMENT btree_search (PBTREE btree, long desired_key);
int btree_insert (PBTREE btree, PBTREE_ELEMENT element);
int btree_delete (PBTREE btree, long target_key);
#endif

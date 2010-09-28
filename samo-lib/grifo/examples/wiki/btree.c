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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grifo.h"
#include "btree.h"

void set_root(PBTREE btree, int node_idx);
int get_free_node(PBTREE btree);
void insert_zeroth_subtree (PBTREE btree, int node_idx, int subtree_node_idx);
int key_count (PBTREE btree, int node_idx);
PBTREE_ELEMENT largest_key (PBTREE btree, int node_idx);
PBTREE_ELEMENT smallest_key (PBTREE btree, int node_idx);
int find_root (PBTREE btree);
int is_leaf (PBTREE btree, int node_idx);
void delete_node(PBTREE btree, int node_idx);
int delete_all_subtrees (PBTREE btree, int node_idx);
int vector_insert (PBTREE btree, int node_idx, PBTREE_ELEMENT element);
int vector_delete (PBTREE btree, int node_idx, long target_key);
int vector_delete_pos (PBTREE btree, int node_idx, int target_pos);
int vector_insert_for_split (PBTREE btree, int node_idx, PBTREE_ELEMENT element);
int split_insert (PBTREE btree, int node_idx, PBTREE_ELEMENT element);
int rotate_from_right(PBTREE btree, int node_idx, int parent_index_this);
int rotate_from_left(PBTREE btree, int node_idx, int parent_index_this);
int merge_right (PBTREE btree, int node_idx, int parent_index_this);
int merge_left (PBTREE btree, int node_idx, int parent_index_this);
int right_sibling (PBTREE btree, int node_idx, int *parent_index_this);
int left_sibling (PBTREE btree, int node_idx, int *parent_index_this);
int index_has_subtree (PBTREE btree, int node_idx);
PBTREE_ELEMENT smallest_key_in_subtree (PBTREE btree, int node_idx);
PBTREE_ELEMENT btree_search_ex (PBTREE btree, int *node_idx, long desired_key);

int btree_minimum_keys () {
    // minus 1 for the empty slot left for splitting the node
    int ceiling_func = (BTREE_MAX_ELEMENTS-1)/2;
    if (ceiling_func*2 < BTREE_MAX_ELEMENTS-1)
        ceiling_func++;
    return ceiling_func-1;  // for clarity, may increment then decrement
}

int btree_init(PBTREE btree, int max_entries, long invalid_key)
{
    btree->nodes = (PBTREE_NODE)memory_allocate(sizeof(BTREE_NODE) * (max_entries / btree_minimum_keys() + 1), "btree");
    if (!btree->nodes)
    {
        return -1;
    }
    else
    {
        int i;
        
        btree->max_nodes = (max_entries / btree_minimum_keys() + 1);
        btree->root_node_idx = BTREE_INVALID_NODE_IDX;
        btree->first_free_node_idx = 0;
		btree->invalid_key = invalid_key;
        for (i = 0; i < btree->max_nodes; i++)
        {
            btree->nodes[i].element_count = 0;
            btree->nodes[i].parent_node_idx = BTREE_INVALID_NODE_IDX;
            if (i == btree->max_nodes - 1)
                btree->nodes[i].next_free_node_idx = BTREE_INVALID_NODE_IDX;
            else
                btree->nodes[i].next_free_node_idx = i + 1;
        }
        return 0;
    }
}

int get_free_node(PBTREE btree)
{
    int free_node_idx;
    
    free_node_idx = btree->first_free_node_idx;
    if (BTREE_IS_VALID_NODE_IDX(free_node_idx))
        btree->first_free_node_idx = btree->nodes[free_node_idx].next_free_node_idx;

    return free_node_idx;
}

void insert_zeroth_subtree (PBTREE btree, int node_idx, int subtree_node_idx) {
    btree->nodes[node_idx].elements[0].key = btree->invalid_key;
	btree->nodes[node_idx].elements[0].subtree_node_idx = subtree_node_idx;
    btree->nodes[node_idx].elements[0].data_entry_idx = BTREE_INVALID_ENTRY_IDX;
    btree->nodes[node_idx].element_count = 1;
    if (subtree_node_idx != BTREE_INVALID_NODE_IDX)
        btree->nodes[subtree_node_idx].parent_node_idx = node_idx;
}

void set_root(PBTREE btree, int node_idx)
{
    btree->root_node_idx = node_idx;
}

int key_count (PBTREE btree, int node_idx)
{
    return btree->nodes[node_idx].element_count-1;
}

PBTREE_ELEMENT largest_key (PBTREE btree, int node_idx)
{
    return &btree->nodes[node_idx].elements[btree->nodes[node_idx].element_count-1];
}

PBTREE_ELEMENT smallest_key (PBTREE btree, int node_idx)
{
    return &btree->nodes[node_idx].elements[1];
}

void btree_dump (PBTREE btree, int node_idx, int depth){
// write out the keys in this node and all its subtrees, along with
// node adresses, for debugging purposes
    char indent[21];
    int i;
    
    if (node_idx == BTREE_INVALID_NODE_IDX)
        node_idx = btree->root_node_idx;

    if (depth > 10)
        depth = 10;
    if (depth > 0)
        memset(indent, ' ', 2 * depth);
    indent[2 * depth] = '\0';
    if (node_idx == btree->root_node_idx)
		debug_printf("ROOT\n");
	debug_printf("%sNode idx=%d, parent idx=%d, element count=%d\n",
		indent, node_idx, btree->nodes[node_idx].parent_node_idx, btree->nodes[node_idx].element_count);
    for (i=0; i<btree->nodes[node_idx].element_count; i++)
    {
        PBTREE_ELEMENT element;
        element = &btree->nodes[node_idx].elements[i];
        if (element->data_entry_idx != BTREE_INVALID_ENTRY_IDX)
            debug_printf("%sKey=%ld, data entry idx=%d\n", indent, element->key, element->data_entry_idx);
        if (element->subtree_node_idx != BTREE_INVALID_NODE_IDX)
            btree_dump(btree, element->subtree_node_idx, depth + 1);
    }
}

int find_root (PBTREE btree) {
    return btree->root_node_idx;
}

int is_leaf (PBTREE btree, int node_idx) {
    return btree->nodes[node_idx].elements[0].subtree_node_idx == BTREE_INVALID_NODE_IDX;
}

void delete_node(PBTREE btree, int node_idx) {
    btree->nodes[node_idx].element_count = 0;
    btree->nodes[node_idx].parent_node_idx = BTREE_INVALID_NODE_IDX;
    btree->nodes[node_idx].next_free_node_idx = btree->first_free_node_idx;
    btree->first_free_node_idx = node_idx;
}

int delete_all_subtrees (PBTREE btree, int node_idx) {
// return the number of nodes deleted
    int count_deleted = 0;
	int i;
    PBTREE_NODE node = &btree->nodes[node_idx];
    for (i=0; i< node->element_count; i++) {
        if (node->elements[i].subtree_node_idx == BTREE_INVALID_NODE_IDX)
            continue;
        else if (is_leaf(btree, node->elements[i].subtree_node_idx)) {
            delete_node(btree, node->elements[i].subtree_node_idx);
            count_deleted++;
        }
        else
            count_deleted += delete_all_subtrees(btree, node->elements[i].subtree_node_idx);
    }
    return count_deleted;
}

int vector_insert (PBTREE btree, int node_idx, PBTREE_ELEMENT element) {
// this method merely tries to insert the argument into the current node.
// it does not concern itself with the entire tree.
// if the element can fit into m_vector, slide all the elements
// greater than the argument forward one position and copy the argument
// into the newly vacated slot, then return 1.  otherwise return 0.
// note: the tree_insert method will already have verified that the key
// value of the argument element is not present in the tree.
	int i;

    if (btree->nodes[node_idx].element_count >= BTREE_MAX_ELEMENTS - 1) // leave an extra slot for split_insert
        return 0;
    i = btree->nodes[node_idx].element_count;

    while (i>0 && btree->nodes[node_idx].elements[i-1].key > element->key) {
        memcpy(&btree->nodes[node_idx].elements[i], &btree->nodes[node_idx].elements[i-1], sizeof(BTREE_ELEMENT));
        i--;
    }
    if (element->subtree_node_idx != BTREE_INVALID_NODE_IDX)
        btree->nodes[element->subtree_node_idx].parent_node_idx = node_idx;
    memcpy(&btree->nodes[node_idx].elements[i], element, sizeof(BTREE_ELEMENT));
    btree->nodes[node_idx].element_count++;
    return 1;
}

int vector_delete (PBTREE btree, int node_idx, long target_key) {
// delete a single element in the vector belonging to *this node.
// if the target is not found, do not look in subtrees, just return 0.

    int target_pos = -1;
    int first = 1;
    int last = btree->nodes[node_idx].element_count-1;
	int i;
    // perform binary search
    while (last-first > 1) {
        int mid = first+(last-first)/2;
        if (target_key >= btree->nodes[node_idx].elements[mid].key)
            first = mid;
        else
            last = mid;
    }
    if (btree->nodes[node_idx].elements[first].key == target_key)
        target_pos = first;
    else if (btree->nodes[node_idx].elements[last].key == target_key)
        target_pos = last;
    else
        return 0;
    // the element's subtree, if it exists, is to be deleted or re-attached
    // in a different function.  not a concern here.  just shift all the
    // elements in positions greater than target_pos.
    for (i=target_pos; i < btree->nodes[node_idx].element_count; i++)
        memcpy(&btree->nodes[node_idx].elements[i], &btree->nodes[node_idx].elements[i+1], sizeof(BTREE_ELEMENT));

    btree->nodes[node_idx].element_count--;
    return 1;
}

int vector_delete_pos (PBTREE btree, int node_idx, int target_pos) {
// delete a single element in the vector belonging to *this node.
// the element is identified by position, not value.
	int i;

    if (target_pos < 0 || target_pos >= btree->nodes[node_idx].element_count)
        return 0;

    // the element's subtree, if it exists, is to be deleted or re-attached
    // in a different function.  not a concern here.  just shift all the
    // elements in positions greater than target_pos.
    for (i=target_pos; i<btree->nodes[node_idx].element_count; i++)
        memcpy(&btree->nodes[node_idx].elements[i], &btree->nodes[node_idx].elements[i+1], sizeof(BTREE_ELEMENT));

    btree->nodes[node_idx].element_count--;
    return 1;
}

int vector_insert_for_split (PBTREE btree, int node_idx, PBTREE_ELEMENT element) {
// this method insert an element that is in excess of the nominal capacity of
// the node, using the extra slot that always remains unused during normal
// insertions.  this method should only be called from split_insert()
	int i;

    if (btree->nodes[node_idx].element_count >= BTREE_MAX_ELEMENTS) // error
        return 0;
    i = btree->nodes[node_idx].element_count;

    while (i>0 && btree->nodes[node_idx].elements[i-1].key > element->key) {
        memcpy(&btree->nodes[node_idx].elements[i], &btree->nodes[node_idx].elements[i-1], sizeof(BTREE_ELEMENT));
        i--;
    }
	if (element->subtree_node_idx)
        btree->nodes[element->subtree_node_idx].parent_node_idx = node_idx;
    memcpy(&btree->nodes[node_idx].elements[i], element, sizeof(BTREE_ELEMENT));
    btree->nodes[node_idx].element_count++;
    return 1;
}

int split_insert (PBTREE btree, int node_idx, PBTREE_ELEMENT element) {
	int i;
	int split_point;
	int new_node_idx;
	BTREE_ELEMENT upward_element;

    // split_insert should only be called if node is full
    if (btree->nodes[node_idx].element_count != BTREE_MAX_ELEMENTS-1)
        return 0;

    vector_insert_for_split (btree, node_idx, element);
    split_point = btree->nodes[node_idx].element_count/2;
    if (2*split_point < btree->nodes[node_idx].element_count)  // perform the "ceiling function"
        split_point++;
    // new node receives the rightmost half of elements in *this node
    new_node_idx = get_free_node(btree);
    if (!BTREE_IS_VALID_NODE_IDX(new_node_idx))
        return 0;
        
    memcpy(&upward_element, &btree->nodes[node_idx].elements[split_point], sizeof(BTREE_ELEMENT));
    insert_zeroth_subtree (btree, new_node_idx, upward_element.subtree_node_idx);
    upward_element.subtree_node_idx = new_node_idx;
    // element that gets added to the parent of this node
    for (i=1; i<btree->nodes[node_idx].element_count-split_point; i++)
        vector_insert(btree, new_node_idx, &btree->nodes[node_idx].elements[split_point+i]);
    btree->nodes[new_node_idx].element_count = btree->nodes[node_idx].element_count-split_point;
    btree->nodes[node_idx].element_count = split_point;
    btree->nodes[new_node_idx].parent_node_idx = btree->nodes[node_idx].parent_node_idx;

    // now insert the new node into the parent, splitting it if necessary
    if (BTREE_IS_VALID_NODE_IDX(btree->nodes[node_idx].parent_node_idx) &&
            vector_insert(btree, btree->nodes[node_idx].parent_node_idx, &upward_element))
        return 1;
    else if (BTREE_IS_VALID_NODE_IDX(btree->nodes[node_idx].parent_node_idx) &&
            split_insert(btree, btree->nodes[node_idx].parent_node_idx, &upward_element))
        return 1;
    else if (!BTREE_IS_VALID_NODE_IDX(btree->nodes[node_idx].parent_node_idx)) { // this node was the root
        int new_root = get_free_node(btree);
        insert_zeroth_subtree(btree, new_root, node_idx);
        btree->nodes[node_idx].parent_node_idx = new_root;
        btree->nodes[new_node_idx].parent_node_idx = new_root;
        vector_insert (btree, new_root, &upward_element);
        btree->root_node_idx = new_root;
        btree->nodes[new_root].parent_node_idx = BTREE_INVALID_NODE_IDX;
    }
    return 1;

}

int btree_insert (PBTREE btree, PBTREE_ELEMENT element) {
    int last_visited_node_idx;

	if (!BTREE_IS_VALID_NODE_IDX(btree->root_node_idx))
    {
        int node_idx = get_free_node(btree);
        if (!BTREE_IS_VALID_NODE_IDX(node_idx))
            return 0;
        else
        {
            set_root(btree, node_idx);
            insert_zeroth_subtree (btree, node_idx, BTREE_INVALID_NODE_IDX);
        }
    }

    last_visited_node_idx = btree->root_node_idx;
	if (btree_search_ex(btree, &last_visited_node_idx, element->key))  // element already in tree
        return 0;
    if (vector_insert(btree, last_visited_node_idx, element))
        return 1;
    return split_insert(btree, last_visited_node_idx, element);
}

int btree_delete_ex (PBTREE btree, int node_idx, long target_key) {
// target is just a package for the key value.  the reference does not
// provide the address of the Elem instance to be deleted.

    // first find the node contain the Elem instance with the given key
    int parent_index_this = BTREE_INVALID_ELEMENT_IDX;
    PBTREE_ELEMENT found;
	int last_visted_node_idx;
	if (node_idx == BTREE_INVALID_NODE_IDX)
		node_idx = btree->root_node_idx;
	last_visted_node_idx = node_idx;
	found = btree_search_ex (btree, &last_visted_node_idx, target_key);
    if (!found)
        return 0;

    if (is_leaf(btree, last_visted_node_idx) && key_count(btree, last_visted_node_idx) > btree_minimum_keys())
        return vector_delete (btree, last_visted_node_idx, target_key);
    else if (is_leaf(btree, last_visted_node_idx)) {
        vector_delete (btree, last_visted_node_idx, target_key);
        // loop invariant: if _node_ is not null_ptr, it points to a node
        // that has lost an element and needs to import one from a sibling
        // or merge with a sibling and import one from its parent.
        // after an iteration of the loop, _node_ may become null or
        // it may point to its parent if an element was imported from the
        // parent and this caused the parent to fall below the minimum
        // element count.
        while (BTREE_IS_VALID_NODE_IDX(last_visted_node_idx)) {
            int right, left;
            // NOTE: the "this" pointer may no longer be valid after the first
            // iteration of this loop!!!
            if (last_visted_node_idx == find_root(btree) && is_leaf(btree, last_visted_node_idx))
                break;
            if (last_visted_node_idx == find_root(btree) && !is_leaf(btree, last_visted_node_idx)) // sanity check
                return 0;
            // is an extra element available from the right sibling (if any)
            right = right_sibling(btree, last_visted_node_idx, &parent_index_this);
            if (BTREE_IS_VALID_NODE_IDX(right) && key_count(btree, right) > btree_minimum_keys())
                last_visted_node_idx = rotate_from_right(btree, last_visted_node_idx, parent_index_this);
            else {
                // is an extra element available from the left sibling (if any)
                left = left_sibling(btree, last_visted_node_idx, &parent_index_this);
                if (BTREE_IS_VALID_NODE_IDX(left) && key_count(btree, left) > btree_minimum_keys())
                    last_visted_node_idx = rotate_from_left(btree, last_visted_node_idx, parent_index_this);
                else if (BTREE_IS_VALID_NODE_IDX(right))
                    last_visted_node_idx = merge_right(btree, last_visted_node_idx, parent_index_this);
                else if (BTREE_IS_VALID_NODE_IDX(left))
                    last_visted_node_idx = merge_left(btree, last_visted_node_idx, parent_index_this);
            }
        }
    }
    else {
        PBTREE_ELEMENT smallest_in_subtree = smallest_key_in_subtree(btree, found->subtree_node_idx);
        found->key = smallest_in_subtree->key;
        found->data_entry_idx = smallest_in_subtree->data_entry_idx;
        btree_delete_ex (btree, found->subtree_node_idx, smallest_in_subtree->key);
    }
    return 1;
}

int btree_delete (PBTREE btree, long target_key) {
    return btree_delete_ex (btree, BTREE_INVALID_NODE_IDX, target_key);
}

int rotate_from_right(PBTREE btree, int node_idx, int parent_index_this) {
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    // new element to be added to this node
    BTREE_ELEMENT underflow_filler;
	memcpy(&underflow_filler, &btree->nodes[parent_node_idx].elements[parent_index_this+1], sizeof(BTREE_ELEMENT));
    // right sibling of this node
    int right_sib = btree->nodes[parent_node_idx].elements[parent_index_this+1].subtree_node_idx;
    underflow_filler.subtree_node_idx = btree->nodes[right_sib].elements[0].subtree_node_idx;
    // copy the entire element
    memcpy(&btree->nodes[parent_node_idx].elements[parent_index_this+1], &btree->nodes[right_sib].elements[1], sizeof(BTREE_ELEMENT));
    // now restore correct pointer
    btree->nodes[parent_node_idx].elements[parent_index_this+1].subtree_node_idx = right_sib;
    vector_insert (btree, node_idx, &underflow_filler);
    vector_delete_pos(btree, right_sib, 0);
	btree->nodes[right_sib].elements[0].key = btree->invalid_key;
    btree->nodes[right_sib].elements[0].data_entry_idx = BTREE_INVALID_ENTRY_IDX;
    return BTREE_INVALID_NODE_IDX; // parent node still has same element count
}

int rotate_from_left(PBTREE btree, int node_idx, int parent_index_this) {
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    // new element to be added to this node
    BTREE_ELEMENT underflow_filler;
	memcpy(&underflow_filler, &btree->nodes[parent_node_idx].elements[parent_index_this], sizeof(BTREE_ELEMENT));
    // left sibling of this node
    int left_sib = btree->nodes[parent_node_idx].elements[parent_index_this-1].subtree_node_idx;
    underflow_filler.subtree_node_idx = btree->nodes[left_sib].elements[0].subtree_node_idx;
    btree->nodes[node_idx].elements[0].subtree_node_idx = btree->nodes[left_sib].elements[btree->nodes[left_sib].element_count-1].subtree_node_idx;
    if (BTREE_IS_VALID_NODE_IDX(btree->nodes[node_idx].elements[0].subtree_node_idx))
        btree->nodes[btree->nodes[node_idx].elements[0].subtree_node_idx].parent_node_idx = node_idx;
    // copy the entire element
     memcpy(&btree->nodes[parent_node_idx].elements[parent_index_this], &btree->nodes[left_sib].elements[btree->nodes[left_sib].element_count-1], sizeof(BTREE_ELEMENT));
   // now restore correct pointer
    btree->nodes[parent_node_idx].elements[parent_index_this].subtree_node_idx = node_idx;
    vector_insert (btree, node_idx, &underflow_filler);
    vector_delete_pos(btree, left_sib, btree->nodes[left_sib].element_count-1);
    return BTREE_INVALID_NODE_IDX; // parent node still has same element count
}

int merge_right (PBTREE btree, int node_idx, int parent_index_this) {
// copy elements from the right sibling into this node, along with the
// element in the parent node vector that has the right sibling as it subtree.
// the right sibling and that parent element are then deleted
	int i;
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    BTREE_ELEMENT parent_elem;
	memcpy(&parent_elem, &btree->nodes[parent_node_idx].elements[parent_index_this+1], sizeof(BTREE_ELEMENT));
    int right_sib = btree->nodes[parent_node_idx].elements[parent_index_this+1].subtree_node_idx;
    parent_elem.subtree_node_idx = btree->nodes[right_sib].elements[0].subtree_node_idx;
    vector_insert (btree, node_idx, &parent_elem);
    for (i=1; i<btree->nodes[right_sib].element_count; i++)
        vector_insert (btree, node_idx, &btree->nodes[right_sib].elements[i]);
    vector_delete_pos (btree, parent_node_idx, parent_index_this+1);
    delete_node(btree, right_sib);
    if (parent_node_idx == find_root(btree) && !key_count(btree, parent_node_idx)) {
        set_root(btree, node_idx);
        delete_node(btree, parent_node_idx);
		btree->nodes[node_idx].parent_node_idx = BTREE_INVALID_NODE_IDX;
        return BTREE_INVALID_NODE_IDX;
    }
    else if (parent_node_idx == find_root(btree) && key_count(btree, parent_node_idx))
        return BTREE_INVALID_NODE_IDX;
    if (BTREE_IS_VALID_NODE_IDX(parent_node_idx) && key_count(btree, parent_node_idx) >= btree_minimum_keys())
        return BTREE_INVALID_NODE_IDX; // no need for parent to import an element
    return parent_node_idx; // parent must import an element
}

int merge_left (PBTREE btree, int node_idx, int parent_index_this) {
// copy all elements from this node into the left sibling, along with the
// element in the parent node vector that has this node as its subtree.
// this node and its parent element are then deleted.
    int i;
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    BTREE_ELEMENT parent_elem;
	memcpy(&parent_elem, &btree->nodes[parent_node_idx].elements[parent_index_this], sizeof(BTREE_ELEMENT));
    int left_sib = btree->nodes[parent_node_idx].elements[parent_index_this-1].subtree_node_idx;
    parent_elem.subtree_node_idx = btree->nodes[node_idx].elements[0].subtree_node_idx;
    vector_insert (btree, left_sib, &parent_elem);
    for (i=1; i<btree->nodes[node_idx].element_count; i++)
        vector_insert (btree, left_sib, &btree->nodes[node_idx].elements[i]);
    vector_delete_pos (btree, parent_node_idx, parent_index_this);
    if (parent_node_idx==find_root(btree) && !key_count(btree, parent_node_idx)) {
        set_root(btree, left_sib);
        delete_node(btree, parent_node_idx);
        btree->nodes[left_sib].parent_node_idx = BTREE_INVALID_NODE_IDX;
        delete_node(btree, node_idx);
        return BTREE_INVALID_NODE_IDX;
    }
    else if (parent_node_idx==find_root(btree) && key_count(btree, parent_node_idx)) {
        delete_node(btree, node_idx);
        return BTREE_INVALID_NODE_IDX;
    }
    delete_node(btree, node_idx);
    if (key_count(btree, parent_node_idx) >= btree_minimum_keys())
        return BTREE_INVALID_NODE_IDX; // no need for parent to import an element
    return parent_node_idx; // parent must import an element
}

int right_sibling (PBTREE btree, int node_idx, int *parent_index_this) {
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    *parent_index_this = index_has_subtree (btree, node_idx); // for element with THIS as subtree
    if (!BTREE_IS_VALID_ELEMENT_IDX(*parent_index_this))
        return BTREE_INVALID_NODE_IDX;
    // now mp_parent is known not to be null
    if (*parent_index_this >= btree->nodes[parent_node_idx].element_count-1)
        return BTREE_INVALID_NODE_IDX;  // no right sibling
    return btree->nodes[parent_node_idx].elements[*parent_index_this+1].subtree_node_idx;  // might be null
}

int left_sibling (PBTREE btree, int node_idx, int *parent_index_this) {
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    *parent_index_this = index_has_subtree (btree, node_idx); // for element with THIS as subtree
    if (!BTREE_IS_VALID_ELEMENT_IDX(*parent_index_this))
        return BTREE_INVALID_NODE_IDX;
    // now mp_parent is known not to be null
    if (*parent_index_this==0)
        return BTREE_INVALID_NODE_IDX;  // no left sibling
    return btree->nodes[parent_node_idx].elements[*parent_index_this-1].subtree_node_idx;  // might be null
}

int index_has_subtree (PBTREE btree, int node_idx) {
// return the element in this node's parent that has the "this" pointer as its subtree
    int first;
    int last;
    int parent_node_idx = btree->nodes[node_idx].parent_node_idx;
    if (!BTREE_IS_VALID_NODE_IDX(parent_node_idx))
        return BTREE_INVALID_NODE_IDX;
    first = 0;
    last = btree->nodes[parent_node_idx].element_count-1;
    while (last-first > 1) {
        int mid = first+(last-first)/2;
        PBTREE_ELEMENT smallest = smallest_key(btree, node_idx);
        if (smallest->key >= btree->nodes[parent_node_idx].elements[mid].key)
            first = mid;
        else
            last = mid;
    }
    if (btree->nodes[parent_node_idx].elements[first].subtree_node_idx == node_idx)
        return first;
    else if (btree->nodes[parent_node_idx].elements[last].subtree_node_idx == node_idx)
        return last;
    else
        return BTREE_INVALID_NODE_IDX;
}

PBTREE_ELEMENT smallest_key_in_subtree (PBTREE btree, int node_idx) {
    if (is_leaf(btree, node_idx))
        return &btree->nodes[node_idx].elements[1];
    else
        return smallest_key_in_subtree(btree, btree->nodes[node_idx].elements[0].subtree_node_idx);
}

PBTREE_ELEMENT btree_search (PBTREE btree, long desired_key) {
    int node_idx = btree->root_node_idx;
    return btree_search_ex(btree, &node_idx, desired_key);
}

PBTREE_ELEMENT btree_search_ex (PBTREE btree, int *node_idx, long desired_key) {
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).  the seach starts at the *this node, not
    // at the root of the b-tree.
    int current_node_idx = *node_idx;
    if (!key_count(btree, *node_idx))
        current_node_idx = BTREE_INVALID_NODE_IDX;
    while (BTREE_IS_VALID_NODE_IDX(current_node_idx)) {
        *node_idx = current_node_idx;
        // if desired is less than all values in current node
        if (btree->nodes[current_node_idx].element_count>1 && desired_key < btree->nodes[current_node_idx].elements[1].key)
            current_node_idx = btree->nodes[current_node_idx].elements[0].subtree_node_idx;
        // if desired is greater than all values in current node
        else if (desired_key > btree->nodes[current_node_idx].elements[btree->nodes[current_node_idx].element_count-1].key)
            current_node_idx = btree->nodes[current_node_idx].elements[btree->nodes[current_node_idx].element_count-1].subtree_node_idx;
        else {
            // binary search of the node
            int first = 1;
            int last = btree->nodes[current_node_idx].element_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (desired_key >= btree->nodes[current_node_idx].elements[mid].key)
                    first = mid;
                else
                    last = mid;
            }
            if (btree->nodes[current_node_idx].elements[first].key == desired_key)
                return &btree->nodes[current_node_idx].elements[first];
            if (btree->nodes[current_node_idx].elements[last].key == desired_key)
                return &btree->nodes[current_node_idx].elements[last];
            else if (btree->nodes[current_node_idx].elements[last].key > desired_key)
                current_node_idx = btree->nodes[current_node_idx].elements[first].subtree_node_idx;
            else
                current_node_idx =btree->nodes[current_node_idx].elements[last].subtree_node_idx;
        }
    }

    return NULL;

}

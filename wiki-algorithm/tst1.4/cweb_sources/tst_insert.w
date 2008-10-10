@*Inserting keys with |tst_insert()|.
This function inserts a key into the symbol table. The main idea is to
follow the nodes of the tree until we hit a NULL node. Once we do, we
can skip to |found_null_branch| and allocate nodes freely since we know
that we will not collide with nodes for previously entered keys. If we
end up going through the entire tree without hitting a NULL node, then
the key is either a proper prefix of a previously entered key, or we
have a duplicate key. For the proper prefix, all we have to do is tack
on a terminating node. For the duplicate, if |option| is set to
|TST_REPLACE| we replace the overwite the old data with |data|,
otherwise, we return |TST_DUPLICATE_KEY|. A return value of |TST_ERROR|
indicates a memory allocation failure while trying to grow the node
free list.

5/05/1999 - Change made to |tst_insert|

When an insert has failed we return |TST_DUPLICATE_KEY|, and if we
still want to do anything with the data for that key we have to make a
separate call to |tst_search| to get the pointer which is wasteful. A
new argument |exist_ptr| has been added to |tst_insert|. When
|TST_DUPLICATE_KEY| is returned, |exist_ptr| will contain the data
pointer for the existing key.

11/03/1999 - Change made to |tst_insert|

If a |NULL| is passed as the |exist_ptr| argument bad things could
happen. Before setting this pointer with an existing item, it must be
checked to see if it is |NULL|. In addition, previously a call to
|tst_insert()| with the |TST_REPLACE| argument specified would NOT
return the existing data for the key. Now, the existing data pointer is
placed in |exist_ptr| before it is overwritten. The check for a |NULL|
|exist_ptr| is done there as well.

@(tst_insert.c@>=
#include "tst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

@<Declaration for |tst_grow_node_free_list()|@>

int tst_insert(const unsigned char *key, void *data, struct tst *tst, int option, void **exist_ptr)
{
   struct node *current_node;
   struct node *new_node_tree_begin = NULL;
   struct node* new_node;
   int key_index;
   int perform_loop = 1;

   @<Check for NULL key@>
   @<Check head entry to see if it is NULL@>
   @<Traverse tree when head entry is not NULL@>
   @<Found null branch so insert rest of key@>
}

@
The first thing we need to do is check for a NULL, or zero length key,
which is an error.
@<Check for NULL key@>=
if (key == NULL)
   return TST_NULL_KEY;

if(key[0] == 0)
   return TST_NULL_KEY;

@
Here, we look at the first character of |key|, and use it to index into
|tst->head|. If the indexed node is NULL, then we know that this key is
not in the tree. The entries in |tst->head| represent all of the
possible starting points for keys. The actual node in the |head| array
store the value of the {\it second} character of |key|, because the
first character is indicated implicitly by |head[key[0]]| not being
NULL. This is why we set the |value| member of the very first node to
|key[1]|.

If we the head entry is NULL, then there are several things we must
perform. First, we have to allocate a node then set the |value| member
to |key[1]|. Then we have to check the length of the key. If the length
is 1, then we set the |middle| pointer to |data| and return |TST_OK|.
Otherwise, we set |perform_loop| to 0 to disable the loop and insert
the rest of the key.
@<Check head entry to see if it is NULL@>=
if(tst->head[(int)key[0]] == NULL)
{
   @<Check |tst->free_list| and grow if necessary@>
   tst->head[(int)key[0]] = tst->free_list;
   @<Update free list after taking a node@>
   current_node = tst->head[(int)key[0]];
   current_node->value = key[1];
   if(key[1] == 0)
   {
      current_node->middle = data;
      return TST_OK;
   }
   else
      perform_loop = 0;
}

@
All we do here is traverse the tree based on characters in |key|. We
handle cases where we have to take the left, middle or right branch,
and the code for each is explained in their own section. The odd
looking test for the left and right branches is there so we can avoid
one way branching at terminating nodes. If we are at a terminating
node, then we take a branch by comparing the character in |key| with
64, which is basically the 127 valid ASCII characters divided by 2. If
the node is not a terminating node, then we can just compare the
character in |key| with |current_node->value|.
11/03/1999 - Change made to |tst_insert|
@<Traverse tree when head entry is not NULL@>=
current_node = tst->head[(int)key[0]];
key_index = 1;
while(perform_loop == 1)
{
   if(key[key_index] == current_node->value)
   {
      @<Key is equal to node value@>
   }

	if (key[key_index] == 0)
	{
		@<Key is a proper prefix of an existing entry@>
	}
	
   if( ((current_node->value == 0) && (key[key_index] < 64)) ||
      ((current_node->value != 0) && (key[key_index] <
      current_node->value)) )
   {
      @<Key is less than node value@>
   }
   else
   {
      @<Key is greater than node value@>
   }
}

@
Wow, the first change since 1999. This particular change is needed
to support longest match lookups. Basically, if we are going to be
adding a string that is a proper prefix of an existing entry (like
test in testing) then we need to store the trailing NULL of test
at the point where it mismatches with the i in testing. In older
versions when test is added after testing the trailing NULL of test
ends up in a left branch where the longest match search doesn't see
it. To make this happen, we create a single new node. We then copy
the data from the mismatched node |current_node| to this new node.
Then set |current_node->value| to 0, and calculate which branch the
new node should go on.
@<Key is a proper prefix of an existing entry@>=
@<Check |tst->free_list| and grow if necessary@>
new_node = tst->free_list;
@<Update free list after taking a node@>
memcpy((void*)new_node, (void*)current_node, sizeof(struct node));
current_node->value = 0;
if (new_node->value < 64)
{
	current_node->left = new_node;
	current_node->right = '\0';
}
else
{
	current_node->left = '\0';
	current_node->right = new_node;
}

current_node->middle = data;
return TST_OK;

@
When the value of |key[key_index]| is equal to |current_node->value|,
we must check first to see if we are looking at the NULL terminator for
the string. It so, we have a duplicate key, and return
|TST_DUPLICATE_KEY| unlees the |option| is set to |TST_REPLACE|, and in
that case we replace overwrite the old data with |data|.

If we are not looking at the NULL terminator, then we have to check the
|middle| pointer of the current node to see if it is NULL. If it is, we
allocate a node, set the pointers, and break out of the loop. If the
|middle| pointer is not NULL, then we increment |key_index| and set
|current_node| to |current_node->middle|.
@<Key is equal to node value@>=
if(key[key_index] == 0)
{
   if (option == TST_REPLACE)
   {
      if (exist_ptr != NULL)
         *exist_ptr = current_node->middle;

      current_node->middle = data;
      return TST_OK;
   }
   else
   {
      if (exist_ptr != NULL)
         *exist_ptr = current_node->middle;
      return TST_DUPLICATE_KEY;
   }
}
else
{
   if(current_node->middle == NULL)
   {
      @<Check |tst->free_list| and grow if necessary@>
      current_node->middle = tst->free_list;
      @<Update free list after taking a node@>
      new_node_tree_begin = current_node;
      current_node = current_node->middle;
      current_node->value = key[key_index];
      break;
   }
   else
   {
      current_node = current_node->middle;
      key_index++;
      continue;
   }
}

@
Here we handle the case when the character |key[key_index]| is less
than |current_node->value|. This means that we need to take the left
branch of the tree. Before we can take this branch, we must check to
see if the left branch is NULL. If it is, allocate a new node, set the
values and break out of the loop. Otherwise, take the branch, and note
that we do not increment |key_index| becuase we are still moving
through the tree, looking for the current character.

If we do happen to allocate a new node for the left branch, we also
have to check if we are at the end of |key|. If so, we set the |middle|
pointer to |data| and return |TST_OK|.
@<Key is less than node value@>=
if (current_node->left == NULL)
{
   @<Check |tst->free_list| and grow if necessary@>
   current_node->left = tst->free_list;
   @<Update free list after taking a node@>
   new_node_tree_begin = current_node;
   current_node = current_node->left;
   current_node->value = key[key_index];
   if(key[key_index] == 0)
   {
      current_node->middle = data;
      return TST_OK;
   }
   else
      break;
}
else
{
   current_node = current_node->left;
   continue;
}

@
Here we handle the case where |key[key_index]| is greater than
|current_node->value|. This means that we need to take the right branch
of the tree. Before we can take this branch, we must check to see if
the right branch is NULL. If it is, allocate a new node, set the values
and break out of the loop. Otherwise, take the branch, and note that we
do not increment |key_index| becuase we are still moving through the
tree, looking for the current character.

Note that in this case we are not checking to see if |key[key_index]| is
0, meaning that we have reached the end of |key|. This is because 0
will always be less than |current_node->value|, and the equality case
in handled in another module.
@<Key is greater than node value@>=
if (current_node->right == NULL)     
{    
   @<Check |tst->free_list| and grow if necessary@>
   current_node->right = tst->free_list;
   @<Update free list after taking a node@>
   new_node_tree_begin = current_node;
   current_node = current_node->right;
   current_node->value = key[key_index];
   break;
}
else
{
   current_node = current_node->right;
   continue;
}

@
When this code is reached, we have broken out of the while loop, so we
must have reached a NULL branch. We insert new nodes into the tree
until the end of the key is reached, then we store |data| and return
success.

03/23/200 There is potentially nasty problem with how this section of
code was implemented. If we get a memory error somewhere in the middle
of adding the new nodes, we return return an error but leave the nodes
we were able to allocate hanging off in space, which can cause lots of
problems. We solve this by saving the node where we hit a |NULL| link
and are going to start adding the rest of the nodes for the key one
after the other. If there is a failure, we put the nodes back on the
free list and reset the middle pointer of the saved node to |NULL|.
@<Found null branch so insert rest of key@>=
do
{
   key_index++;

   if(tst->free_list == NULL)
   {
      if(tst_grow_node_free_list(tst) != 1)
      {
         current_node = new_node_tree_begin->middle;

         while (current_node->middle != NULL)
            current_node = current_node->middle;

         current_node->middle = tst->free_list;
         tst->free_list = new_node_tree_begin->middle;
         new_node_tree_begin->middle = NULL;

         return TST_ERROR;
      }
   }

   @<Check |tst->free_list| and grow if necessary@>
   current_node->middle = tst->free_list;
   @<Update free list after taking a node@>
   current_node = current_node->middle;
   current_node->value = key[key_index];
} while(key[key_index] !=0);

current_node->middle = data;
return TST_OK;

@
This is code that is used throughout this function that checks to see
if |tst->free_list| is empty. If it is, then we call
|tst_grow_node_free_list()|.
@<Check |tst->free_list| and grow if necessary@>=
if(tst->free_list == NULL)
{
   if(tst_grow_node_free_list(tst) != 1)
      return TST_ERROR;
}

@
This is only one line of code, but it is included here as a module to
make it stand out more, so hopefully it will not be forgotten. This
code updates |tst->free_list| to the next node in the free list. This
{\it must} be called after a node is taken off of the free list.
@<Update free list after taking a node@>=
tst->free_list = tst->free_list->middle;

@*Deleting keys with |tst_delete()|.
This is the most complex function of the package. If the |key| is
found, the |data| associated with the key is returned, otherwise the
return value is NULL. The basic task of this function is to find
something I call the |last_branch|. This node is the last node in the
path for a key which has non-NULL children, or is a node branched off
of another. We also have to store the parent of this node, because we
have to NULL the branch that leads to |last_branch|.

\pstree[levelsep=1cm,radius=6pt]
{\Tcircle{t}~[tnpos=r]{t is implied by the existence of |tst->head[key[0]]|}}
{
   \pstree{\Tcircle{e}~[tnpos=r]{this is the actual node stored in |tst->head[key[0]]|}}
   {
      \pstree{\Tcircle{s}}
      {
         \pstree{\Tcircle{t}}
         {
            \pstree{\Tcircle{0}~[tnpos=r]{|last_branch| when deleting ``test''}}
            {
               \Tn
               \Tn
               \pstree{\Tcircle{i}~[tnpos=r]{|last_branch| when deleting ``testing''}}
               {
                  \pstree{\Tcircle{n}}
                  {
                     \pstree{\Tcircle{g}}
                     {
                        \Tcircle{0}
                     }
                  }
               }
            }
         }
      }
   }
}
@(tst_delete.c@>=
#include "tst.h"
#include <stdio.h>
#include <stdlib.h>

void *tst_delete(const unsigned char *key, struct tst *tst)
{
   struct node *current_node;
   struct node *current_node_parent;
   struct node *last_branch;
   struct node *last_branch_parent;
   struct node *next_node;
   struct node *last_branch_replacement;
   struct node *last_branch_dangling_child;
   int key_index;

   @<NULL keys and head nodes return failure@>
   @<Find last branch@>
   if(current_node == NULL)
      return NULL;
   @<Handle key deletion@>
}

@
Here we check for the NULL key, which is not allowed.

We also check the head node to see if it is NULL. If it is, then
we know that the key cannot exist in the tree so we return NULL to
indicate failure.
@<NULL keys and head nodes return failure@>=
if(key[0] == 0)
   return NULL;
if(tst->head[(int)key[0]] == NULL)
   return NULL;

@
Here is where we look for |last_branch|.
@<Find last branch@>=
last_branch = NULL;
last_branch_parent = NULL;
current_node = tst->head[(int)key[0]];
current_node_parent = NULL;
key_index = 1;
while(current_node != NULL)
{
   if(key[key_index] == current_node->value)
   {
      @<Check node for branches@>
      if(key[key_index] == 0)
         break;
      else
      {
         current_node_parent = current_node;
         current_node = current_node->middle;
         key_index++;
         continue;
      }
   }
   else if( ((current_node->value == 0) && (key[key_index] < 64)) ||
      ((current_node->value != 0) && (key[key_index] <
      current_node->value)) )
   {
      last_branch_parent = current_node;
      current_node_parent = current_node;
      current_node = current_node->left;
      last_branch = current_node;
      continue;
   }
   else
   {
      last_branch_parent = current_node;
      current_node_parent = current_node;
      current_node = current_node->right;
      last_branch = current_node;
      continue;
   }

}

@
Here we check whether one or both of the children of |current_node| are
not NULL, which means that |key| up to this point is a proper prefix of
another key in the tree, so we can delete this node, but we have to
balance the tree first. We therefore set |last_branch| to
|current_node| and |last_branch_parent| to |current_node_parent|.
@<Check node for branches@>=
if( (current_node->left != NULL) || (current_node->right != NULL) )
{
   last_branch = current_node;
   last_branch_parent = current_node_parent;
}

@
@<Handle key deletion@>=
if(last_branch == NULL)
{
   @<|last_branch| is NULL so we can remove the whole key and set the head to NULL@>
}
else if( (last_branch->left == NULL) && (last_branch->right == NULL) )
{
   @<Both children are NULL so we can delete from |last_branch|@>
}
else
{
   @<Determine values for |last_branch_replacement| and |last_branch_dangling_child|@>
   @<Deal with case where |last_branch_parent| is NULL@>
   @<Move |last_branch_dangling_child| to new slot in left subtree of |last_branch_replacement|@>
}
@<Free nodes from |next_node| onward and return data@>

@
When |last_branch| is NULL, we set |next_node| to the head node, NULL
the head, then fall through the statements so we can remove the entire
key.
@<|last_branch| is NULL so we can remove the whole key and set the head to NULL@>=
   next_node = tst->head[(int)key[0]];
   tst->head[(int)key[0]] = NULL;

@
When both children of |last_branch| are NULL, we can safely remove all
nodes from that point on without having to balance any other nodes. All
we have to do is set the path out of |last_branch_parent| to NULL.
@<Both children are NULL so we can delete from |last_branch|@>=
if(last_branch_parent->left == last_branch)
   last_branch_parent->left = NULL;
else
   last_branch_parent->right = NULL;

next_node = last_branch;

@
At this point we know that |last_branch| has one or more children, so
we have to move nodes around before we can start deleting them. Since
the node at |last_branch| is going to be removed, we have the variable
|last_branch_replacement|. When both children are valid, we arbitrarily
set this to the right child, otherwise, we set it to the child that is
not NULL. Also in the case where both children are valid, we use the
variable |last_branch_dangling_child| to store the extra child.
@<Determine values for |last_branch_replacement| and |last_branch_dangling_child|@>=
if( (last_branch->left != NULL) && (last_branch->right != NULL) )
{
   last_branch_replacement = last_branch->right;
   last_branch_dangling_child = last_branch->left;
}
else if(last_branch->right != NULL)
{
   last_branch_replacement = last_branch->right;
   last_branch_dangling_child = NULL;
}
else
{
   last_branch_replacement = last_branch->left;
   last_branch_dangling_child = NULL;
}

@
If |last_branch_parent| is NULL, then wee have a situation where
|last_branch| is actually equal to |tst->head[key[0]]|, or in other
words, it is the head node and we need to handle this in a special way.
We do this by setting the head node to |last_branch_replacement|. On
the other hand, if |last_branch_parent| is not NULL, then we need to
find which path was taken out of |last_branch_parent| to |last_branch|.
We set this path, or rather pointer, to |last_branch_replacement|.
@<Deal with case where |last_branch_parent| is NULL@>=
if(last_branch_parent == NULL)
   tst->head[(int)key[0]]=last_branch_replacement;
else
{
   if (last_branch_parent->left == last_branch)
      last_branch_parent->left = last_branch_replacement;
   else if (last_branch_parent->right == last_branch)
      last_branch_parent->right = last_branch_replacement;
   else
      last_branch_parent->middle = last_branch_replacement;
}

@
At this point we have replaced |last_branch| with
|last_branch_replacement| in the tree, and now we have to handle the
case where both children of |last_branch| were valid. If
|last_branch_dangling_child| is NULL, then we have nothing to do.
Otherwise, we need to find an open slot in the left subtree of
|last_branch_replacement| to put |last_branch_dangling_child|.
@<Move |last_branch_dangling_child| to new slot in left subtree of |last_branch_replacement|@>=
if(last_branch_dangling_child != NULL)
{
   current_node = last_branch_replacement;

   while (current_node->left != NULL)
      current_node = current_node->left;

   current_node->left = last_branch_dangling_child;
}

next_node = last_branch;

@
This puts the nodes back on the free list and returns the data
associated with a key. To use, set |next_node| to the value of
|last_branch| or whichever node the deletion needs to
start from.
@<Free nodes from |next_node| onward and return data@>=
do
{
   current_node = next_node;
   next_node = current_node->middle;
   @<Return node to free list@>
}
while(current_node->value != 0);

return next_node;

@
This code returns a node to the free list and makes sure that the child
pointers are set to NULL.
@<Return node to free list@>=
current_node->left = NULL;
current_node->right = NULL;
current_node->middle = tst->free_list;
tst->free_list = current_node;

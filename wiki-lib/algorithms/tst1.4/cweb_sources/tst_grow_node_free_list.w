@*Growing the free list with |tst_grow_node_free_list()|.
@(tst_grow_node_free_list.c@>=
#include "tst.h"
#include <stdio.h>
#include <stdlib.h>

int tst_grow_node_free_list(struct tst *tst)
{
   struct node *current_node;
   struct node_lines *new_line;
   int i;

   @<Allocate |tst->node_lines->next|@>
   @<Allocate the |node_line| member of |tst->node_lines->next|@>
   @<Add the nodes from |node_line| to |tst->free_list|@>
}

@
Allocate a struct |node_lines| to fill |new_line|. We do this so that
we can insert the new structure at the beginning of the linked list.
If the allocation fails we return |TST_ERROR|.  We do not reset
|tst->node_lines| until all of the other allocations have completed
successfully.
@<Allocate |tst->node_lines->next|@>=
if((new_line = (struct node_lines *) malloc(sizeof(struct node_lines))) == NULL)
   return TST_ERROR;

@
Now that we have a new |node_lines| placeholder, we allocate its
|node_line| member with the number of nodes specified in
|tst->node_line_width|. If this fails, we have to deallocate the
|node_lines| structure we just allocated, and return |TST_ERROR|.
If the allocation goes okay, we can then update |tst->node_lines|.
@<Allocate the |node_line| member of |tst->node_lines->next|@>=
if((new_line->node_line = (struct node *)
calloc(tst->node_line_width, sizeof(struct node))) == NULL)
{
   free(new_line);
   return TST_ERROR;
}
else
{
   new_line->next = tst->node_lines;
   tst->node_lines = new_line;
}

@
Finally, we need to step through |tst->node_lines->node_line| and
insert the nodes into |tst->free_list|. We use the local variable
|current_node| to move the pointers from |node_line| to
|tst->free_list|. Note the essential assumption that |free_list|
is empty. Therefore, allocation of nodes with this function must only
be done when the free list is empty. When done, we set the last pointer
to NULL so we know when the list is empty later, and return 1 to
indicate true.
@<Add the nodes from |node_line| to |tst->free_list|@>=
current_node = tst->node_lines->node_line;
tst->free_list = current_node;
for (i = 1; i < tst->node_line_width; i++)
{
   current_node->middle = &(tst->node_lines->node_line[i]);
   current_node = current_node->middle;
}
current_node->middle = NULL;
return 1;

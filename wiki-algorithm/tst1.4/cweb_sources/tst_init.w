@*Initialization with |tst_init()|.
@(tst_init.c@>=
#include "tst.h"
#include <stdio.h>
#include <stdlib.h>

struct tst *tst_init(int width)
{
   struct tst *tst; 
   struct node *current_node;
   int i;

@<Allocate tst structure@>
@<Allocate |node_lines| member@>
@<Set |node_line_width| and allocate first chunk of nodes@>
@<Build free list from just allocated |node_line|@>
}

@
Allocate space for the |struct tst|. If this fails we return NULL;
@<Allocate tst structure@>=
if((tst = (struct tst *) calloc(1, sizeof(struct tst))) == NULL)
   return NULL;

@
Allocate space for the |node_lines| member of |struct tst|. If this
fails we have to free our |struct tst| and return NULL;
@<Allocate |node_lines| member@>=
if ((tst->node_lines = (struct node_lines *) calloc(1, sizeof(struct node_lines))) == NULL)
{
   free(tst);
   return NULL;
}

@
After we have our tst structure and the |node_lines| member allocated,
we need to set the |node_line_width| member for this first chunk of
nodes as well as further allocations. If we fail to allocate our chunk
of nodes, we must free our |struct tst| as well as the |node_lines|
member and return NULL.
@<Set |node_line_width| and allocate first chunk of nodes@>=
tst->node_line_width = width;
tst->node_lines->next = NULL;
if ((tst->node_lines->node_line = (struct node *) calloc(width, sizeof(struct node))) == NULL)
{
   free(tst->node_lines);
   free(tst);
   return NULL;
}

@
Now we have to step through the just allocated |node_line| and link
them together in a linked list fashion. Then we set |tst->free_list| to
the first node. Finally, we return a pointer to the new |struct tst|.
@<Build free list from just allocated |node_line|@>=
current_node = tst->node_lines->node_line;
tst->free_list = current_node;
for (i = 1; i < width; i++)
{
   current_node->middle = &(tst->node_lines->node_line[i]);
   current_node = current_node->middle;
}
current_node->middle = NULL;
return tst;

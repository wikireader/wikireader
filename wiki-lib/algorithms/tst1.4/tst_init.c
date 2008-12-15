/*12:*/
#line 2 "./tst_init.w"

#include "tst.h"
#include <stdio.h> 
#include <stdlib.h> 

struct tst*tst_init(int width)
{
struct tst*tst;
struct node*current_node;
int i;

/*13:*/
#line 21 "./tst_init.w"

if((tst= (struct tst*)calloc(1,sizeof(struct tst)))==NULL)
return NULL;

/*:13*/
#line 13 "./tst_init.w"

/*14:*/
#line 28 "./tst_init.w"

if((tst->node_lines= (struct node_lines*)calloc(1,sizeof(struct node_lines)))==NULL)
{
free(tst);
return NULL;
}

/*:14*/
#line 14 "./tst_init.w"

/*15:*/
#line 41 "./tst_init.w"

tst->node_line_width= width;
tst->node_lines->next= NULL;
if((tst->node_lines->node_line= (struct node*)calloc(width,sizeof(struct node)))==NULL)
{
free(tst->node_lines);
free(tst);
return NULL;
}

/*:15*/
#line 15 "./tst_init.w"

/*16:*/
#line 55 "./tst_init.w"

current_node= tst->node_lines->node_line;
tst->free_list= current_node;
for(i= 1;i<width;i++)
{
current_node->middle= &(tst->node_lines->node_line[i]);
current_node= current_node->middle;
}
current_node->middle= NULL;
return tst;
#line 1 "./tst_grow_node_free_list.w"
/*:16*/
#line 16 "./tst_init.w"

}

/*:12*/

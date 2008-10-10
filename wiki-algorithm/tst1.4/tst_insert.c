/*21:*/
#line 34 "./tst_insert.w"

#include "tst.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

/*10:*/
#line 160 "./tst.w"

int tst_grow_node_free_list(struct tst*tst);

/*:10*/
#line 40 "./tst_insert.w"


int tst_insert(const unsigned char*key,void*data,struct tst*tst,int option,void**exist_ptr)
{
struct node*current_node;
struct node*new_node_tree_begin= NULL;
struct node*new_node;
int key_index;
int perform_loop= 1;

/*22:*/
#line 59 "./tst_insert.w"

if(key==NULL)
return TST_NULL_KEY;

if(key[0]==0)
return TST_NULL_KEY;

/*:22*/
#line 50 "./tst_insert.w"

/*23:*/
#line 82 "./tst_insert.w"

if(tst->head[(int)key[0]]==NULL)
{
/*30:*/
#line 334 "./tst_insert.w"

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
return TST_ERROR;
}

/*:30*/
#line 85 "./tst_insert.w"

tst->head[(int)key[0]]= tst->free_list;
/*31:*/
#line 346 "./tst_insert.w"

tst->free_list= tst->free_list->middle;
#line 1 "./tst_search.w"
/*:31*/
#line 87 "./tst_insert.w"

current_node= tst->head[(int)key[0]];
current_node->value= key[1];
if(key[1]==0)
{
current_node->middle= data;
return TST_OK;
}
else
perform_loop= 0;
}

/*:23*/
#line 51 "./tst_insert.w"

/*24:*/
#line 110 "./tst_insert.w"

current_node= tst->head[(int)key[0]];
key_index= 1;
while(perform_loop==1)
{
if(key[key_index]==current_node->value)
{
/*26:*/
#line 181 "./tst_insert.w"

if(key[key_index]==0)
{
if(option==TST_REPLACE)
{
if(exist_ptr!=NULL)
*exist_ptr= current_node->middle;

current_node->middle= data;
return TST_OK;
}
else
{
if(exist_ptr!=NULL)
*exist_ptr= current_node->middle;
return TST_DUPLICATE_KEY;
}
}
else
{
if(current_node->middle==NULL)
{
/*30:*/
#line 334 "./tst_insert.w"

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
return TST_ERROR;
}

/*:30*/
#line 203 "./tst_insert.w"

current_node->middle= tst->free_list;
/*31:*/
#line 346 "./tst_insert.w"

tst->free_list= tst->free_list->middle;
#line 1 "./tst_search.w"
/*:31*/
#line 205 "./tst_insert.w"

new_node_tree_begin= current_node;
current_node= current_node->middle;
current_node->value= key[key_index];
break;
}
else
{
current_node= current_node->middle;
key_index++;
continue;
}
}

/*:26*/
#line 117 "./tst_insert.w"

}

if(key[key_index]==0)
{
/*25:*/
#line 149 "./tst_insert.w"

/*30:*/
#line 334 "./tst_insert.w"

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
return TST_ERROR;
}

/*:30*/
#line 150 "./tst_insert.w"

new_node= tst->free_list;
/*31:*/
#line 346 "./tst_insert.w"

tst->free_list= tst->free_list->middle;
#line 1 "./tst_search.w"
/*:31*/
#line 152 "./tst_insert.w"

memcpy((void*)new_node,(void*)current_node,sizeof(struct node));
current_node->value= 0;
if(new_node->value<64)
{
current_node->left= new_node;
current_node->right= '\0';
}
else
{
current_node->left= '\0';
current_node->right= new_node;
}

current_node->middle= data;
return TST_OK;

/*:25*/
#line 122 "./tst_insert.w"

}

if(((current_node->value==0)&&(key[key_index]<64))||
((current_node->value!=0)&&(key[key_index]<
current_node->value)))
{
/*27:*/
#line 231 "./tst_insert.w"

if(current_node->left==NULL)
{
/*30:*/
#line 334 "./tst_insert.w"

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
return TST_ERROR;
}

/*:30*/
#line 234 "./tst_insert.w"

current_node->left= tst->free_list;
/*31:*/
#line 346 "./tst_insert.w"

tst->free_list= tst->free_list->middle;
#line 1 "./tst_search.w"
/*:31*/
#line 236 "./tst_insert.w"

new_node_tree_begin= current_node;
current_node= current_node->left;
current_node->value= key[key_index];
if(key[key_index]==0)
{
current_node->middle= data;
return TST_OK;
}
else
break;
}
else
{
current_node= current_node->left;
continue;
}

/*:27*/
#line 129 "./tst_insert.w"

}
else
{
/*28:*/
#line 267 "./tst_insert.w"

if(current_node->right==NULL)
{
/*30:*/
#line 334 "./tst_insert.w"

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
return TST_ERROR;
}

/*:30*/
#line 270 "./tst_insert.w"

current_node->right= tst->free_list;
/*31:*/
#line 346 "./tst_insert.w"

tst->free_list= tst->free_list->middle;
#line 1 "./tst_search.w"
/*:31*/
#line 272 "./tst_insert.w"

new_node_tree_begin= current_node;
current_node= current_node->right;
current_node->value= key[key_index];
break;
}
else
{
current_node= current_node->right;
continue;
}

/*:28*/
#line 133 "./tst_insert.w"

}
}

/*:24*/
#line 52 "./tst_insert.w"

/*29:*/
#line 298 "./tst_insert.w"

do
{
key_index++;

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
{
current_node= new_node_tree_begin->middle;

while(current_node->middle!=NULL)
current_node= current_node->middle;

current_node->middle= tst->free_list;
tst->free_list= new_node_tree_begin->middle;
new_node_tree_begin->middle= NULL;

return TST_ERROR;
}
}

/*30:*/
#line 334 "./tst_insert.w"

if(tst->free_list==NULL)
{
if(tst_grow_node_free_list(tst)!=1)
return TST_ERROR;
}

/*:30*/
#line 320 "./tst_insert.w"

current_node->middle= tst->free_list;
/*31:*/
#line 346 "./tst_insert.w"

tst->free_list= tst->free_list->middle;
#line 1 "./tst_search.w"
/*:31*/
#line 322 "./tst_insert.w"

current_node= current_node->middle;
current_node->value= key[key_index];
}while(key[key_index]!=0);

current_node->middle= data;
return TST_OK;

/*:29*/
#line 53 "./tst_insert.w"

}

/*:21*/

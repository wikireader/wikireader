/*36:*/
#line 38 "./tst_delete.w"

#include "tst.h"
#include <stdio.h> 
#include <stdlib.h> 

void*tst_delete(const unsigned char*key,struct tst*tst)
{
struct node*current_node;
struct node*current_node_parent;
struct node*last_branch;
struct node*last_branch_parent;
struct node*next_node;
struct node*last_branch_replacement;
struct node*last_branch_dangling_child;
int key_index;

/*37:*/
#line 67 "./tst_delete.w"

if(key[0]==0)
return NULL;
if(tst->head[(int)key[0]]==NULL)
return NULL;

/*:37*/
#line 54 "./tst_delete.w"

/*38:*/
#line 75 "./tst_delete.w"

last_branch= NULL;
last_branch_parent= NULL;
current_node= tst->head[(int)key[0]];
current_node_parent= NULL;
key_index= 1;
while(current_node!=NULL)
{
if(key[key_index]==current_node->value)
{
/*39:*/
#line 123 "./tst_delete.w"

if((current_node->left!=NULL)||(current_node->right!=NULL))
{
last_branch= current_node;
last_branch_parent= current_node_parent;
}

/*:39*/
#line 85 "./tst_delete.w"

if(key[key_index]==0)
break;
else
{
current_node_parent= current_node;
current_node= current_node->middle;
key_index++;
continue;
}
}
else if(((current_node->value==0)&&(key[key_index]<64))||
((current_node->value!=0)&&(key[key_index]<
current_node->value)))
{
last_branch_parent= current_node;
current_node_parent= current_node;
current_node= current_node->left;
last_branch= current_node;
continue;
}
else
{
last_branch_parent= current_node;
current_node_parent= current_node;
current_node= current_node->right;
last_branch= current_node;
continue;
}

}

/*:38*/
#line 55 "./tst_delete.w"

if(current_node==NULL)
return NULL;
/*40:*/
#line 131 "./tst_delete.w"

if(last_branch==NULL)
{
/*41:*/
#line 152 "./tst_delete.w"

next_node= tst->head[(int)key[0]];
tst->head[(int)key[0]]= NULL;

/*:41*/
#line 134 "./tst_delete.w"

}
else if((last_branch->left==NULL)&&(last_branch->right==NULL))
{
/*42:*/
#line 160 "./tst_delete.w"

if(last_branch_parent->left==last_branch)
last_branch_parent->left= NULL;
else
last_branch_parent->right= NULL;

next_node= last_branch;

/*:42*/
#line 138 "./tst_delete.w"

}
else
{
/*43:*/
#line 176 "./tst_delete.w"

if((last_branch->left!=NULL)&&(last_branch->right!=NULL))
{
last_branch_replacement= last_branch->right;
last_branch_dangling_child= last_branch->left;
}
else if(last_branch->right!=NULL)
{
last_branch_replacement= last_branch->right;
last_branch_dangling_child= NULL;
}
else
{
last_branch_replacement= last_branch->left;
last_branch_dangling_child= NULL;
}

/*:43*/
#line 142 "./tst_delete.w"

/*44:*/
#line 201 "./tst_delete.w"

if(last_branch_parent==NULL)
tst->head[(int)key[0]]= last_branch_replacement;
else
{
if(last_branch_parent->left==last_branch)
last_branch_parent->left= last_branch_replacement;
else if(last_branch_parent->right==last_branch)
last_branch_parent->right= last_branch_replacement;
else
last_branch_parent->middle= last_branch_replacement;
}

/*:44*/
#line 143 "./tst_delete.w"

/*45:*/
#line 221 "./tst_delete.w"

if(last_branch_dangling_child!=NULL)
{
current_node= last_branch_replacement;

while(current_node->left!=NULL)
current_node= current_node->left;

current_node->left= last_branch_dangling_child;
}

next_node= last_branch;

/*:45*/
#line 144 "./tst_delete.w"

}
/*46:*/
#line 239 "./tst_delete.w"

do
{
current_node= next_node;
next_node= current_node->middle;
/*47:*/
#line 253 "./tst_delete.w"

current_node->left= NULL;
current_node->right= NULL;
current_node->middle= tst->free_list;
tst->free_list= current_node;
#line 1 "./tst_cleanup.w"
/*:47*/
#line 244 "./tst_delete.w"

}
while(current_node->value!=0);

return next_node;

/*:46*/
#line 146 "./tst_delete.w"


/*:40*/
#line 58 "./tst_delete.w"

}

/*:36*/

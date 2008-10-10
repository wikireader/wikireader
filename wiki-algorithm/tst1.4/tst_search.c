/*32:*/
#line 2 "./tst_search.w"

#include "tst.h"
#include <stdio.h> 
#include <stdlib.h> 

void*tst_search(const unsigned char*key,struct tst*tst,int option,
unsigned int*match_len)
{
struct node*current_node;
struct node*longest_match= NULL;
unsigned int longest_match_len= 0;
int key_index;

/*33:*/
#line 26 "./tst_search.w"

if(key[0]==0)
return NULL;

/*:33*/
#line 15 "./tst_search.w"

/*34:*/
#line 34 "./tst_search.w"

if(tst->head[(int)key[0]]==NULL)
return NULL;

/*:34*/
#line 16 "./tst_search.w"


if(match_len)
*match_len= 0;

/*35:*/
#line 43 "./tst_search.w"

current_node= tst->head[(int)key[0]];
key_index= 1;

while(current_node!=NULL)
{
if(key[key_index]==current_node->value)
{
if(current_node->value==0)
{
if(match_len)
*match_len= key_index;

return current_node->middle;
}
else
{
current_node= current_node->middle;
key_index++;
continue;
}
}
else
{
if(current_node->value==0)
{
if(option&TST_SUBSTRING_MATCH)
{
longest_match= current_node->middle;
longest_match_len= key_index;
}

if(key[key_index]<64)
{
current_node= current_node->left;
continue;
}
else
{
current_node= current_node->right;
continue;
}
}
else
{
if(key[key_index]<current_node->value)
{
current_node= current_node->left;
continue;
}
else
{
current_node= current_node->right;
continue;
}
}
}
}

if(match_len)
*match_len= longest_match_len;

return longest_match;
#line 1 "./tst_delete.w"
/*:35*/
#line 21 "./tst_search.w"

}

/*:32*/

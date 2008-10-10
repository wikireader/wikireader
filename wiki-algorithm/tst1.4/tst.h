/*49:*/
#line 177 "./tst.w"

/*1:*/
#line 28 "./tst.w"

struct node
{
unsigned char value;
struct node*left;
struct node*middle;
struct node*right;
};

/*:1*/
#line 178 "./tst.w"

/*2:*/
#line 61 "./tst.w"

struct tst
{
int node_line_width;
struct node_lines*node_lines;
struct node*free_list;
struct node*head[256];
};

/*:2*/
#line 179 "./tst.w"

/*3:*/
#line 81 "./tst.w"

struct node_lines
{
struct node*node_line;
struct node_lines*next;
};

/*:3*/
#line 180 "./tst.w"

/*4:*/
#line 93 "./tst.w"

enum tst_constants
{
TST_OK,TST_ERROR,TST_NULL_KEY,TST_DUPLICATE_KEY,TST_REPLACE,
TST_SUBSTRING_MATCH
};

/*:4*/
#line 181 "./tst.w"

/*6:*/
#line 110 "./tst.w"

struct tst*tst_init(int node_line_width);

/*:6*/
#line 182 "./tst.w"

/*7:*/
#line 132 "./tst.w"

int tst_insert(const unsigned char*key,void*data,struct tst*tst,
int option,void**exist_ptr);

/*:7*/
#line 183 "./tst.w"

/*8:*/
#line 145 "./tst.w"

void*tst_search(const unsigned char*key,struct tst*tst,
int option,unsigned int*match_len);

/*:8*/
#line 184 "./tst.w"

/*9:*/
#line 153 "./tst.w"

void*tst_delete(const unsigned char*key,struct tst*tst);

/*:9*/
#line 185 "./tst.w"

/*11:*/
#line 166 "./tst.w"

void tst_cleanup(struct tst*tst);

#line 1 "./tst_init.w"
/*:11*/
#line 186 "./tst.w"
/*:49*/

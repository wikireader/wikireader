
This is an implementation of the ternary search trie algorithm,
intended for use with null terminated strings. A package which handles
arbitrary sequences of unsigned bytes has been done, but I need to clean
up the code and write some documentation before I will feel comfortable
making it available. Note that keys are case sensitive, so you should
force your keys to lower case if you want to deal with things in a case
insensitive manner. All of this code is mine and not copied from
anywhere, so all bugs and sloppy code are my doing. Please contact me
if you have suggestions for changes or corrections.

{\obeylines
Peter A. Friend
pafriend@@octavian.org
http://www.octavian.org}

\input pstricks
\input pst-node
\input pst-tree

@*Structures.
This implementation uses a ternary search trie to store the characters
of a C string. This type of tree works much like a binary tree, yet has
three child nodes. The additional middle node is used when a character
of a key string matches the character at the current node in the tree.

\pstree[levelsep=1cm,radius=2pt]{\Tc{3pt}}{\TC*\TC*\TC*}
@<Node structure@>=
struct node
{
   unsigned char value;
   struct node *left;
   struct node *middle;
   struct node *right;
};

@
All function calls in the TST package refer to a |struct tst|. This
structure stores various values set during initialization as well as
the node storage area.

The |node_line_width| member refers to how many nodes are allocated at
once when no nodes are available from the free list.

The |node_lines| member is a pointer to a |struct node_lines|, which
will be explained below. This member exists so that all memory
allocated for node structures can be freed with a call to
|tst_cleanup()|.

The |free_list| member is a pointer to a |struct node|. This is
actually a linked list of nodes, linked together by the |middle|
pointers. When nodes are needed for the tree, they are removed from the
head of this list, and during deletion of keys, the nodes are
inserted.

The |head| member is an array of 256 pointers to |struct node|. All of
these pointers are NULL initially, and are filled in as keys are added.
Having a separate slot for each letter of the alphabet aids in
balancing the top of the tree.

@<TST structure@>=
struct tst
{
   int node_line_width;
   struct node_lines *node_lines;
   struct node *free_list;
   struct node *head[256];
};

@
Nodes are allocated in chunks of size |tst->node_line_width|. Each time
a chunk is allocated, another line of nodes is added to the
|node_lines| member of the |struct tst|.

The |node_line| member is allocated by a call to calloc(), so it is not
a linked list of nodes, but nodes in contiguous memory. During each
allocation, the nodes are added to the |free_list| member of |struct
tst|, and the pointers updated.

The |next| pointer is just a pointer to the next line of nodes.
@<Node lines structure@>=
struct node_lines
{
   struct node *node_line;
   struct node_lines *next;
};

@*Constants.
Some functions return pointers while others return integer values. All
functions that return a pointer return NULL on failure. All functions
that return an integer return one of the constants below. There are
also other constants for use in function calls.
@<TST constants@>=
enum tst_constants
{
   TST_OK, TST_ERROR, TST_NULL_KEY, TST_DUPLICATE_KEY, TST_REPLACE,
   TST_SUBSTRING_MATCH
};

@*Functions.
The definitions for all of the functions are below. Note that
|tst_grow_node_free_list()| is an internal function used only by
|tst_insert()|.

@
This function allocates a |struct tst| and returns the pointer. The
|node_line_width| argument controls how many nodes are allocated during
initialization and by each call to |tst_grow_node_free_list()|. This
function returns a valid pointer if it succeeds, NULL otherwise.
@<Declaration for |tst_init()|@>=
struct tst *tst_init(int node_line_width);

@
This function inserts |key| into the the tree and associates |key| with
a pointer to some |data|. The |data| argument must not be NULL, since
NULL is the value returned when a search or delete fails. If |option|
is set to |TST_REPLACE|, when an attempt is made to insert a key that
is already in the tree, the new |data| replaces the old. Otherwise,
|TST_DUPLICATE_KEY| is returned. If the key is successfully inserted,
|TST_OK| is returned. If |key| is zero length, |TST_NULL_KEY| is
returned. A return value of |TST_ERROR| indicates a memory allocation
failure occurred while tring to grow the node free list.

5/05/1999 - Change made to |tst_insert|
 
When an insert has failed we return |TST_DUPLICATE_KEY|, and if we
still want to do anything with the data for that key we have to make a
separate call to |tst_search| to get the pointer which is wasteful. A
new argument |exist_ptr| has been added to |tst_insert|. When
|TST_DUPLICATE_KEY| is returned, |exist_ptr| will contain the data
pointer for the existing key.
@<Declaration for |tst_insert()|@>=
int tst_insert(const unsigned char *key, void *data, struct tst *tst,
               int option, void **exist_ptr);

@
This function searches for |key| in the tree. If it succeeds, it
returns the |data| pointer associated with the key, NULL otherwise.
If a substring match is desired, then specify |TST_SUBSTRING_MATCH| as
the option, otherwise set the option to 0. If |match_len| is not
NULL, then the length of the match not counting the NULL terminator
will be stored there. For example, if the trie contains the strings
``test'' and ``testing'' a search for ``testi'' with |TST_SUBSTRING_MATCH|
will return ``test'' as a match.
@<Declaration for |tst_search()|@>=
void *tst_search(const unsigned char *key, struct tst *tst,
                 int option, unsigned int* match_len);

@
This function deletes |key| from the tree and returns the |data|
pointer associated with it. NULL is returned if the key is not in the
tree.
@<Declaration for |tst_delete()|@>=
void *tst_delete(const unsigned char *key, struct tst *tst);

@
This function is used to grow the free list in the |struct tst|. This
must not be called by the user. It is only called by |tst_insert()|
when inserting keys. It returns 1 on success, |TST_ERROR| otherwise.
@<Declaration for |tst_grow_node_free_list()|@>=
int tst_grow_node_free_list(struct tst *tst);

@
The function |tst_cleanup()| is used to free the lines of nodes
allocated, as well as the |struct tst| itself.
@<Declaration for |tst_cleanup()|@>=
void tst_cleanup(struct tst *tst);

@i tst_init.w 
@i tst_grow_node_free_list.w 
@i tst_insert.w 
@i tst_search.w 
@i tst_delete.w 
@i tst_cleanup.w

@*Header file.
@(tst.h@>=
@<Node structure@>
@<TST structure@>
@<Node lines structure@>
@<TST constants@>
@<Declaration for |tst_init()|@>
@<Declaration for |tst_insert()|@>
@<Declaration for |tst_search()|@>
@<Declaration for |tst_delete()|@>
@<Declaration for |tst_cleanup()|@>

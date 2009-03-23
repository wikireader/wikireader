/*
 * 	Simple doubly linked list.
 *
 *  This is a generic implementation.
 *	No dynamic memeory allocation required.
 *
 */

#ifndef LIST_H_
#define LIST_H_

/*
 * Use this struct in the beginning of your structure declaration.
 */
struct wl_list {
	struct wl_list *prev, *next;
};

static inline void wl_list_init(struct wl_list *list)
{
	list->prev = list;
	list->next = list;
}

static inline void
wl_list_insert_after(struct wl_list *list, struct wl_list *node)
{
	node->prev = list;
	node->next = list->next;
	list->next->prev = node;
	list->next = node;
}


static inline void
wl_list_link_node(struct wl_list *prev, struct wl_list *next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void wl_list_del(struct wl_list *node)
{
	wl_list_link_node(node->prev, node->next);
	wl_list_init(node);
}

static inline int wl_list_size(struct wl_list *list)
{
	struct wl_list *node = list->next;
	int count = 0;

	while (node != list) {
		++count;
		node = node->next;
	}

	return count;
}

static struct wl_list *
wl_list_search(struct wl_list *head, void *value, unsigned int offset,
int (*comp)(void *value, unsigned int offset, const struct wl_list *node))
{
	struct wl_list *p = head->next;

	while (p != head) {
		if (!(comp)(value, offset, p))
			break;
		p = p->next;
	}
	return p == head ? NULL : p;
}

#endif /* LIST_H_*/

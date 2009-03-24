/*
    Wikipedia reader tools
    ncurses based simulator

    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>
    Copyright (c) 2009 Matt Hsu <matt_hsu@openmoko.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include <wikilib.h>
#include <guilib.h>
#include <msg.h>
#include <input.h>
#include <list.h>

/* empty dummies - no framebuffer here */
void fb_set_pixel(int x, int y, int val) {}
void fb_refresh(void) {}
void fb_clear(void) {}

static unsigned char framebuffer_data[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];
unsigned char *framebuffer = &framebuffer_data[0];


static int tests = 0;
static int passed = 0;
static int failed = 0;
#define COMPARE_INT(actual, expected, statement, failure_text) \
	++tests; \
	if (actual statement expected) { \
		printf("SUCCESS: %s\n", failure_text); \
		++passed; \
	} else { \
		printf("FAIL: Got: %d Expected: %d Msg: %s\n", \
				actual, expected, failure_text); \
		++failed; \
	}


int wl_input_wait(struct wl_input_event *ev, int sleep)
{
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = getc(stdin);
	ev->key_event.value = 1;
	return 0;
}


/* Simple tests for the linked list */
struct list_template {
	struct wl_list list;
	int index;
	int index_2;
	char member[8];
};

static int comp_node(void *value, unsigned int offset, struct wl_list *node)
{
	int *p = (void *)(node)+sizeof(struct wl_list)+offset;
	int val = (int)(value);

	return val == *p? 0: 1;
}

static void list_test(void) 
{
	int i;
	struct list_template head;
	struct list_template *tmp;
	struct list_template alist[10];

	/* init head of this lists */
	head.index = -1;
	wl_list_init(&head.list);

	for (i = 0; i < 10; i++){
		alist[i].index = i;
		alist[i].index_2 = i+10;
		wl_list_insert_after(&head.list, &alist[i].list);
	}

	COMPARE_INT(wl_list_size(&head.list), 10, ==, "All added");

	wl_list_del(&alist[3].list);
	COMPARE_INT(wl_list_size(&head.list), 9, ==, "Removed 3rd item");


	tmp = wl_list_search(&head.list, (void*)(int)3, 0, &comp_node);
	COMPARE_INT(tmp, &alist[3].list, !=, "Not found");

	tmp = wl_list_search(&head.list, (void*)(int)2, 0, &comp_node);
	COMPARE_INT(tmp, &alist[2].list, ==, "Found");
}

int main(int argc, char *argv[])
{
	wikilib_init();
	guilib_init();

	/* add tests here */
	list_test();

	printf("Test result: Executed tests: %d Passed: %d Failed: %d\n",
			tests, passed, failed);
	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}


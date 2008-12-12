#include <stdio.h>
#include <ncurses.h>

#include <wikilib.h>
#include <guilib.h>
#include <msg.h>
#include <input.h>

static int loglevel = MSG_LEVEL_MAX;

/* empty dummies - no framebuffer here */
void fb_set_pixel(int x, int y, int val) {}
void fb_refresh(void) {}
void fb_clear(void) {}

int wl_input_wait(struct wl_input_event *ev)
{
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = getch();
	ev->key_event.value = 1;
	return 0;
}

void exit_handler(void)
{
	echo();
	nocbreak();
	clrtoeol();
	refresh();
	endwin();
}


void set_loglevel(int level)
{
	loglevel = level;
}

void msg(int level, const char *format, ...)
{
	va_list ap;
	int attrs;

	if (level > loglevel)
		return;

	switch (level) {
	case MSG_DEBUG:
		attrs = COLOR_PAIR(3);
		break;
	case MSG_ERROR:
		attrs = COLOR_PAIR(2);
		break;
	default:
		attrs = COLOR_PAIR(1);
		break;
	}

        va_start(ap, format);
	attron(attrs);
	vwprintw(stdscr, format, ap);
	printw("\n");
	attroff(attrs);
	va_end(ap);
}

int main(int argc, char *argv[])
{
	atexit(exit_handler);
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	cbreak();

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);

	wikilib_init();
	guilib_init();
	wikilib_run();

	/* never reached */
	return 0;
}


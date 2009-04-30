#ifndef WL_MSG_H
#define WL_MSG_H

enum {
	MSG_ERROR = 0,
	MSG_WARNING,
	MSG_INFO,
	MSG_DEBUG,
	MSG_LEVEL_MAX
};

void msg(int level, const char *format, ...);
void set_loglevel(int level);
void hexdump(const char *p, unsigned int len);

#endif /* WL_MSG_H */

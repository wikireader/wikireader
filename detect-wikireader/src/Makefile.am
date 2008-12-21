AM_CFLAGS = -Wall

detect-wikireader-version.h:
	echo -e '#ifndef DETECT_WIKIREADER_VERSION' \
	'\n#define DETECT_WIKIREADER_VERSION "'`svnversion`'"' \
	'\n#endif' > detect-wikireader-version.h
BUILT_SOURCES = detect-wikireader-version.h

bin_PROGRAMS = det-wikird det-wikird_static
det_wikird_SOURCES = main.c check-wikireader.c wiki-inotify.c daemon.c

det_wikird_static_SOURCES = main.c check-wikireader.c wiki-inotify.c daemon.c
det_wikird_static_LDFLAGS = -static

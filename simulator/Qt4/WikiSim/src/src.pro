SOURCES += ../../../../gui-lib/guilib.c
SOURCES += ../../../../gui-lib/glyph.c
SOURCES += ../../../../wiki-lib/specific/file-io-posix.c
SOURCES += ../../../../wiki-lib/specific/msg-printf.c
SOURCES += ../../../../wiki-lib/specific/time-posix.c
SOURCES += ../../../../wiki-lib/generic/wikilib.c
SOURCES += ../../../../wiki-lib/generic/file-io.c
SOURCES += ../../../../wiki-lib/generic/search-test.c
INCLUDEPATH += ../../../../gui-lib/
INCLUDEPATH += ../../../../wiki-lib/include/

SOURCES += main.cpp \
 WikiDisplay.cpp \
 MainWindow.cpp \
 WikilibThread.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/wikisim

HEADERS += WikiDisplay.h \
 MainWindow.h \
 WikilibThread.h



TOPLEVEL = ../../../../../
SOURCES += $$TOPLEVEL/gui-lib/guilib.c
SOURCES += $$TOPLEVEL/gui-lib/glyph.c
SOURCES += $$TOPLEVEL/gui-lib/fontfile.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/file-io-posix.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/msg-printf.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/time-posix.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/article.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/wikilib.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/search.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/hexdump.c
SOURCES += $$TOPLEVEL/host-tools/indexing/locate/fast_search.c
SOURCES += $$TOPLEVEL/host-tools/indexing/locate/lsearcher.c
INCLUDEPATH += $$TOPLEVEL/gui-lib/
INCLUDEPATH += $$TOPLEVEL/wiki-lib/include/
INCLUDEPATH += $$TOPLEVEL/host-tools/indexing/locate/
INCLUDEPATH += $$TOPLEVEL/common/

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

TOPLEVEL = ../../../
SOURCES += $$TOPLEVEL/gui-lib/guilib.c
SOURCES += $$TOPLEVEL/gui-lib/glyph.c
SOURCES += $$TOPLEVEL/gui-lib/fontfile.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/file-io-posix.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/msg-printf.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/time-posix.c
SOURCES += $$TOPLEVEL/wiki-lib/specific/malloc-simple.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/decompress.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/history.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/file-io-general.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/wikilib.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/wom_reader.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/search.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/hexdump.c
SOURCES += $$TOPLEVEL/wiki-lib/generic/keyboard.c
SOURCES += $$TOPLEVEL/host-tools/indexing/locate/fast_search.c
SOURCES += $$TOPLEVEL/host-tools/indexing/locate/lsearcher.c
SOURCES += $$TOPLEVEL/samo-lib/minilzo/minilzo.c
SOURCES += $$TOPLEVEL/samo-lib/fatfs_stdio.c
INCLUDEPATH += $$TOPLEVEL/gui-lib/
INCLUDEPATH += $$TOPLEVEL/wiki-lib/include/
INCLUDEPATH += $$TOPLEVEL/host-tools/indexing/locate/
INCLUDEPATH += $$TOPLEVEL/samo-lib/
INCLUDEPATH += $$TOPLEVEL/samo-lib/minilzo/
INCLUDEPATH += $$TOPLEVEL/samo-lib/fatfs/src
INCLUDEPATH += $$TOPLEVEL/samo-lib/fatfs/config/c33/read-write
DEPENDPATH = $$INCLUDEPATH

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

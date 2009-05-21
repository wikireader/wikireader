TOPLEVEL = ../../../
SOURCES += $$TOPLEVEL/wiki-app/guilib.c
SOURCES += $$TOPLEVEL/wiki-app/glyph.c
SOURCES += $$TOPLEVEL/wiki-app/fontfile.c
SOURCES += $$TOPLEVEL/wiki-app/file-io-posix.c
SOURCES += $$TOPLEVEL/wiki-app/msg-printf.c
SOURCES += $$TOPLEVEL/wiki-app/time-posix.c
SOURCES += $$TOPLEVEL/wiki-app/malloc-simple.c
SOURCES += $$TOPLEVEL/wiki-app/decompress.c
SOURCES += $$TOPLEVEL/wiki-app/history.c
SOURCES += $$TOPLEVEL/wiki-app/file-io-general.c
SOURCES += $$TOPLEVEL/wiki-app/wikilib.c
SOURCES += $$TOPLEVEL/wiki-app/wom_reader.c
SOURCES += $$TOPLEVEL/wiki-app/search.c
SOURCES += $$TOPLEVEL/wiki-app/random.c
SOURCES += $$TOPLEVEL/wiki-app/hexdump.c
SOURCES += $$TOPLEVEL/wiki-app/keyboard.c
SOURCES += $$TOPLEVEL/host-tools/indexing/locate/fast_search.c
SOURCES += $$TOPLEVEL/host-tools/indexing/locate/lsearcher.c
SOURCES += $$TOPLEVEL/samo-lib/minilzo/minilzo.c
SOURCES += $$TOPLEVEL/samo-lib/src/fatfs_stdio.c
INCLUDEPATH += $$TOPLEVEL/wiki-app/
INCLUDEPATH += $$TOPLEVEL/host-tools/indexing/locate/
INCLUDEPATH += $$TOPLEVEL/samo-lib/include/
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

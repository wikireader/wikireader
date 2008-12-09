SOURCES += ../../../../gui-lib/guilib.c
SOURCES += ../../../../gui-lib/glyph.c
SOURCES += ../../../../wiki-lib/lib/file-io-posix.c
INCLUDEPATH += ../../../../gui-lib/
INCLUDEPATH += ../../../../wiki-lib/include

SOURCES += main.cpp \
 WikiDisplay.cpp \
 MainWindow.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/wikisim

HEADERS += WikiDisplay.h \
 MainWindow.h


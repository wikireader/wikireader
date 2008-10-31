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


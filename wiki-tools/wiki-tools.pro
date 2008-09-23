TEMPLATE = app
TARGET   = wiki-tools

INCLUDEPATH += src/

# Core functionality
HEADERS += \
    src/ArticleHandler.h \
    src/Article.h \
    src/StreamReader.h \
    src/Title.h

SOURCES += \
    src/ArticleHandler.cc \
    src/Article.cc \
    src/StreamReader.cc \
    src/Title.cc
    

# Extractors/Functionality
HEADERS += \
    src/CreateIndex.h \
    src/CreateText.h \
    src/ExtractWords.h \
    src/ExtractTitles.h
SOURCES += \
    src/CreateIndex.cc \
    src/CreateText.cc \
    src/ExtractWords.cc \
    src/ExtractTitles.cc \
    src/main.cc

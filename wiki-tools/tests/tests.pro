TEMPLATE = app
TARGET = tst_wiki-tools
SOURCES += tst_wikitools.cc

INCLUDEPATH += ../src ../mozilla-sha1
DEPENDPATH += ../src
SOURCES += \
    ../src/Article.cc \
    ../src/ArticleHandler.cc \
    ../src/CreateIndex.cc \
    ../src/CreateText.cc \
    ../src/ExtractText.cc \
    ../src/ExtractTextHashed.cc \
    ../src/ExtractTitles.cc \
    ../src/ExtractWords.cc \
    ../src/SplitArticles.cc \
    ../src/StreamReader.cc \
    ../src/Title.cc \
    ../mozilla-sha1/sha1.c

QT += testlib webkit

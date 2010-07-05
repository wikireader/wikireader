# src.pro

TEMPLATE = app

TOPLEVEL = ../../..

SOURCES += $${TOPLEVEL}/wiki-app/bigram.c
SOURCES += $${TOPLEVEL}/wiki-app/bmf.c
SOURCES += $${TOPLEVEL}/wiki-app/file-io-posix.c
SOURCES += $${TOPLEVEL}/wiki-app/glyph.c
SOURCES += $${TOPLEVEL}/wiki-app/guilib.c
SOURCES += $${TOPLEVEL}/wiki-app/history.c
SOURCES += $${TOPLEVEL}/wiki-app/keyboard.c
SOURCES += $${TOPLEVEL}/wiki-app/lcd_buf_draw.c
SOURCES += $${TOPLEVEL}/wiki-app/malloc-simple.c
SOURCES += $${TOPLEVEL}/wiki-app/msg-printf.c
SOURCES += $${TOPLEVEL}/wiki-app/restricted.c
SOURCES += $${TOPLEVEL}/wiki-app/search.c
SOURCES += $${TOPLEVEL}/wiki-app/search_hash.c
SOURCES += $${TOPLEVEL}/wiki-app/sha1.c
SOURCES += $${TOPLEVEL}/wiki-app/time-posix.c
SOURCES += $${TOPLEVEL}/wiki-app/wikilib.c
SOURCES += $${TOPLEVEL}/wiki-app/wiki_info.c
SOURCES += $${TOPLEVEL}/wiki-app/utf8.c

SOURCES += $${TOPLEVEL}/samo-lib/lzma/Alloc.c
SOURCES += $${TOPLEVEL}/samo-lib/lzma/Bra.c
SOURCES += $${TOPLEVEL}/samo-lib/lzma/LzFind.c
SOURCES += $${TOPLEVEL}/samo-lib/lzma/LzmaDec.c

INCLUDEPATH += $${TOPLEVEL}/wiki-app/

INCLUDEPATH += $${TOPLEVEL}/samo-lib/include/
INCLUDEPATH += $${TOPLEVEL}/samo-lib/drivers/include/
INCLUDEPATH += $${TOPLEVEL}/samo-lib/minilzo/
INCLUDEPATH += $${TOPLEVEL}/samo-lib/lzma/
INCLUDEPATH += $${TOPLEVEL}/samo-lib/fatfs/src
INCLUDEPATH += $${TOPLEVEL}/samo-lib/fatfs/config/c33/read-write

DEPENDPATH = $$INCLUDEPATH

SOURCES += main.cpp
SOURCES += WikiDisplay.cpp
SOURCES += MainWindow.cpp
SOURCES += WikilibThread.cpp

CONFIG += warn_on
CONFIG += thread
CONFIG += qt
CONFIG += debug

TARGET = ../bin/wikisim

HEADERS += WikiDisplay.h
HEADERS += MainWindow.h
HEADERS += WikilibThread.h

images = keyboard_123 keyboard_abc keyboard_abc2 keyboard_phone_jp keyboard_phone_tw keyboard_phone_abc keyboard_phone_123 keyboard_phone_tw_abc keyboard_phone_tw_123 nls

all_images.target = all_images.h

QMAKE_CLEAN += $${all_images.target}

all_images.commands = '@${DEL_FILE} "$@";'
for(i, images) {
    h = $${i}.h
    png = $${TOPLEVEL}/wiki-app/$${i}.png
    all_images.depends += $${h}

    QMAKE_CLEAN += $${h}

    all_images.commands += 'echo Keyboard: $${h};'
    all_images.commands += 'echo \'$${LITERAL_HASH}include "$${h}"\' >> "$@";'

    eval($${i}.target = $${h})
    eval($${i}.depends = $${png})
    eval($${i}.commands = '$${TOPLEVEL}/host-tools/imagetool/image2header --inverted --header=$${h} --variable=$${i}_image $${png}')

    QMAKE_EXTRA_TARGETS += $${i}
}

QMAKE_EXTRA_TARGETS += all_images

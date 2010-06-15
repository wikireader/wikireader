#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Article Rendering
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys, os, struct, os.path, re
import io
import time
import HTMLParser
import unicodedata
import htmlentitydefs
import codecs
import getopt
import os.path
import sqlite3
import WordWrap
import bucket
import PrintLog
import LanguageTranslation
import EscapeBuffer

try:
    import gd
except:
    print 'error: Missing python module: python-gd'
    print '       sudo apt-get install python-gd'
    exit(1)

# try to find a lzma library interface

no_compression = True

# python-lzma
if no_compression:
    try:
        import lzma

        def CompressData(data):
            c = lzma.compress(data, options={'format': 'alone'})
            # header: options(1) dictionary-size(4) uncompressed-length(8) = 13 bytes
            return c[:5] + c[13:] # drop the uncompressed length (always 0xffff_ffff_ffff_ffff)

        no_compression = False

    except:
        pass


# PyLZMA
if no_compression:
    try:
        import pylzma

        def CompressData(data):
            return pylzma.compress(data,
                                   dictionary = 24, fastBytes = 32,
                                   literalContextBits = 3,
                                   literalPosBits = 0, posBits = 2,
                                   algorithm = 1, eos = 1)

        no_compression = False

    except:
        pass




# none detected
if no_compression:
    print 'error: Missing python LZMA compression module'
    print 'alternative 1: (preferred)'
    print '       sudo apt-get install python-lzma'
    print 'alternative 2:'
    print '       sudo apt-get install python-pylzma'
    print 'alternative 3: compile/install local PyLZMA'
    print '       make local-pylzma-install'
    exit(1)



verbose = False
warnings = False
article_count = 0

# NASTY HACK: allow this </div class="something">
HTMLParser.endtagfind = re.compile('</\s*([a-zA-Z][-.a-zA-Z0-9:_]*)\s*[^>]*>')


# from: wiki-app/bmf.h
FONT_BMF_HEADER = '<4bI'    # struct font_bmf_header (header)
CHARMETRIC_BMF  = '<8b48s'  # struct charmetric_bmf (font)

FONT_BMF_HEADER_SIZE = struct.calcsize(FONT_BMF_HEADER)
CHARMETRIC_BMF_SIZE  = struct.calcsize(CHARMETRIC_BMF)

# font face defines - match the #defines of the same name in: wiki-app/lcd_buf_draw.h
ITALIC_FONT_IDX         = 1
DEFAULT_FONT_IDX        = 2
TITLE_FONT_IDX          = 3
SUBTITLE_FONT_IDX       = 4
DEFAULT_ALL_FONT_IDX    = 5
TITLE_ALL_FONT_IDX      = 6
SUBTITLE_ALL_FONT_IDX   = 7

FONT_FACE_NAME = {
    ITALIC_FONT_IDX: 'Italic',
    DEFAULT_FONT_IDX: 'Default',
    TITLE_FONT_IDX: 'Title',
    SUBTITLE_FONT_IDX: 'Subtitle',
    DEFAULT_ALL_FONT_IDX: 'Default All',
    TITLE_ALL_FONT_IDX: 'Title All',
    SUBTITLE_ALL_FONT_IDX: 'Subtitle All',
}

# Screen dimensions
LCD_WIDTH               = 240
LCD_LEFT_MARGIN         = 6     # def. in lcd_buf_draw.h
LCD_IMG_MARGIN          = 8

# Line Spaces (read directly from the font using gdbfed)
H1_LSPACE               = 21     # 17 4
H2_LSPACE               = 18     # 14 4
H3_LSPACE               = H2_LSPACE
H4_LSPACE               = H2_LSPACE
H5_LSPACE               = H2_LSPACE
H6_LSPACE               = H2_LSPACE
P_LSPACE                = 16     # 13 3

# Margins & Spacing
LIST_INDENT             = 12
DIV_MARGIN_TOP          = 10
P_MARGIN_TOP            = DIV_MARGIN_TOP
BLOCKQUOTE_MARGIN_TOP   = DIV_MARGIN_TOP
BLOCKQUOTE_MARGIN_LEFT  = LIST_INDENT
BLOCKQUOTE_MARGIN_RIGHT = LIST_INDENT
LIST_MARGIN_TOP         = DIV_MARGIN_TOP
BR_MARGIN_TOP           = DIV_MARGIN_TOP
DEFAULT_LWIDTH          = (LCD_WIDTH - LCD_LEFT_MARGIN + 1)
H1_MARGIN_TOP           = 8
H1_MARGIN_BOTTOM        = P_MARGIN_TOP
H2_MARGIN_TOP           = 14
H3_MARGIN_TOP           = H2_MARGIN_TOP
H4_MARGIN_TOP           = H2_MARGIN_TOP
H5_MARGIN_TOP           = H2_MARGIN_TOP
H6_MARGIN_TOP           = H2_MARGIN_TOP


LIMAX_INDENT_LEVELS     = 3
MAX_QUOTE_LEVEL         = 1

# bullet[0] charater is not used (the '!')
bullet_c                = u"!\u25aa\u2022\u25ab"
LIMAX_BULLETS           = len(bullet_c) - 1

font_id_values = {}


g_starty = 0
g_curr_face = DEFAULT_FONT_IDX
g_halign = 0
g_this_article_title = 'NO TITLE'
g_links = {}
g_link_cnt = 0
i_out = None
f_out = None
file_number = 0

article_db = None

output = None
compress = True

article_writer = None


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options> html-files...'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    print('       --warnings              Enable warnings output')
    print('       --number=n              Number for the .dat/.idx-tmp files [0]')
    print('       --test=file             Output the uncompressed file for testing')
    print('       --font-path=dir         Path to font files (*.bmf) [fonts]')
    print('       --article-index=file    Article index dictionary input [articles.db]')
    print('       --data-prefix=name      Directory and file name portion for .dat files [pedia]')
    print('       --index-prefix=name     Directory and file name portion for .idx-tmp files [pedia]')
    print('       --languages-links=<YN>  Turn on/off inter-wiki links [YES]')
    print('       --images=<YN>           Turn on/off in-line math images [YES]')
    print('       --articles=<N>          Articles per block [32]')
    print('       --block-size=<bytes>    Max size for artical block [262144]')
    exit(1)


def main():
    global verbose, warnings, compress
    global f_out, output, i_out
    global font_id_values
    global file_number
    global article_count
    global article_db
    global start_time
    global article_writer

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   'hvwn:d:p:i:t:f:l:a:b:',
                                   ['help',
                                    'verbose',
                                    'warnings',
                                    'number=',
                                    'data-prefix=',
                                    'index-prefix=',
                                    'article-index=',
                                    'test=',
                                    'font-path=',
                                    'language-links=',
                                    'images=',
                                    'articles=',
                                    'block-size=',
                                    ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    warnings = False
    data_file = 'pedia{0:d}.dat'
    index_file = 'pedia{0:d}.idx-tmp'
    art_file = 'articles.db'
    file_number = 0
    test_file = ''
    font_path = "../fonts"
    article_db = None
    inter_links = True
    enable_images = True
    articles_per_block = 32
    block_size = 262144


    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-w', '--warnings'):
            warnings = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-t', '--test'):
            test_file = arg
        elif opt in ('-i', '--article-index'):
            art_file = arg
        elif opt in ('-n', '--number'):
            try:
                file_number = int(arg)
            except ValueError:
                usage('"{0:s}={1:s}" is not numeric'.format(opt, arg))
        elif opt in ('-d', '--data-prefix'):
            data_file = arg + '{0:d}.dat'
        elif opt in ('-p', '--index-prefix'):
            index_file = arg + '{0:d}.idx-tmp'
        elif opt in ('-f', '--font-path'):
            font_path = arg
        elif opt in ('-l', '--language-links'):
            arg = arg.lower()
            inter_links = ('yes' == arg)
        elif opt in ('-l', '--images'):
            arg = arg.lower()
            enable_images = ('yes' == arg)
        elif opt in ('-a', '--articles'):
            try:
                articles_per_block = int(arg)
            except ValueError:
                usage('"{0:s}={1:s}" is not numeric'.format(opt, arg))
            if articles_per_block < 1 or articles_per_block > 64:
                usage('"{0:s}={1:s}" is out of range [1..64]'.format(opt, arg))
        elif opt in ('-b', '--block-size'):
            try:
                block_size = int(arg)
            except ValueError:
                usage('"{0:s}={1:s}" is not numeric'.format(opt, arg))
            if block_size < 65536 or block_size > 524288:
                usage('"{0:s}={1:s}" is out of range [65536..524288]'.format(opt, arg))
        else:
            usage('unhandled option: ' + opt)

    start_time = time.time()

    f_fontr  = open(os.path.join(font_path, "text.bmf"), "rb")
    f_fonti  = open(os.path.join(font_path, "texti.bmf"), "rb")
    f_fontt  = open(os.path.join(font_path, "title.bmf"), "rb")
    f_fontst = open(os.path.join(font_path, "subtitle.bmf"), "rb")
    f_font_all = open(os.path.join(font_path, "textall.bmf"), "rb")
    f_fontt_all = open(os.path.join(font_path, "titleall.bmf"), "rb")
    f_fontst_all = open(os.path.join(font_path, "subtlall.bmf"), "rb")

    font_id_values = {
        ITALIC_FONT_IDX: f_fonti,
        DEFAULT_FONT_IDX: f_fontr,
        TITLE_FONT_IDX: f_fontt,
        TITLE_ALL_FONT_IDX: f_fontt_all,
        SUBTITLE_FONT_IDX: f_fontst,
        SUBTITLE_ALL_FONT_IDX: f_fontst_all,
        DEFAULT_ALL_FONT_IDX: f_font_all
    }

    article_db = sqlite3.connect(art_file)

    article_db.execute('pragma auto_vacuum = none')
    article_db.execute('pragma synchronous = off')
    article_db.execute('pragma temp_store = memory')
    article_db.execute('pragma locking_mode = normal')
    article_db.execute('pragma read_uncommitted = true')
    article_db.execute('pragma cache_size = 20000000')
    article_db.execute('pragma default_cache_size = 20000000')
    article_db.execute('pragma journal_mode = off')

    def y_adjust(inc):
        global g_starty
        g_starty += inc

    output = EscapeBuffer.EscapeBuffer(callback=y_adjust)

    if test_file == '':
        compress = True
        i_out = open(index_file.format(file_number), 'wb')
        f_out = open(data_file.format(file_number), 'wb')
        article_writer = ArticleWriter(file_number, f_out, i_out,
                                       max_buckets = 50,
                                       bucket_size = block_size,
                                       max_items_per_bucket = articles_per_block)
    else:
        compress = False
        f_out = open(test_file, 'wb')

    for name in args:
        f = codecs.open(name, 'r', 'utf-8', 'replace')
        WrProcess(f, inter_links, enable_images)
        f.close()

    for item in font_id_values:
        font_id_values[item].close()

    if output != None:
        del output

    if article_writer != None:
        del article_writer

    if f_out != None:
        f_out.close()
    if i_out != None:
        i_out.close()

    if article_db != None:
        article_db.close()

    for i in font_id_values:
        font_id_values[i].close()

    # final message
    PrintLog.message("Render[{0:d}]: Total: {1:d}".format(file_number, article_count))


#
# cached font information
#
font_width_cache = {}
font_default_cache = {}

def get_utf8_cwidth(c, face):
    global font_width_cache
    global font_default_cache
    global font_id_values
    global FONT_BMF_HEADER
    global CHARMETRIC_BMF
    global FONT_BMF_HEADER_SIZE
    global CHARMETRIC_BMF_SIZE
    global FONT_FACE_NAME

    if type(c) != unicode:
        c = unicode(c, 'utf-8')

    if (c, face) in font_width_cache:
        return font_width_cache[(c, face)]

    font_file = font_id_values[face]

    if face not in font_default_cache:
        font_file.seek(0)
        buffer = font_file.read(FONT_BMF_HEADER_SIZE)

        if len(buffer) != 0:
            linespace, ascent, descent, bmp_buffer_len, default_char = struct.unpack(FONT_BMF_HEADER, buffer)
        else:
            linespace, ascent, descent, bmp_buffer_len, default_char = (0, 0, 0, 0, ord(u' '))

        font_default_cache[face] = unichr(default_char)

    font_file.seek(ord(c) * CHARMETRIC_BMF_SIZE + FONT_BMF_HEADER_SIZE)
    buffer = font_file.read(CHARMETRIC_BMF_SIZE)

    if len(buffer) != 0:
        width, height, widthBytes, widthBits, ascent, descent, LSBearing, widthDevice, bitmap = struct.unpack(CHARMETRIC_BMF, buffer)
    else:
        width, height, widthBytes, widthBits, ascent, descent, LSBearing, widthDevice, bitmap = (0,0,0,0,0,0,0,0,
                                                                                               r'\x55' * 48)
    character_width = widthDevice

    if 0 == character_width:

        if TITLE_FONT_IDX == face:
            character_width = get_utf8_cwidth(c, TITLE_ALL_FONT_IDX)

        elif SUBTITLE_FONT_IDX == face:
            character_width = get_utf8_cwidth(c, SUBTITLE_ALL_FONT_IDX)

        elif face in [TITLE_ALL_FONT_IDX, SUBTITLE_ALL_FONT_IDX, DEFAULT_ALL_FONT_IDX]:
            character_width = get_utf8_cwidth(font_default_cache[face], face)

        else:
            character_width = get_utf8_cwidth(c, DEFAULT_ALL_FONT_IDX)

    font_width_cache[(c, face)] = character_width

    return character_width



get_lineheight_values = {
    ITALIC_FONT_IDX:       P_LSPACE,
    DEFAULT_FONT_IDX:      P_LSPACE,
    TITLE_FONT_IDX:        H1_LSPACE,
    TITLE_ALL_FONT_IDX:    H1_LSPACE,
    SUBTITLE_FONT_IDX:     H2_LSPACE,
    SUBTITLE_ALL_FONT_IDX: H2_LSPACE,
    DEFAULT_ALL_FONT_IDX:  P_LSPACE
    }

def get_lineheight(face):
    global get_lineheight_values
    return get_lineheight_values[face]


def make_link(url, x0, x1, text):
    global g_starty, g_curr_face, g_link_cnt, g_links

    if article_index(url):
        esc_code10(x1 - x0)
        g_links[g_link_cnt] = (x0, g_starty + 2, x1, g_starty + get_lineheight(g_curr_face) + 2, url)
        g_link_cnt =  g_link_cnt + 1


def get_imgdata(imgfile, indent):
    try:
        img = gd.image(imgfile)
    except IOError, e:
        PrintLog.message(u'unable to open image file: {0:s}'.format(imgfile))
        return (0, 0, r'')

    (width, height) = img.size()
    if width <= (LCD_WIDTH - LCD_IMG_MARGIN - indent):
        is_black = lambda x, y: (0, 0, 0) == img.colorComponents(img.getPixel((x, y)))
        h_range = range(0, width)
        v_range = range(0, height)
    elif height <= (LCD_WIDTH - LCD_IMG_MARGIN - indent):
        is_black = lambda x, y: (0, 0, 0) == img.colorComponents(img.getPixel((y, x)))
        v_range = range(0, width)
        h_range = range(height - 1, -1, -1)
        (width, height) = (height, width)
    else:
        PrintLog.message(u'image file: {0:s} is too big'.format(imgfile))
        return (0, 0, r'')

    data = ''
    for v in v_range:
        byte = 0
        bit_count = 8

        for h in h_range:
            if is_black(h, v):
                pixel = 1
            else:
                pixel = 0
            bit_count -= 1
            byte |= pixel << bit_count
            if 0 == bit_count:
                data += struct.pack('<B', byte)
                byte = 0
                bit_count = 8
        if 8 != bit_count:
            data += struct.pack('<B', byte)

    return (width, height, data)

def esc_code_separate():
    """used to prevent merging to consecutive escape code zeros"""
    global output
    output.write('')

def esc_code0(num_pixels):
    """blank line height in pixels"""
    global g_starty
    global output

    output.write(struct.pack('<BB', 1, num_pixels))
    g_starty += num_pixels


def esc_code1():
    """new line with default font and default line space"""
    global g_starty, g_curr_face
    global output

    output.write(struct.pack('<B', 2))
    g_starty += get_lineheight(DEFAULT_FONT_IDX)
    g_curr_face = DEFAULT_FONT_IDX


def esc_code2():
    """new line with current font and current line space"""
    global g_starty, g_curr_face
    global output

    output.write(struct.pack('<B', 3))
    g_starty += get_lineheight(g_curr_face)


def esc_code3(face):
    """new line using new font face."""
    global g_starty, g_curr_face
    global output

    num_pixels = get_lineheight(face)
    output.write(struct.pack('<BB', 4, face|(num_pixels<<3)))
    g_starty += num_pixels
    g_curr_face = face


def esc_code4(face, halign=0):
    """change font with current horizontal alignment (in pixels)"""
    global g_curr_face
    global output

    output.write(struct.pack('<BB', 5, face|(halign<<3)))
    g_curr_face = face


def esc_code5():
    """set font as default"""
    global g_curr_face
    global output

    output.write(struct.pack('<B', 6))
    g_curr_face = DEFAULT_FONT_IDX


def esc_code6():
    """set default alignment"""
    global output

    output.write(struct.pack('<B', 7))


def esc_code7(num_pixels):
    """move right num_pixels"""
    global output

    output.write(struct.pack('<BB', 8, num_pixels))


def esc_code8(num_pixels):
    """move left num_pixels"""
    global output

    output.write(struct.pack('<BB', 9, num_pixels))


def esc_code9(num_pixels):
    """alignment adjustment"""
    global g_halign
    global output

    output.write(struct.pack('<Bb', 10, num_pixels))
    g_halign = num_pixels


def esc_code10(num_pixels):
    """draw line from right to left"""
    global output

    output.write(struct.pack('<BB', 11, num_pixels))


def esc_code14(width, height, data):
    """output bitmap"""
    global g_starty, g_curr_face
    global output

    if 0 == width or 0 == height:
        return

    output.write(struct.pack('<BBH', 15, width, height))
    output.write(data)

    lineh = get_lineheight(g_curr_face)

    if (height) > lineh:
        g_starty += (height)-lineh + 3   # since Eric draws images 3px lower for alignment


#
# Parse the HTML into the WikiReader's format
#
class WrProcess(HTMLParser.HTMLParser):

    READ_BLOCK_SIZE = 64 * (1024 * 1024)

    def __init__ (self, f, inter_links = True, enable_images = True):
        global g_this_article_title, article_count

        HTMLParser.HTMLParser.__init__(self)
        self.wordwrap = WordWrap.WordWrap(get_utf8_cwidth)
        self.local_init()
        self.tag_stack = []
        self.inter_links = inter_links
        self.enable_images = enable_images
        self.bucket = bucket.Bucket()
        block = f.read(self.READ_BLOCK_SIZE)
        while block:
            self.feed(block)
            block = f.read(self.READ_BLOCK_SIZE)


    def local_init(self):

        global g_starty, g_curr_face, g_halign
        global g_this_article_title, g_links, g_link_cnt

        self.in_html = False
        self.in_title = False
        self.in_body = False

        self.in_h1 = False
        self.in_h2 = False
        self.in_h3 = False
        self.in_h4 = False
        self.in_h5 = False
        self.in_h6 = False

        self.in_table = 0
        self.in_p  = False
        self.in_b  = False
        self.in_big = False
        self.in_strong = False
        self.in_del = False
        self.in_ins = False
        self.in_i  = False
        self.in_a  = False
        self.in_br = False
        self.in_img = False

        self.quote = 0
        self.level = 0
        self.lwidth = DEFAULT_LWIDTH
        self.indent = 0
        self.li_cnt = {}
        self.li_inside = {}
        self.li_type = {}
        self.link_x = 0
        self.link_y = 0
        self.url = None

        self.language_links = []

        self.printing = True

        g_starty = 0
        g_curr_face = DEFAULT_FONT_IDX
        g_halign = 0
        g_this_article_title = 'NO TITLE'
        g_links = {}
        g_link_cnt = 0


    def handle_starttag(self, tag, attrs):

        global g_starty, g_curr_face, g_halign
        global g_this_article_title, g_links, g_link_cnt
        global warnings

        attrs = dict(attrs)

        # must always do the <html> tag
        if tag == 'html':
            self.local_init()
            self.in_html = True
            self.tag_stack = [(tag, True)]
            return

        self.tag_stack.append((tag, self.printing))

        # we want to skip content that isn't for printing
        if 'class' in attrs:
            if 'noprint' in attrs['class']:
                self.printing = False

            # create a list of language links
            if self.inter_links and tag == 'a' and 'lang-link' in attrs['class']:
                self.language_links.append(attrs['href'])

        # handle the tags
        if not self.printing:
            return;

        elif tag == 'script':
            self.printing = False

        elif tag == 'title':
            self.in_title = True
            g_this_article_title = ''

        elif tag == 'body':
            self.in_body = True

        elif tag == 'table':
            self.in_table += 1

        # if in a table suppress everything after this point
        if self.in_table > 0:
            return

        elif tag == 'h1':
            self.flush_buffer()
            self.in_h1 = True
            esc_code0(H1_MARGIN_TOP)

        elif tag == 'h2':
            self.flush_buffer()
            self.in_h2 = True
            esc_code0(H2_MARGIN_TOP)

        elif tag == 'h3':
            self.flush_buffer()
            self.in_h3 = True
            esc_code0(H3_MARGIN_TOP)

        elif tag == 'h4':
            self.flush_buffer()
            self.in_h4 = True
            esc_code0(H4_MARGIN_TOP)

        elif tag == 'h5':
            self.flush_buffer()
            self.in_h5 = True
            esc_code0(H5_MARGIN_TOP)

        elif tag == 'h6':
            self.flush_buffer()
            self.in_h6 = True
            esc_code0(H6_MARGIN_TOP)

        elif tag == 'div':
            self.flush_buffer()
            # suppress thumb info boxes
            if 'class' in attrs:
                c = attrs['class'].lower()
                for ignore in ['thumb', 'left', 'right',
                               'dablink', 'magnify',
                               'navframe', 'navtoggle', 'navcontent',
                               ]:
                    if ignore in c:
                        self.printing = False
                        return
            esc_code0(DIV_MARGIN_TOP)

        elif tag == 'p':
            self.flush_buffer()
            self.in_p = True
            #esc_code0(P_MARGIN_TOP)

        elif tag == 'blockquote':
            self.flush_buffer()
            self.quote += 1
            if self.quote < MAX_QUOTE_LEVEL:
                esc_code0(BLOCKQUOTE_MARGIN_TOP)
                self.indent += BLOCKQUOTE_MARGIN_LEFT
                self.lwidth -= BLOCKQUOTE_MARGIN_LEFT + BLOCKQUOTE_MARGIN_RIGHT
                esc_code9(BLOCKQUOTE_MARGIN_LEFT)

        elif tag == 'b':
            self.in_b = True

        elif tag == 'i':
            self.in_i = True

        elif tag == 'big':            # Not sure what to do with this one
            self.in_b = True

        elif tag == 'strong':
            self.in_b = True

        elif tag == 'del':
            self.in_del = True

        elif tag == 'ins':
            self.in_ins = True

        elif tag == 'a' and 'href' in attrs:
            self.in_a = True
            self.url  = attrs['href']

        elif tag in ['ul', 'ol', 'dl']:
            if 'start' in attrs:
                list_start = re.sub(r'^\D*(\d+)\D?.*$', r'\1', attrs['start'])
                try:
                    list_start = int(list_start)
                except ValueError:
                    list_start = 1

                self.enter_list(tag, list_start)
            else:
                self.enter_list(tag)

        elif tag == 'li':
            if 0 == self.level:
                if warnings:
                    (line, column) = self.getpos()
                    PrintLog.message(u'Warning: stray <{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                     .format(tag, line, column, article_count + 1, g_this_article_title))
                (t, p) = self.tag_stack.pop()
                return  # just ignore it
                # force ul since this is a li without a parent
                #(t, p) = self.tag_stack.pop()
                #self.tag_stack.append(('ul', p))
                #self.tag_stack.append((t,p))
                #self.enter_list('ul')

            # handle missing </li> at the same level
            # simulate </li> and continue
            if self.li_inside[self.level]:
                if warnings:
                    (line, column) = self.getpos()
                    PrintLog.message(u'Warning: missing </{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                     .format(tag, line, column, article_count + 1, g_this_article_title))
                (t, p) = self.tag_stack.pop()
                self.flush_buffer(False)
                self.list_decrease_indent()

            self.li_inside[self.level] = True

            if 'value' in attrs:
                list_index = re.sub(r'^\D*(\d+)\D?.*$', r'\1', attrs['value'])
                try:
                    self.li_cnt[self.level] = int(list_index)
                except ValueError:
                    pass
            else:
                self.li_cnt[self.level] += 1

            if self.li_type[self.level] == 'ol':
                self.wordwrap.append(("{0:d}".format(self.li_cnt[self.level])) + u".", DEFAULT_FONT_IDX, None)
            else:
                if self.level > LIMAX_BULLETS:
                    bullet_num = LIMAX_BULLETS
                else:
                    bullet_num = self.level

                self.wordwrap.append(bullet_c[bullet_num], DEFAULT_FONT_IDX, None)

            self.flush_buffer()
            self.list_increase_indent()

        elif tag == 'dd':
            if 0 == self.level:
                if warnings:
                    (line, column) = self.getpos()
                    PrintLog.message(u'Warning: stray <{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                     .format(tag, line, column, article_count + 1, g_this_article_title))
                (t, p) = self.tag_stack.pop()
                return  # just ignore it
            esc_code0(LIST_MARGIN_TOP)
            if not self.li_inside[self.level]:
                self.li_cnt[self.level] += 1
                self.li_inside[self.level] = True
                self.list_increase_indent()
            elif warnings:
                (line, column) = self.getpos()
                PrintLog.message(u'Warning: nested <{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                 .format(tag, line, column, article_count + 1, g_this_article_title))

        elif tag == 'dt':
            if 0 == self.level:
                if warnings:
                    (line, column) = self.getpos()
                    PrintLog.message(u'Warning: stray <{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                     .format(tag, line, column, article_count + 1, g_this_article_title))
                (t, p) = self.tag_stack.pop()
                return  # just ignore it
            # close unterminated 'dd'
            # i.e. have this  <dt>tag</dt><dd>xxxxx<dt>tag2</dt>.......
            if self.li_inside[self.level]:
                if warnings:
                    (line, column) = self.getpos()
                    PrintLog.message(u'Warning: unterminated <{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                     .format('dd', line, column, article_count + 1, g_this_article_title))
                (t, p) = self.tag_stack.pop()
                self.handle_endtag('dd')
                self.tag_stack.append((t, p))
            esc_code0(LIST_MARGIN_TOP)

        elif tag == 'br':
            self.in_br = True

        elif tag == 'img' and 'src' in attrs:
            # include either image or the 'alt' text
            if self.enable_images:
                (width, height, data) = get_imgdata(attrs['src'], self.indent)
                self.wordwrap.AppendImage(width, height, data, None)
            elif 'alt' in attrs:
                self.handle_data(attrs['alt'])

            self.in_img = True


    def handle_endtag(self, tag):
        global g_this_article_title
        global article_count
        global warnings

        # ignore end tag without start tag
        if (tag, True) not in self.tag_stack and (tag, False) not in self.tag_stack:
            if warnings:
                (line, column) = self.getpos()
                PrintLog.message(u'Warning: superfluous </{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                 .format(tag, line, column, article_count + 1, g_this_article_title))
            return

        # backtrack up the stack closing each open tag until there is a match
        (start_tag, self.printing) = self.tag_stack.pop()
        while start_tag != tag:
            self.tag_stack.append((start_tag, self.printing))
            if warnings:
                (line, column) = self.getpos()
                PrintLog.message(u'Warning: force </{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                 .format(start_tag, line, column, article_count + 1, g_this_article_title))
            self.handle_endtag(start_tag)
            (start_tag, self.printing) = self.tag_stack.pop()

        # must always do </html> tag
        if tag == 'html':
            self.printing = True
            self.tag_stack = []
            self.in_html = False
            esc_code1()
            write_article(self.language_links)
            return

        if not self.printing:
            return

        elif tag == 'script':
            pass

        elif tag == 'title':
            self.in_title = False
            g_this_article_title = g_this_article_title.strip()

        elif tag == 'body':
            self.in_body = False
            self.flush_buffer()

        elif tag == 'table':
            if self.in_table > 0:
                self.in_table -= 1

        # if in a table suppress everything after this point
        if self.in_table > 0:
            return

        elif tag == 'h1':
            self.flush_buffer()
            self.in_h1 = False
            esc_code0(H1_MARGIN_BOTTOM)
            esc_code_separate() # force the above escape code be output

        elif tag == 'h2':
            self.flush_buffer()
            self.in_h2 = False

        elif tag == 'h3':
            self.flush_buffer()
            self.in_h3 = False

        elif tag == 'h4':
            self.flush_buffer()
            self.in_h4 = False

        elif tag == 'h5':
            self.flush_buffer()
            self.in_h5 = False

        elif tag == 'h6':
            self.flush_buffer()
            self.in_h6 = False

        elif tag == 'div':
            self.flush_buffer()

        elif tag == 'p':
            self.flush_buffer()
            self.in_p = False

        elif tag == 'blockquote':
            self.flush_buffer()
            if self.quote > 0:
                if self.quote < MAX_QUOTE_LEVEL:
                    self.indent -= BLOCKQUOTE_MARGIN_LEFT
                    self.lwidth += BLOCKQUOTE_MARGIN_LEFT + BLOCKQUOTE_MARGIN_RIGHT
                    esc_code9(-BLOCKQUOTE_MARGIN_LEFT)
                self.quote -= 1

        elif tag == 'b':
            self.in_b = False

        elif tag == 'big':
            self.in_b = False

        elif tag == 'strong':
            self.in_b = False

        elif tag == 'i':
            self.in_i = False

        elif tag == 'del':
            self.in_del = False

        elif tag == 'ins':
            self.in_ins = False

        elif tag == 'a':
            self.in_a = False
            self.url  = ""

        elif tag in ['ul', 'ol', 'dl']:
            self.leave_list()

        elif tag == 'li':
            if 0 == self.level:
                if warnings:
                    (line, column) = self.getpos()
                    PrintLog.message(u'Warning: stray </{0:s}> @[L{1:d}/C{2:d}] in article[{3:d}]: {4:s}'
                                     .format(tag, line, column, article_count + 1, g_this_article_title))
            else:
                self.flush_buffer(False)
                self.list_decrease_indent()
                self.li_inside[self.level] = False

        elif tag == 'dd':
            self.flush_buffer()
            esc_code0(LIST_MARGIN_TOP)
            if self.li_inside[self.level]:
                self.li_inside[self.level] = False
                self.list_decrease_indent()

        elif tag == 'dt':
            self.flush_buffer()
            esc_code0(LIST_MARGIN_TOP)

        elif tag == 'br':
            self.flush_buffer()
            self.in_br = False

        elif tag == 'img':
            self.in_img = False


    def enter_list(self, list_type, start = 1):
        self.flush_buffer(list_type == 'dl')
        esc_code0(LIST_MARGIN_TOP)
        self.level += 1
        self.li_cnt[self.level] = start - 1
        self.li_inside[self.level] = False
        self.li_type[self.level] = list_type


    def list_increase_indent(self):
        if self.level <= LIMAX_INDENT_LEVELS:
            esc_code9(LIST_INDENT)
            esc_code8(LIST_INDENT)  ### Bug in lcd_buf_draw ASK ERIC
            self.lwidth -= LIST_INDENT
            self.indent += LIST_INDENT


    def leave_list(self):
        self.flush_buffer()
        esc_code0(LIST_MARGIN_TOP)
        if self.level > 0:
            del self.li_cnt[self.level]
            del self.li_inside[self.level]
            self.level -= 1

    def list_decrease_indent(self):
        if self.level <= LIMAX_INDENT_LEVELS:
            esc_code9(- LIST_INDENT)
            self.lwidth += LIST_INDENT
            self.indent -= LIST_INDENT


    def handle_charref(self, name):
        """handle &#DDDD; &#xXXXX;"""

        if 0 == len(name):
            return

        if 'x' == name[0] or 'X' == name[0]:
            try:
                value = int(name[1:], 16)
            except ValueError:
                PrintLog.message(u'charref: "{0:s}" is not hexadecimal'.format(name))
                return

        elif name.isdigit():
            try:
                value = int(name)
            except ValueError:
                PrintLog.message(u'charref: "{0:s}" is not decimal'.format(name))
                return

        try:
            c = unichr(value)
        except ValueError:
            PrintLog.message(u'charref: "{0:d}" is not convertible to unicode'.format(value))
            c = '?'
        self.handle_data(c)


    def handle_entityref(self, name):
        """handle &amp; &gt; ..."""
        try:
            self.handle_data(unichr(htmlentitydefs.name2codepoint[name]))
        except KeyError:
            PrintLog.message(u'ENTITYREF ERROR: {0:s} article: {1:s}'.format(name, g_this_article_title))


    def handle_data(self, data):
        global g_this_article_title

        if self.in_p:
            self.in_p = False
            esc_code0(P_MARGIN_TOP)

        if self.in_title:
            g_this_article_title += data

        # only parse valid tags in <body>
        # skip tables for now
        if not self.in_body or self.in_table > 0 or not self.printing:
            return

        # defaults
        data = re.sub("\s+" , " ", data)
        face = DEFAULT_FONT_IDX
        url  = None

        # only use italic fonts now (don't care about bold)
        if self.in_i:
            face = ITALIC_FONT_IDX

        if self.in_h1:
            face = TITLE_FONT_IDX
        elif self.in_h2 or self.in_h3 or self.in_h4 or self.in_h5 or self.in_h6:
            face = SUBTITLE_FONT_IDX

        # figure out if we need a url
        if self.in_a:
            url = self.url

        self.wordwrap.append(data, face, url)


    def flush_buffer(self, new_line = True):
        global output
        font = -1
        while self.wordwrap.have():
            url = None
            x0 = self.indent
            url_x0 = x0
            line = self.wordwrap.wrap(self.lwidth)
            if line == []:
                break

            if tuple == type(line[0][1]):
                if font < 0:
                    new_font = DEFAULT_FONT_IDX
                else:
                    new_font = font
            else:
                new_font = line[0][1]

            if new_line:
                if font != new_font:
                    font = new_font
                    esc_code3(font)
                else:
                    esc_code2()
            else:
                if font != new_font:
                    font = new_font
                    esc_code4(font)
                new_line = True


            for i in line:
                if tuple == type(i[1]):
                    (width, height, data) = i[1]
                    esc_code14(width, height, data)
                else:
                    if font != i[1]:
                        font = i[1]
                        esc_code4(font)

                    if url != i[2]:
                        if url != None:
                            make_link(url, url_x0, x0, i[0])
                        url = i[2]
                        if url != None:
                            url_x0 = x0
                    output.write(i[0].encode('utf-8'))
                x0 += i[3]

            if url != None:
                make_link(url, url_x0, x0, line[-1][0])


def link_number(url):
    global article_index

    try:
        n = article_index(url)[0]
    except KeyError:
        n = -1
    return n


# Add the '~' padding back here
def article_index(title):
    global article_db

    c = article_db.cursor()
    c.execute('select article_number, fnd_offset, restricted from articles where title = ? limit 1', ["~" + title])
    result = c.fetchone()
    c.close()
    return result  # this returns a tuple of text strings, so beware!


def write_article(language_links):
    global compress
    global verbose
    global output, f_out, i_out
    global article_count
    global g_this_article_title
    global file_number
    global start_time
    global article_writer

    article_count += 1
    if verbose:
        PrintLog.message(u'[MWR {0:d}] {1:s}'.format(article_count, g_this_article_title))

    elif article_count % 1000 == 0:
        now_time = time.time()
        PrintLog.message(u'Render[{0:d}]: {1:7.2f}s {2:10d}'.format(file_number, now_time - start_time, article_count))
        start_time = now_time

    # create links
    links_stream = io.BytesIO('')

    for i in g_links:
        (x0, y0, x1, y1, url) = g_links[i]
        links_stream.write(struct.pack('<3I', (y0 << 8) | x0, (y1 << 8) | x1, link_number(url)))

    links_stream.flush()
    links = links_stream.getvalue()
    links_stream.close()

    # create language links
    links_stream = io.BytesIO('')
    japanese_convert = LanguageTranslation.LanguageJapanese().translate
    normal_convert = LanguageTranslation.LanguageNormal().translate

    for l in language_links:
        language, link = l.split(':', 1)

        if 'ja' == language:
            stripped = japanese_convert(link)
        else:
            stripped = normal_convert(link)

        if link == stripped:
            links_stream.write(l.encode('utf-8') + '\0')
        else:
            links_stream.write((language + '#' + stripped).encode('utf-8') + '\1' + link.encode('utf-8') + '\0')

    links_stream.flush()
    langs = links_stream.getvalue()
    links_stream.close()

    # create the header (header size = 8)
    header = struct.pack('<I2H', 8 + len(links) + len(langs), g_link_cnt, 0)
    body = output.fetch()

    # combine the data
    whole_article = header + links + langs + body

    if compress:
        try:
            (article_number, fnd_offset, restricted) = article_index(g_this_article_title)
            restricted =  bool(int(restricted))  # '0' is True so turn it into False
            article_writer.add_article(article_number, whole_article, fnd_offset, restricted)
        except KeyError:
            PrintLog.message(u'Error in: write_article, Title not found')
            PrintLog.message(u'Title:  {0:s}'.format(g_this_article_title))
            PrintLog.message(u'Offset: {0:s}'.format(file_offset))
            PrintLog.message(u'Count:  {0:s}'.format(article_count))
    else:
        f_out.write(whole_article)
        f_out.flush()


class ArticleWriter(bucket.Bucket):
    """to combine sets of articles and compress them together"""

    def __init__(self, file_number, data_file, index_file,
                 max_buckets = 50, bucket_size = 524288, max_items_per_bucket = 64):

        super(ArticleWriter, self).__init__(max_buckets = max_buckets,
                                       bucket_size = bucket_size,
                                       max_items_per_bucket = max_items_per_bucket)
        self.file_number = file_number
        self.index_file = index_file
        self.data_file = data_file
        self.index = {}


    def add_article(self, article_index, article_data, fnd_offset, restricted):
        self.add((article_index, article_data, fnd_offset, restricted), len(article_data))


    def write(self, data):
        """output the article data"""
        all_data = ''
        blocks = ''
        offset = 0
        for size, item in data:
            article_index, article_data, fnd_offset, restricted = item
            blocks += struct.pack('<3I', article_index,
                                  offset + (0x80000000 if restricted else 0),
                                  size)
            offset += size
            all_data += article_data

        sizeof_one_block = 12 # number of bytes generated by struct above

        ah = chr(len(blocks) / sizeof_one_block) + blocks
        ac = CompressData(all_data)
        file_offset = self.data_file.tell()

        data_length = struct.pack('<I', len(ac))
        self.data_file.write(ah + data_length + ac)

        for size, item in data:
            article_index, article_data, fnd_offset, restricted = item
            self.index[article_index] = struct.pack('<2IB', file_offset, fnd_offset, self.file_number)


    def __del__(self):
        self.flush()
        keys = self.index.keys()
        keys.sort()
        for k in keys:
            self.index_file.write(self.index[k])


# run the program
if __name__ == "__main__":
    main()


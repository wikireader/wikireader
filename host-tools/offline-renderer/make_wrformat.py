#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys, os, struct, os.path, re
import io
from HTMLParser import HTMLParser
import pylzma
import unicodedata
import htmlentitydefs
import codecs
import getopt
import os.path
import cPickle


verbose = False
article_count = 0

fh       = '4b' # struct font_bmf_header (header)
cmr      = '8b48s'  # struct charmetric_bmf (font)

fh_size  = struct.calcsize(fh)
cmr_size = struct.calcsize(cmr)

# font face defines
ITALIC_FONT_IDX         = 1
DEFAULT_FONT_IDX        = 2
BOLD_ITALIC_FONT_IDX    = 3
BOLD_FONT_IDX           = 4
TITLE_FONT_IDX          = 5
SUBTITLE_FONT_IDX       = 6
DEFAULT_ALL_FONT_IDX    = 7
BOLD_ALL_FONT_IDX       = 8

# Screen dimensions
LCD_WIDTH               = 240
LCD_LEFT_MARGIN         = 6     # def. in lcd_buf_draw.h

# Line Spaces (read directly from the font using gdbfed)
LINE_SPACE_ADDON        = 1 # added in lcd_buf_draw.h
H1_LSPACE               = 19
H2_LSPACE               = 17
H3_LSPACE               = H2_LSPACE
H4_LSPACE               = H2_LSPACE
H5_LSPACE               = H2_LSPACE
H6_LSPACE               = H2_LSPACE
P_LSPACE                = 15 + LINE_SPACE_ADDON

# Margins & Spacing
LIST_INDENT             = 16
DIV_MARGIN_TOP          = 10
P_MARGIN_TOP            = DIV_MARGIN_TOP
BLOCKQUOTE_MARGIN_TOP   = DIV_MARGIN_TOP
BLOCKQUOTE_MARGIN_LEFT  = LIST_INDENT
BLOCKQUOTE_MARGIN_RIGHT = LIST_INDENT
LIST_MARGIN_TOP         = DIV_MARGIN_TOP
BR_MARGIN_TOP           = DIV_MARGIN_TOP
DEFAULT_LWIDTH          = (LCD_WIDTH-LCD_LEFT_MARGIN)
H1_MARGIN_TOP           = 8
H1_MARGIN_BOTTOM        = P_MARGIN_TOP
H2_MARGIN_TOP           = 14
H3_MARGIN_TOP           = H2_MARGIN_TOP
H4_MARGIN_TOP           = H2_MARGIN_TOP
H5_MARGIN_TOP           = H2_MARGIN_TOP
H6_MARGIN_TOP           = H2_MARGIN_TOP


LIMAX_INDENT_LEVELS     = 3

# bullet[0] charater is not used (the '!')
bullet_c                = u"!\u25aa\u2022\u2022"

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

article_index = None

output = None
compress = True


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {html-files...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --verbose               Enable verbose output'
    print '       --number=n              Number for the .dat/.idx-tmp files [0]'
    print '       --test=file             Output the uncompressed file for testing'
    print '       --font-path=dir         Path to font files (*.bmf) [fonts]'
    print '       --article-index=file    Article index dictionary input [articles.pickle]'
    print '       --prefix=name           Device file name portion for .dat/.idx-tmp [pedia]'
    exit(1)


def main():
    global verbose, compress
    global f_out, output, i_out
    global font_id_values
    global file_number
    global article_count
    global article_index

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvn:p:i:t:f:', ['help', 'verbose', 'number=', 'prefix=',
                                                                'article-index=',
                                                                'test=', 'font-path='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    data_file = 'pedia%d.dat'
    index_file = 'pedia%d.idx-tmp'
    art_file = 'articles.pickle'
    file_number = 0
    test_file = ''
    font_path = "fonts"

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
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
                usage('%s=%s" is not numeric' % (opt, arg))
        elif opt in ('-p', '--prefix'):
            data_file = arg + '%d.dat'
            index_file = arg + '%d.idx-tmp'
        elif opt in ('-f', '--font-path'):
            font_path = arg
        else:
            usage('unhandled option: ' + opt)

    f_fontr  = open(os.path.join(font_path, "text.bmf"), "r")
    f_fontb  = open(os.path.join(font_path, "textb.bmf"), "r")
    f_fonti  = open(os.path.join(font_path, "texti.bmf"), "r")
    f_fontbi = open(os.path.join(font_path, "textbi.bmf"), "r")
    f_fontt  = open(os.path.join(font_path, "title.bmf"), "r")
    f_fontst = open(os.path.join(font_path, "subtitle.bmf"), "r")
    f_fontall = open(os.path.join(font_path, "textall.bmf"), "r")
    f_fontallb = open(os.path.join(font_path, "textallb.bmf"), "r")

    font_id_values = {
        ITALIC_FONT_IDX: f_fonti,
        DEFAULT_FONT_IDX: f_fontr,
        BOLD_ITALIC_FONT_IDX: f_fontbi,
        BOLD_FONT_IDX: f_fontb,
        TITLE_FONT_IDX: f_fontt,
        SUBTITLE_FONT_IDX: f_fontst,
        DEFAULT_ALL_FONT_IDX: f_fontall,
        BOLD_ALL_FONT_IDX: f_fontallb
    }

    f = open(art_file, 'rb')
    article_index = cPickle.load(f)
    f.close()

    output = io.BytesIO('')

    if test_file == '':
        compress = True
        i_out = open('pedia%d.idx-tmp' % file_number, 'w')
        f_out = open('pedia%d.dat' % file_number, "w")
    else:
        compress = False
        fout = open(test_file, 'w')

    for name in args:
        f = codecs.open(name, "r", 'utf-8')
        WrProcess(f)
        f.close()

    for item in font_id_values:
        font_id_values[item].close()

    output.close()

    if f_out != None:
        f_out.close()
    if i_out != None:
        i_out.close()

    print 'Wrote %d articles' % article_count


#
# Get the width of a character in a given font face
#
width_cache = {}

def get_utf8_cwidth(c, face):
    global width_cache, font_id_values

    if (c, face) in width_cache:
        return width_cache[(c, face)]

    f = font_id_values[face]

    f.seek(c*cmr_size+fh_size);
    buffer = f.read(cmr_size)
    if len(buffer) != 0:
        width, height, widthBytes, widthBits, ascent, descent, LSBearing, RSBearing, bitmap = struct.unpack(cmr, buffer)
        width += LSBearing
    else:
        width = 0
    if width == 0:
        if face == BOLD_ITALIC_FONT_IDX or face == BOLD_FONT_IDX:
            return get_utf8_cwidth(c, BOLD_ALL_FONT_IDX)
        if face != DEFAULT_ALL_FONT_IDX:
            return get_utf8_cwidth(c, DEFAULT_ALL_FONT_IDX)

    width_cache[(c, face)] = width
    return width


def get_lineheight(face):

    values = {
            ITALIC_FONT_IDX:      P_LSPACE,
            DEFAULT_FONT_IDX:     P_LSPACE,
            BOLD_ITALIC_FONT_IDX: P_LSPACE,
            BOLD_FONT_IDX:        P_LSPACE,
            TITLE_FONT_IDX:       H1_LSPACE,
            SUBTITLE_FONT_IDX:    H2_LSPACE,
            DEFAULT_ALL_FONT_IDX: P_LSPACE,
            BOLD_ALL_FONT_IDX:    P_LSPACE,
        }

    return values[face]


def get_textwidth(text, face):
    width = 0
    for c in text:
        width += get_utf8_cwidth(ord(c), face) + LINE_SPACE_ADDON
    return width


# buffer structure: an array of tuples
#    0     1     2    3             4
# [ (text, face, url, total_width, [char_width]) ]

def get_bufferwidth(buffer):
    width = 0
    for b in buffer:
        width += b[3]
    return width


def resize_item(item):
    total_width = 0
    char_widths = []
    for c in item[0]:
        width = get_utf8_cwidth(ord(c), item[1]) + LINE_SPACE_ADDON
        total_width += width
        char_widths.append(width)
    return item[:-2] + (total_width, char_widths)


def remove_one_char(buffer, remainder):
    if buffer == []:
        return (buffer, remainder)
    last_item = buffer[-1]

    removed_char = last_item[0][-1]
    removed_width = last_item[4][-1]
    removed = (removed_char, last_item[1], last_item[2], removed_width, [removed_width])

    text = last_item[0][:-1]
    text_width = last_item[3] - removed_width
    text_chars = last_item[4][:-1]

    if remainder == []:
        remainder = [removed]
    else:
        remainder = merge_two([removed], remainder)

    if text == '':
        return (buffer[:-1], remainder)
    else:
        last_item = (text, last_item[1], last_item[2], text_width, text_chars)
        return (buffer[:-1] + [last_item], remainder)


def merge_two(first, second):
    if first == []:
        return second
    if second == []:
        return first
    first_item = first[-1]
    second_item = second[0]
    if first_item[1] == second_item[1] and first_item[2] == second_item[2]:
        merge = (first_item[0] + second_item[0], first_item[1], first_item[2],
            first_item[3] + second_item[3], first_item[4] + second_item[4])
        return first[:-1] + [merge] + second[1:]
    return first + second


def split_buffer(buffer, remainder):
    if buffer == []:
        return (buffer, remainder)
    last_item = buffer[-1]
    cut = []
    while True:
        tokens = last_item[0].rsplit(None, 1)
        if len(tokens) == 2:
            sp = ''
            if last_item[0][-1] == u' ':
                sp = u' '
            cut = merge_two([resize_item((tokens[1] + sp,) + last_item[1:])], cut)
            buffer[-1] = resize_item((tokens[0] + u' ',) + last_item[1:])
            return (buffer, merge_two(cut, remainder))
        cut = [last_item] + cut
        buffer = buffer[:-1]
        if last_item[0] != last_item[0].lstrip():
            return (buffer, merge_two(cut, remainder))
        if buffer == []:
            return ([], merge_two(cut, remainder))
        last_item = buffer[-1]
        text = last_item[0]
        stripped = text.rstrip()
        if text != stripped:
            buffer[-1] = (text,) + last_item[1:]
            return (buffer, merge_two(cut, remainder))


def truncate_buffer(buffer, maxwidth):
    remainder = buffer
    too_long = False

    width = 0
    buffer = []
    while remainder and width <= maxwidth:
        width += remainder[0][3]
        buffer.append(remainder[0])
        remainder = remainder[1:]

    while width > maxwidth:
        if not too_long:
            (buffer, remainder) = split_buffer(buffer, remainder)
            if buffer == []:
                buffer = remainder
                remainder = []
                too_long = True
        if too_long:
            (buffer, remainder) = remove_one_char(buffer, remainder)
        width = get_bufferwidth(buffer)
    return (buffer, remainder)


def lstrip_buffer(buffer):
    if buffer == []:
        return buffer
    first = buffer[0]
    text = first[0].lstrip()
    if text != '':
        buffer[0] = (text,) + first[1:]
        return buffer
    return strip_buffer(buffer[1:])


def rstrip_buffer(buffer):
    if buffer == []:
         return buffer
    last = buffer[-1]
    text = last[0].rstrip()
    if text != '':
        buffer[-1] = (text,) + last[1:]
        return buffer
    return strip_buffer(buffer[:-1])


def strip_buffer(buffer):
    return(lstrip_buffer(rstrip_buffer(buffer)))


def append_buffer(buffer, text, face, url):
        buffer.append(resize_item((text, face, url, 0, [])))


def render_text(buffer, maxwidth, indent = 0, nl = True):

    font = -1
    x0 = indent

    buffer = rstrip_buffer(buffer)

    while buffer:
        if nl:
            buffer = lstrip_buffer(buffer)
        (line, buffer) = truncate_buffer(buffer, maxwidth)
        for item in line:
            if nl:
                if font != item[1]:
                    font = item[1]
                    esc_code3(font)
                else:
                    esc_code2()
                nl = False
                x0 = indent
            else:
                if font != item[1]:
                    font = item[1]
                    esc_code4(font)
            if item[2]: # has url
                text = item[0].rstrip()
                output.write(text.encode('utf-8'))
                x1 = x0 + get_textwidth(text, item[1])
                try:
                    make_link(item[2], x0, x1, text)
                except struct.error:
                    print 'invalid link:', item[2], x0, x1, text
                    print '..', indent, maxwidth
                    print 'B:', buffer
                x0 = x1
            else:
                x0 += item[3]           # get_textwidth(item[0], item[1])
                output.write(item[0].encode('utf-8'))
        nl = True



def make_link(url, x0, x1, text):
    global g_starty, g_curr_face, g_link_cnt, g_links
    global article_index

    if url in article_index:
        esc_code10(x1 - x0)
        g_links[g_link_cnt] = (x0, g_starty - get_lineheight(g_curr_face), x1, g_starty, url)
        g_link_cnt =  g_link_cnt + 1


def esc_code0(num_pixels):
    """blank line height in pixels"""
    global g_starty

    output.write(struct.pack('BB', 1, num_pixels))
    g_starty += num_pixels


def esc_code1():
    """new line with default font and default line space"""
    global g_starty, g_curr_face

    output.write(struct.pack('B', 2))
    g_starty += get_lineheight(DEFAULT_FONT_IDX)
    g_curr_face = DEFAULT_FONT_IDX


def esc_code2():
    """new line with current font and current line space"""
    global g_starty, g_curr_face

    output.write(struct.pack('B', 3))
    g_starty += get_lineheight(g_curr_face)


def esc_code3(face):
    """new line using new font face."""
    global g_starty, g_curr_face

    num_pixels = get_lineheight(face)
    output.write(struct.pack('BB', 4, face|(num_pixels<<3)))
    g_starty += num_pixels
    g_curr_face = face

def esc_code4(face, halign=0):
    """change font with current horizontal alignment (in pixels)"""
    global g_curr_face

    output.write(struct.pack('BB', 5, face|(halign<<3)))
    g_curr_face = face


def esc_code5():
    """set font as default"""
    global g_curr_face

    output.write(struct.pack('B', 6))
    g_curr_face = DEFAULT_FONT_IDX


def esc_code6():
    """set default alignment"""
    output.write(struct.pack('B', 7))


def esc_code7(num_pixels):
    """move right num_pixels"""
    output.write(struct.pack('BB', 8, num_pixels))


def esc_code8(num_pixels):
    """move left num_pixels"""
    output.write(struct.pack('BB', 9, num_pixels))


def esc_code9(num_pixels):
    """alignment adjustment"""
    global g_halign

    output.write(struct.pack('Bb', 10, num_pixels))
    g_halign = num_pixels


def esc_code10(num_pixels):
    """draw line from right to left"""
    output.write(struct.pack('BB', 11, num_pixels))


#
# Parse the HTML into the WikiReader's format
#
class WrProcess(HTMLParser):

    def __init__ (self, f):
        HTMLParser.__init__(self)
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
        self.level = 0
        self.lwidth = DEFAULT_LWIDTH
        self.indent = 0
        self.buffer = []
        self.li_cnt = {}
        self.li_type = {}
        self.link_x = 0
        self.link_y = 0
        self.url = None
        self.feed(f.read())


    def handle_starttag(self, tag, attrs):

        global g_starty, g_curr_face, g_halign
        global g_this_article_title, g_links, g_link_cnt

        attrs = dict(attrs)

        if tag == 'html':
            self.in_html = True

            # reset the global variables
            # (There should be a better place to put this stuff $$$
            g_starty = 0
            g_curr_face = DEFAULT_FONT_IDX
            g_halign = 0
            g_this_article_title = 'NO TITLE'
            g_links = {}
            g_link_cnt = 0
            self.level = 0
            self.lwidth = DEFAULT_LWIDTH
            self.indent = 0

        if tag == 'title':
            self.in_title = True
            g_this_article_title = ''

        if tag == 'body':
            self.in_body = True

        if tag == 'h1':
            self.flush_buffer()
            self.in_h1 = True
            esc_code0(H1_MARGIN_TOP)

        if tag == 'h2':
            self.flush_buffer()
            self.in_h2 = True
            esc_code0(H2_MARGIN_TOP)

        if tag == 'h3':
            self.flush_buffer()
            self.in_h3 = True
            esc_code0(H3_MARGIN_TOP)

        if tag == 'h4':
            self.flush_buffer()
            self.in_h4 = True
            esc_code0(H4_MARGIN_TOP)

        if tag == 'h5':
            self.flush_buffer()
            self.in_h5 = True
            esc_code0(H5_MARGIN_TOP)

        if tag == 'h6':
            self.flush_buffer()
            self.in_h6 = True
            esc_code0(H6_MARGIN_TOP)

        if tag == 'div':
            self.flush_buffer()
            esc_code0(DIV_MARGIN_TOP)

        if tag == 'table':
            self.in_table += 1

        if tag == 'p':
            self.flush_buffer()
            self.in_p = True
            esc_code0(P_MARGIN_TOP)

        if tag == 'blockquote':
            self.flush_buffer()
            esc_code0(BLOCKQUOTE_MARGIN_TOP)
            self.indent += BLOCKQUOTE_MARGIN_LEFT
            self.lwidth -= BLOCKQUOTE_MARGIN_LEFT + BLOCKQUOTE_MARGIN_RIGHT
            esc_code9(BLOCKQUOTE_MARGIN_LEFT)

        if tag == 'b':
            self.in_b = True

        if tag == 'i':
            self.in_i = True

        if tag == 'big':            # Not sure what to do with this one
            self.in_b = True

        if tag == 'strong':
            self.in_b = True

        if tag == 'del':
            self.in_del = True

        if tag == 'ins':
            self.in_ins = True

        if tag == 'a' and 'href' in attrs:
            self.in_a = True
            self.url  = attrs['href']

        if tag in ['ul', 'ol', 'dl']:
            self.enter_list(tag)

        if tag == 'li':
            self.li_cnt[self.level] += 1

            if self.li_type[self.level] == 'ol':
                append_buffer(self.buffer, ("%d" % self.li_cnt[self.level]) + u".", DEFAULT_FONT_IDX, None)
            else:
                if self.level > LIMAX_INDENT_LEVELS:    # we only have 3 types of bullets
                    bullet_num = 3;
                else:
                    bullet_num = self.level

                append_buffer(self.buffer, bullet_c[bullet_num], DEFAULT_FONT_IDX, None)

            self.flush_buffer()
            if self.level < LIMAX_INDENT_LEVELS:
                esc_code9(LIST_INDENT)
                esc_code8(LIST_INDENT)  ### Bug in lcd_buf_draw ASK ERIC

        if tag == 'dd':
            self.li_cnt[self.level] += 1
            if self.level < LIMAX_INDENT_LEVELS:
                esc_code9(LIST_INDENT)
                esc_code8(LIST_INDENT)  ### Bug in lcd_buf_draw ASK ERIC

        if tag == 'br':
            self.in_br = True


    def handle_endtag(self, tag):
        global g_this_article_title

        if tag == 'html':
            self.in_html = False
            esc_code1()
            write_article()

        if tag == 'title':
            self.in_title = False
            g_this_article_title = g_this_article_title.strip()

        if tag == 'body':
            self.in_body = False
            self.flush_buffer()

        if tag == 'h1':
            self.flush_buffer()
            self.in_h1 = False
            esc_code0(H1_MARGIN_BOTTOM)

        if tag == 'h2':
            self.flush_buffer()
            self.in_h2 = False

        if tag == 'h3':
            self.flush_buffer()
            self.in_h3 = False

        if tag == 'h4':
            self.flush_buffer()
            self.in_h4 = False

        if tag == 'h5':
            self.flush_buffer()
            self.in_h5 = False

        if tag == 'h6':
            self.flush_buffer()
            self.in_h6 = False

        if tag == 'div':
            self.flush_buffer()

        if tag == 'table':
            self.in_table -= 1

        if tag == 'p':
            self.flush_buffer()
            self.in_p = False

        if tag == 'blockquote':
            self.flush_buffer()
            self.indent -= BLOCKQUOTE_MARGIN_LEFT
            self.lwidth += BLOCKQUOTE_MARGIN_LEFT + BLOCKQUOTE_MARGIN_RIGHT
            esc_code9(-BLOCKQUOTE_MARGIN_LEFT)

        if tag == 'b':
            self.in_b = False

        if tag == 'big':
            self.in_b = False

        if tag == 'strong':
            self.in_b = False

        if tag == 'i':
            self.in_i = False

        if tag == 'del':
            self.in_del = False

        if tag == 'ins':
            self.in_ins = False

        if tag == 'a':
            self.in_a = False
            self.url  = ""

        if tag in ['ul', 'ol', 'dl']:
            self.leave_list()

        if tag == 'li':
            self.flush_buffer(False)
            esc_code9(- LIST_INDENT)

        if tag == 'dd':
            self.flush_buffer()
            esc_code9(- LIST_INDENT)

        if tag == 'dt':
            self.flush_buffer()

        if tag == 'br':
            self.flush_buffer()
            self.in_br = False


    def enter_list(self, list_type):
        self.flush_buffer()
        esc_code0(LIST_MARGIN_TOP)
        self.level += 1
        self.li_cnt[self.level] = 0
        self.li_type[self.level] = list_type
        if self.level < LIMAX_INDENT_LEVELS:
            self.lwidth -= LIST_INDENT
            self.indent += LIST_INDENT


    def leave_list(self):
        self.flush_buffer()
        self.level -= 1
        if self.level < LIMAX_INDENT_LEVELS:
            self.lwidth += LIST_INDENT
            self.indent -= LIST_INDENT


    def handle_charref(self, name):
        self.handle_data(unichr(int(name)))


    def handle_entityref(self, name):
        self.handle_data(unichr(htmlentitydefs.name2codepoint[name]))


    def handle_data(self, data):
        global g_this_article_title

        if self.in_title:
            g_this_article_title += data

        # only parse valid tags in <body>
        # skip tables for now
        if not self.in_body or self.in_table > 0:
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

        append_buffer(self.buffer, data, face, url)


    def flush_buffer(self, new_line = True):
        render_text(self.buffer, self.lwidth, self.indent, new_line)
        self.buffer = []


def link_number(url):
    global article_index

    try:
        n = article_index[url][0]
    except KeyError:
        n = -1
    return n


# write this article
def write_article():
    global compress
    global verbose
    global output, f_out, i_out
    global article_count
    global article_index

    article_count += 1
    if verbose:
        print "[MWR %d] %s" % (article_count, g_this_article_title)
        sys.stdout.flush()

    output.flush()

    # create link
    links_stream = io.BytesIO('')

    for i in g_links:
        (x0, y0, x1, y1, url) = g_links[i]
        links_stream.write(struct.pack('LLL', (y0 << 8) | x0, (y1 << 8) | x1, link_number(url)))

    links_stream.flush()
    links = links_stream.getvalue()
    links_stream.close()

    header = struct.pack('L2H', 8 + len(links), g_link_cnt, 0)
    body = output.getvalue()

    file_offset = f_out.tell()
    if compress:
        body = chr(5) + pylzma.compress(header+links+body, dictionary = 24, fastBytes = 32,
                                literalContextBits = 3,
                                literalPosBits = 0, posBits = 2, algorithm = 1, eos = 1)
        f_out.write(body)
    else:
        f_out.write(header)
        f_out.write(links)
        f_out.write(body)
    output.truncate(0)
    if compress:
        i_out.write(struct.pack('LLL', file_offset, article_index[g_this_article_title][1], (0x80 << 24) | (file_number << 24) | len(body)))

# run the program
if __name__ == "__main__":
    main()


#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, re
import struct
import littleparser
import getopt
import os.path
import cPickle


tparser = littleparser.LittleParser() # for handling titles

# this _must_ be in ascending ASCII sequence
KEYPAD_KEYS = """ !#$%&'()*+,-.0123456789=?@abcdefghijklmnopqrstuvwxyz"""

# to check if in order: uncomment and look at result
#for c in KEYPAD_KEYS:
#    print ord(c)


# titles
title_tag = re.compile(r'</?title>', re.IGNORECASE)

# redirect: <text.....#redirect.....[[title#relative link]].....
redirected_to = re.compile(r'<text\s+xml:space="preserve">\s*#redirect[^\[]*\[\[(.*?)([#|].*?)?\]\]', re.IGNORECASE)

# Filter out Wikipedia's non article namespaces
non_articles = re.compile(r'User\:|Wikipedia\:|File\:|MediaWiki\:|Template\:|Help\:|Category\:|Portal\:', re.IGNORECASE)

# underscore and space
whitespaces = re.compile(r'([\s_]+)', re.IGNORECASE)


verbose = False
bigram = {}
redirects = {}

# format: [article_number, fnd_index]
article_index = {}

index_matrix = {}

article_file_offsets = {}
modulo = 1

idx = 1 # first article number; cumulative count of all articles


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {xlm-file...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --verbose               Enable verbose output'
    print '       --article-index=file    Article index dictionary output [articles.pickle]'
    print '       --article-offsets=file  Article file offsets dictionary output [offsets.pickle]'
    print '       --article-counts=file   File to store the counts [counts.text]'
    print '       --modulo=n              only save the offsets for "mod n" articles [1] (1k => 1000)'
    print '       --prefix=name           Device file name portion for .fnd/.pfx [pedia]'
    exit(1)

def main():
    global verbose
    global modulo
    global redirects, article_index

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvi:o:c:m:p:',
                                   ['help', 'verbose',
                                    'article-index=',
                                    'article-offsets=',
                                    'article-counts=',
                                    'modulo=', 'prefix='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    art_name = "articles.pickle"
    off_name = "offsets.pickle"
    cnt_name = "counts.text"
    fnd_name = 'pedia.fnd'
    pfx_name = 'pedia.pfx'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-i', '--article-index'):
            art_name = arg
        elif opt in ('-o', '--article-offsets'):
            off_name = arg
        elif opt in ('-c', '--article-counts'):
            cnt_name = arg
        elif opt in ('-m', '--modulo'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            try:
                modulo = int(arg)
            except ValueError:
                usage('%s=%s" is not numeric' % (opt, arg))
            if modulo < 1:
                usage('%s=%s" must be >= 1' % (opt, arg))
        elif opt in ('-p', '--prefix'):
            fnd_name = arg + '.fnd'
            pfx_name = arg + '.pfx'
        else:
            usage('unhandled option: ' + opt)

    for f in args:
        process_file(f)

    # record initial counts
    a = len(article_index)
    r = len(redirects)

    # add redirect to article_index
    for item in redirects:
        try:
            article_index[item] = find(item)
        except KeyError:
            print 'Invalid redirect:', item, '->', redirects[item]

    # record combined count and display statistics
    m = len(article_index)
    s = a + r

    cf = open(cnt_name, 'w')

    for f in (sys.stdout, cf):
        f.write('Articles:   %10d\n' % a)
        f.write('Redirects:  %10d\n' % r)
        f.write('Sum:        %10d\n' % s)
        f.write('Merged:     %10d\n' % m)
        f.write('Difference: %10d\n' % (m - s))

    cf.close()

    output_fnd(fnd_name)
    output_pfx(pfx_name)
    output_index(art_name)
    output_offsets(off_name)


def generate_bigram(text):
    global bigram, redirects, article_index
    if len(text) > 2:
        try:
            if ord(text[0]) < 128 and ord(text[1]) < 128:
                bigram[text[0:2]] += 1
        except KeyError:
            bigram[text[0:2]] = 1

    if len(text) > 4:
        try:
            if ord(text[2]) < 128 and ord(text[3]) < 128:
                bigram[text[2:4]] += 1
        except KeyError:
            bigram[text[2:4]] = 1


def process_file(filename):
    global title_tag, redirected_to, non_articles, whitespaces
    global redirects, article_index, idx
    global article_offsets
    global modulo
    global verbose

    skip  = False
    redirect = False
    f = open(filename, 'r')
    current_offset = 0
    article_file_offsets[idx] = (0, filename) # start of file

    line  = f.readline()

    while line:
        lower_line = line.lower()

        if "<title>" in lower_line:
            redirect = False
            skip = False

            if non_articles.search(line):
                skip = True    # we only need articles
            else:
                title = tparser.translate(title_tag.sub('', line.strip())).strip(u'\u200e\u200f')
                if verbose:
                    print 'Title:', title
                generate_bigram(title)

        if '</page>' in lower_line:
            current_offset = f.tell();

        if not skip and "#redirect" in lower_line:
            match = redirected_to.search(line)
            if match:
                redirect_title = tparser.translate(match.group(1)).strip().strip(u'\u200e\u200f')
                redirect_title = whitespaces.sub(' ', redirect_title).strip()
                redirect = True
                redirects[title] = redirect_title
                if verbose:
                    print 'Redirect:', title

        if not skip and "</text>" in lower_line:
            skip = False
            if not redirect:
                article_index[title] = [idx, -1]
                if idx % modulo == 0:
                    article_file_offsets[idx] = (current_offset, filename)
                idx = idx + 1

                if verbose:
                    if idx % 10000 == 0:
                        print idx

        line = f.readline()
    f.close()


def find(title):
    """get index from article title

    also handles redirects
    returns: [index, fnd]
    """
    global redirects, article_index

    try:
        title = redirects[title]
    except KeyError:
        title = redirects[title[0].swapcase() + title[1:]]

    try:
        result = article_index[title]
    except KeyError:
        try:
            result = article_index[title[0].swapcase() + title[1:]]
        except:
            result = find(title)
    return result


import unicodedata
def strip_accents(s):
   return ''.join((c for c in unicodedata.normalize('NFD', s) if unicodedata.category(c) != 'Mn'))


def bigram_encode(title):
    global bigram

    result = ''
    title = strip_accents(title)

    while len(title) >= 2:
        if ord(title[0]) < 128:
            b = title[0:2]
            if b in bigram:
                result += bigram[b]
                title = title[2:]
            else:
                result += chr(ord(title[0:1]))
                title = title[1:]
        else:
            #result += '?'
            title = title[1:]
    if len(title) == 1:
        if ord(title[0]) < 128:
            result += chr(ord(title[0]))
        #else:
        #    result += '?'
    return result


def output_fnd(filename):
    """create bigram table"""
    global bigram, article_index
    global index_matrix

    print 'Writing:', filename
    out_f = open(filename, 'w')

    sortedgram = [ (value, key) for key, value in bigram.iteritems() ]
    sortedgram.sort()
    sortedgram.reverse()

    bigram = {}
    i = 0
    for k, v in sortedgram:
        out_f.write(v)
        bigram[v] = chr(i + 128)
        i += 1
        if i >= 128:
            break
    while i < 128:
        out_f.write('zz')
        bigram['zz'] = chr(i + 128)
        i += 1

    # create pfx matrix and write encoded titles
    article_list = article_index.keys()
    article_list.sort()
    index_matrix = {}
    index_matrix['\0\0\0'] = out_f.tell()
    for title in article_list:
        offset = out_f.tell()
        key3 = (title[0:3] + '\0\0\0')[0:3].lower()
        key2 = key3[0:2] + '\0'
        key1 = key3[0:1] + '\0\0'
        if key1 not in index_matrix:
            index_matrix[key1] = offset
        if key2 not in index_matrix:
            index_matrix[key2] = offset
        if key3 not in index_matrix:
            index_matrix[key3] = offset
        article_index[title][1] = offset
        out_f.write(struct.pack('Ib', article_index[title][0], 0) + bigram_encode(title) + '\0')

    out_f.close()


def output_pfx(filename):
    """output the pfx matrix"""
    global index_matrix

    print 'Writing:', filename
    out_f = open(filename, 'w')
    list = '\0' + KEYPAD_KEYS
    for k1 in list:
        for k2 in list:
            for k3 in list:
                key = k1+k2+k3
                if key in index_matrix:
                    offset = index_matrix[key]
                else:
                    offset = 0
                out_f.write(struct.pack('I', offset))

    out_f.close()


def output_index(filename):
    """output the article data"""
    global article_index

    print 'Writing:', filename
    output = open(filename, 'wb')
    cPickle.dump(article_index, output)
    output.close()


def output_offsets(filename):
    """output file offsets"""
    global article_file_offsets

    print 'Writing:', filename
    output = open(filename, 'wb')
    cPickle.dump(article_file_offsets, output)
    output.close()


# run the program
if __name__ == "__main__":
    main()

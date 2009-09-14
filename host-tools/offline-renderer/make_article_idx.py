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


tparser = littleparser.LittleParser() # for handling titles

# this _must_ be in ascending ASCII sequence
KEYPAD_KEYS = """ !#$%&'()*+,-.0123456789=?@abcdefghijklmnopqrstuvwxyz"""

# to check if in order: uncomment and look at result
#for c in KEYPAD_KEYS:
#    print ord(c)


x1 = re.compile(r'</?title>')
x2 = re.compile(r'[^"]*\[\[([^"]*)\]\]')

# Filter out Wikipedia's non article namespaces
x3 = re.compile(r'User\:|Wikipedia\:|File\:|MediaWiki\:|Template\:|Help\:|Category\:|Portal\:', re.IGNORECASE)


verbose = False
bigram = {}
redirects = {}
article_index = {}
article_fnd = {}
index_matrix = {}

idx = 1 # first article number


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s [--verbose] [--out=file] [--prefix=name]' % os.path.basename(__file__)
    exit(1)

def main():
    global verbose
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvo:p:', ['help', 'verbose', 'out=', 'prefix='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    out_name = "article_idx.py"
    fnd_name = 'pedia.fnd'
    pfx_name = 'pedia.pfx'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-o', '--out'):
            out_name = arg
        elif opt in ('-p', '--prefix'):
            fnd_name = arg + '.fnd'
            pfx_name = arg + '.pfx'
        else:
            usage('unhandled option: ' + opt)

    for f in args:
        process_file(f)

    # add redirect to article_index
    for item in redirects:
        article_index[item] = find(item)

    output_fnd (fnd_name)
    output_pfx (pfx_name)
    output_index (out_name)



def generate_bigram (text):
    global bigram, redirects, article_index, article_fnd, idx
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


def process_file (filename):
    global x1, x2, x3
    global redirects, article_index, idx
    global verbose

    skip  = False
    redirect = False
    f = open(filename, 'r')

    line  = f.readline()

    while line:

        if "<title>" in line:
            redirect = False

            if x3.search(line):
                skip = True    # we only need articles
            else:
                title = tparser.translate(x1.sub('', line.strip()))
                # title = x1.sub('', line).strip()
                # ??? not here ?? title = map_accented_chars(title)
                if verbose:
                    print 'Title:', title
                generate_bigram(title)

        if skip is False and "#redirect" in line:
            x = re.compile(r'<text xml:space="preserve">')
            line = x.sub('', line)
            redirect_title = tparser.translate(x2.match(line).group(1))
            redirect = True
            redirects[title] = redirect_title
            if verbose:
                print 'Redirect:', title

        if not skip and "</text>" in line:
            skip = False
            if not redirect:
                article_index[title] = idx
                idx = idx + 1

                if verbose:
                    if idx % 10000 == 0:
                        print idx

        line = f.readline()
    f.close()


def find (title):
    """get index from article title

    also handles redirects
    """
    global redirects, article_index

    title = redirects[title]
    try:
        number = article_index[title]
    except KeyError:
        number = find(title)
    return number


def bigram_encode (title):
    global bigram
    result = ''
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
            result += '?'
            title = title[1:]
    if len(title) == 1:
        if ord(title[0]) < 128:
            result += chr(ord(title[0]))
        else:
            result += '?'
    return result



def output_fnd (fnd_name):
    """create bigram table"""
    global bigram, article_index, article_fnd
    global index_matrix
    out_f = open(fnd_name, 'w')

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
        article_fnd[title] = offset
        out_f.write(struct.pack('Lb', article_index[title], 0) + bigram_encode(title) + '\0')

    out_f.close()


def output_pfx (pfx_name):
    """output the pfx matrix"""
    global index_matrix

    out_f = open(pfx_name, 'w')
    list = '\0' + KEYPAD_KEYS
    for k1 in list:
        for k2 in list:
            for k3 in list:
                key = k1+k2+k3
                if key in index_matrix:
                    offset = index_matrix[key]
                else:
                    offset = 0
                out_f.write(struct.pack('L', offset))

    out_f.close()


def output_index (out_name):
    """output python code for redirected titles to article hash table"""
    global article_index, article_fnd

    out_f = open(out_name, "w")

    # write article indexs into a python dictionary
    out_f.write("#! /usr/bin/env python\n")
    out_f.write("# -*- coding: utf-8 -*-\n\n")

    out_f.write("def idx(title):\n")
    out_f.write("    global article_indices\n")
    out_f.write("    if title not in article_indices:\n")
    out_f.write("        return 0\n")
    out_f.write("    return article_indices[title][0]\n")
    out_f.write("\n")

    out_f.write("def fnd(title):\n")
    out_f.write("    global article_indices\n")
    out_f.write("    if title not in article_indices:\n")
    out_f.write("        return 0\n")
    out_f.write("    return article_indices[title][1]\n")
    out_f.write("\n")

    out_f.write("article_indices = {\n")

    # output python code for article hash table
    for item in article_index:
        out_f.write('    """%s""": [%d, %d],\n' % (item.encode('utf-8'), article_index[item], article_fnd[item]))

    # and the redirects
    #for item in redirects:
    #    out_f.write('*** "%s": [%d, %d],\n' % (item.encode('utf-8'), find(item), 0))

    # end of python output, close dictionary
    out_f.write("}\n")
    out_f.close()


# run the program
if __name__ == "__main__":
    main()

#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, re, subprocess
import getopt
import os.path

verbose = False

# Regular expressions for parsing the XML
start_text = re.compile(r'<text xml:space="preserve">', re.IGNORECASE)
title_tag = re.compile(r'</?title>', re.IGNORECASE)
end_article = re.compile(r'==\s*External\s+links\s*==|</text>', re.IGNORECASE)

begin_ignore = re.compile(r'^\s*(&lt;!--|&lt;gallery&gt;)\s*$', re.IGNORECASE)
end_ignore = re.compile(r'^\s*(--&gt;|&lt;/gallery&gt;)\s*$', re.IGNORECASE)
inline_comment= re.compile(r'(&lt;!--.*?--&gt;|&lt;ref.*?&lt;/ref&gt;)', re.IGNORECASE)

line_break = re.compile(r'&lt;br /&gt;', re.IGNORECASE)
entities = re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE)

img = re.compile(r'\[\[(file|image):(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE)

# Filter out Wikipedia's non article namespaces
no_parse = re.compile(r'User\:|Wikipedia\:|File\:|MediaWiki\:|Template\:|Help\:|Category\:|Portal\:', re.IGNORECASE)

redirect = re.compile(r'<text xml:space="preserve">#redirect.*</text>', re.IGNORECASE)

def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s [--verbose] [--out=file]' % os.path.basename(__file__)
    exit(1)

def main():
    global verbose
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvo:p:', ['help', 'verbose', 'out=', 'prefix='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    out_name = 'all_articles.html'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-o', '--out'):
            out_name = arg
        else:
            usage('unhandled option: ' + opt)

    newf = subprocess.Popen('(cd mediawiki-offline && php wr_parser.php -) > ' + out_name, shell=True, stdin=subprocess.PIPE).stdin

    for f in args:
        process_file(f, newf)

    newf.close()


def process_file (file_name, newf):
    global verbose
    global start_text, title_tag, end_article
    global begin_ignore, end_ignore, inline_comment, line_break
    global entities, img, no_parse

    cnt = 1

    if verbose:
        print 'Reading:', file_name

    f = open(file_name, 'r')
    line    = f.readline()
    parse   = False
    ignore  = False
    skip    = False

    while line:

        if "<title>" in line:
            parse = False
            if no_parse.search(line):
                skip = True    # we only need articles
            else:
                skip = False
                title = title_tag.sub('', line.strip())
                #title = title_tag.sub('', line.strip())


        if redirect.search(line):
            pass
        elif not skip and not parse and "<text xml:space=\"preserve\">" in line:
            line = start_text.sub('', line)
            parse = True
            newf.write(title);

            if verbose:
                print "[PA %d] " %cnt + title
                cnt += 1

            newf.write('\n__NOTOC__\n')
            ignore = False
            had_blank = False

        if parse:
            line = inline_comment.sub('', line.strip())
            line = line_break.sub('\n', line)
            line = entities.sub(r'&\1;', line)
            if end_article.search(line):
                newf.write('***EOF***\n')
                parse = False

            elif begin_ignore.search(line):
                ignore = True

            elif end_ignore.search(line):
                ignore = False

            elif not ignore:
                line2 = img.sub('', line)
                if line2 == line:
                    if line.strip() == '':
                        if not had_blank:
                            newf.write('\n');
                            had_blank = True
                    else:
                        had_blank = False
                        newf.write(line + '\n');

        line = f.readline()


    f.close()


# run the program
if __name__ == "__main__":
    main()


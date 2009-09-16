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
import cPickle

verbose = False

PARSER_COMMAND = '(cd mediawiki-offline && php wr_parser.php -)'

# Regular expressions for parsing the XML
start_text = re.compile(r'<text xml:space="preserve">', re.IGNORECASE)
title_tag = re.compile(r'</?title>', re.IGNORECASE)
end_article = re.compile(r'==\s*External\s+links\s*==|</text>', re.IGNORECASE)

begin_ignore = re.compile(r'^\s*(&lt;!--|&lt;gallery&gt;)\s*$', re.IGNORECASE)
end_ignore = re.compile(r'^\s*(--&gt;|&lt;/gallery&gt;)\s*$', re.IGNORECASE)
inline_comment = re.compile(r'(&lt;!--.*?--&gt;|&lt;ref.*?&lt;/ref&gt;)', re.IGNORECASE)
inline_ref = re.compile(r'(&lt;ref\s+name.*?/&gt;)', re.IGNORECASE)

comment_start = re.compile(r'&lt;!--.*$', re.IGNORECASE)
comment_end =  re.compile(r'^.*--&gt;', re.IGNORECASE)

ref_start = re.compile(r'&lt;ref.*?&gt;.*$', re.IGNORECASE)
ref_end = re.compile(r'^.*&lt;/ref&gt;', re.IGNORECASE)

delete_tags = re.compile(r'&lt;.*?&gt;', re.IGNORECASE)

line_break = re.compile(r'&lt;br /&gt;', re.IGNORECASE)
entities = re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE)

img = re.compile(r'\[\[(file|image):(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE)

# Filter out Wikipedia's non article namespaces
no_parse = re.compile(r'User\:|Wikipedia\:|File\:|MediaWiki\:|Template\:|Help\:|Category\:|Portal\:', re.IGNORECASE)

redirect = re.compile(r'<text xml:space="preserve">#redirect.*</text>', re.IGNORECASE)

def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {xml-file...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --verbose               Enable verbose output'
    print '       --xhtml=file            XHTML output [all_articles.html]'
    print '       --start=n               First artcle to process [1] (1k => 1000)'
    print '       --count=n               Number of artcles to process [all] (1k => 1000)'
    print '       --article-offsets=file  Article file offsets dictionary input [offsets.pickle]'
    print '       --just-cat              Replace php parset be "cat" for debugging'
    exit(1)

def main():
    global verbose
    global PARSER_COMMAND

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvx:s:co:j', ['help', 'verbose', 'xhtml=',
                                                              'start=', 'count=',
                                                              'article-offsets=',
                                                              'just-cat',
                                                              ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    out_name = 'all_articles.html'
    off_name = 'offsets.pickle'
    start_article = 1
    article_count = 'all'
    processing_articles = True

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-x', '--xhtml'):
            out_name = arg
        elif opt in ('-o', '--article-offsets'):
            off_name = arg
        elif opt in ('-j', '--just-cat'):
            PARSER_COMMAND = 'cat'
        elif opt in ('-s', '--start'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            try:
                start_article = int(arg)
            except ValueError:
                usage('%s=%s" is not numeric' % (opt, arg))
            if start_article < 1:
                usage('%s=%s" must be >= 1' % (opt, arg))
        elif opt in ('-c', '--count'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            if arg != 'all':
                try:
                    article_count = int(arg)
                except ValueError:
                    usage('%s=%s" is not numeric' % (opt, arg))
            if article_count <= 0:
                usage('%s=%s" must be > zero' % (opt, arg))
        else:
            usage('unhandled option: ' + opt)


    f = open(off_name, 'rb')
    article_file_offsets = cPickle.load(f)
    f.close()

    parsing_articles = start_article == 1 and article_count == 'all'

    if not parsing_articles:
        (seek, filename) = article_file_offsets[start_article]

    newf = subprocess.Popen(PARSER_COMMAND + ' > ' + out_name, shell=True, stdin=subprocess.PIPE).stdin

    for f in args:
        if not parsing_articles:
            if f == filename:
                parsing_articles = True
                article_count = process_file(f, seek, article_count, newf)
        else:
            article_count = process_file(f, 0, article_count, newf)

        if article_count != 'all' and article_count == 0:
            break

    newf.close()


def process_file(file_name, seek, count, newf):
    global verbose
    global start_text, title_tag, end_article
    global begin_ignore, end_ignore, inline_comment, line_break
    global entities, img, no_parse

    if verbose:
        print 'Reading:', file_name

    f = open(file_name, 'r')
    if seek > 0:
        f.seek(seek)

    line    = f.readline()
    parse   = False
    ignore  = False
    skip    = False
    comment = False
    ref = False

    while line:

        if "<title>" in line:
            parse = False
            comment = False
            ref = False
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

            if parse:
                newf.write(title);
                if verbose:
                    print "[PA] " + title
                newf.write('\n__NOTOC__\n')
                ignore = False
                had_blank = False

        if parse:
            line = inline_comment.sub('', line.strip())
            line = inline_ref.sub('', line)
            if comment:
                if comment_end.search(line):
                    line = comment_end.sub('', line)
                    comment = False
                else:
                    line = None
            else:
                if comment_start.search(line):
                    line = comment_start.sub('', line)
                    comment = True

            if line != None:
                if ref:
                    if ref_end.search(line):
                        line = ref_end.sub('', line)
                        ref = False
                    else:
                        line = None
                else:
                    if ref_start.search(line):
                        line = ref_start.sub('', line)
                        ref = True

            if line != None:
                line = delete_tags.sub('', line)
                line = line_break.sub('\n', line)
                line = entities.sub(r'&\1;', line)
                if end_article.search(line):
                    newf.write('***EOF***\n')
                    if count != 'all':
                        count -= 1
                        if count <= 0:
                            break
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
    return count


# run the program
if __name__ == "__main__":
    main()


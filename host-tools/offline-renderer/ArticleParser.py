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
import sqlite3
from types import *

verbose = False


PARSER_COMMAND = '(cd mediawiki-offline && php wr_parser.php -)'

# Regular expressions for parsing the XML



subs = [
    (re.compile(r'\s*(==\s*External\s+links\s*==.*)$', re.IGNORECASE + re.DOTALL), ''),

    (re.compile(r'\s*(<|&lt;)gallery(>|&gt;).*?(<|&lt;)/gallery(>|&gt;)', re.IGNORECASE + re.DOTALL), ''),

    (re.compile(r'((<|&lt;)!--.*?--(>|&gt;)|(<|&lt;)ref.*?(<|&lt;)/ref(>|&gt;))', re.IGNORECASE + re.DOTALL), ''),

    (re.compile(r'((<|&lt;)ref\s+name.*?/(>|&gt;))', re.IGNORECASE), ''),

    (re.compile(r'(<|&lt;)br\s+/(>|&gt;)', re.IGNORECASE), '\n'),

#img = re.compile(r'\[\[(file|image):(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE)
    (re.compile(r'\[\[(file|image):.*$', re.IGNORECASE + re.MULTILINE), ''),

#img = re.compile(r'\[\[(file|image):([^\[]|\s|\[(\s|[^\[])*\]|\[\[(\s|[^\[])*\]\])*?\]{2,3}', re.IGNORECASE + re.DOTALL)

    (re.compile(r'\[\[\w\w:(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE), ''),

# Wikipedia's installed Parser extension tags
# <categorytree>, <charinsert>, <hiero>, <imagemap>, <inputbox>, <poem>,
# <pre>, <ref>, <references>, <source>, <syntaxhighlight> and <timeline>
# All referenced using special characters
# Ex: <timeline> --> &lt;timeline&gt;
# For now, we're only going to remove <timeline>
    (re.compile(r'\s*(<|&lt;)timeline(>|&gt;).*?(<|&lt;)/timeline(>|&gt;)', re.IGNORECASE + re.DOTALL), ''),
    (re.compile(r'\s*(<|&lt;)imagemap(>|&gt;).*?(<|&lt;)/imagemap(>|&gt;)', re.IGNORECASE + re.DOTALL), ''),

    (re.compile(r'(<|&lt;).*?(>|&gt;)', re.IGNORECASE), ''),

    (re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE), r'&\1;')

]


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {xml-file...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --verbose               Enable verbose output'
    print '       --xhtml=file            XHTML output [all_articles.html]'
    print '       --start=n               First artcle to process [1] (1k => 1000)'
    print '       --count=n               Number of artcles to process [all] (1k => 1000)'
    print '       --article-offsets=file  Article file offsets database input [offsets.db]'
    print '       --just-cat              Replace php parset be "cat" for debugging'
    print '       --no-output             Do not run any parsing'
    exit(1)

def main():
    global verbose
    global PARSER_COMMAND
    global total_articles

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvx:s:c:o:jn',
                                   ['help', 'verbose', 'xhtml=',
                                    'start=', 'count=',
                                    'article-offsets=',
                                    'just-cat',
                                    'no-output',
                                    ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    out_name = 'all_articles.html'
    off_name = 'offsets.db'
    start_article = 1
    article_count = 'all'
    do_output = True

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
        elif opt in ('-n', '--no-output'):
            do_output = False
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



    offset_db = sqlite3.connect(off_name)
    offset_db.execute('pragma synchronous = 0')
    offset_db.execute('pragma temp_store = 2')
    #offset_db.execute('pragma locking_mode = exclusive')   # Sean: Ask Chris if this is ok?!
    offset_db.execute('read_uncommitted = 1')
    offset_db.execute('pragma cache_size = 20000000')
    offset_db.execute('pragma default_cache_size = 20000000')
    #offset_db.execute('pragma journal_mode = memory')
    offset_db.execute('pragma journal_mode = off')

    offset_cursor = offset_db.cursor()

    if do_output:
        newf = subprocess.Popen(PARSER_COMMAND + ' > ' + out_name, shell=True, stdin=subprocess.PIPE).stdin
    else:
        newf = None

    # process all required articles
    current_file_id = None
    f = None
    total_articles = 0
    while article_count == 'all' or article_count != 0:
        offset_cursor.execute('select file_id, title, seek, length from offsets where article_number = ? limit 1',
                              (start_article,))
        row = offset_cursor.fetchone()
        if None == row:
            break
        (title, file_id, seek, length) = row  # this order is different from select!
        #print "row : ", row   # just to show select order is wrong, check create table and insert/import
        if file_id != current_file_id:
            current_file_id = file_id
            if f:
                f.close()
            offset_cursor.execute('select filename from files where file_id = ? limit 1', (file_id,))
            filename = offset_cursor.fetchone()[0]
            f = open(filename, 'r')
            if verbose:
                print 'Open:', filename
        f.seek(seek)
        
        if type(title) is IntType:  # Chris: Can we fix this in the database?
            title = str(title)
        
        process_article_text(title.encode('utf-8'),  f.read(length), newf)
        if article_count != 'all':
            article_count -= 1
        total_articles += 1
        start_article += 1
        if not verbose and total_articles % 1000 == 0:
            print 'Count: %d' % total_articles

    # close files
    if f:
         f.close()
    if newf:
        newf.close()
    print 'Total: %d' % total_articles


def process_article_text(title, text, newf):
    global verbose
    global subs
    #global end_article, gallery, comment, inline_ref
    #global delete_tags, line_break, entities, img, language

    if verbose:
        print "[PA] " + title

    #text = end_article.sub('', text)

    #text = gallery.sub('', text)
    #text = comment.sub('', text)
    #text = inline_ref.sub('', text)

    #text = delete_tags.sub('', text)
    
    
    #text = line_break.sub('\n', text)
    #text = language.sub('', text)
    #text = img.sub('', text)
    
    #text = entities.sub(r'&\1;', text)

    for e,r in subs:
        text = e.sub(r, text)
    
    if newf:
        newf.write(title);
        newf.write('\n__NOTOC__\n')
        newf.write(text + '\n')
        newf.write('***EOF***\n')


# run the program
if __name__ == "__main__":
    main()


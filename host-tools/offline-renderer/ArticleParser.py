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

end_article = re.compile(r'\s*(==\s*External\s+links\s*==.*)$', re.IGNORECASE + re.DOTALL)

gallery = re.compile(r'\s*(<|&lt;)gallery(>|&gt;).*?(<|&lt;)/gallery(>|&gt;)', re.IGNORECASE + re.DOTALL)

comment = re.compile(r'((<|&lt;)!--.*?--(>|&gt;)|(<|&lt;)ref.*?(<|&lt;)/ref(>|&gt;))', re.IGNORECASE + re.DOTALL)

inline_ref = re.compile(r'((<|&lt;)ref\s+name.*?/(>|&gt;))', re.IGNORECASE)

delete_tags = re.compile(r'(<|&lt;).*?(>|&gt;)', re.IGNORECASE)

line_break = re.compile(r'(<|&lt;)br\s+/(>|&gt;)', re.IGNORECASE)
entities = re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE)

#img = re.compile(r'\[\[(file|image):(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE)
img = re.compile(r'\[\[(file|image):.*?\]{2,3}', re.IGNORECASE)

language = re.compile(r'\[\[\w\w:(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE)


# redirect: <text.....#redirect.....[[title#relative link]].....
redirected_to = re.compile(r'<text\s+xml:space="preserve">\s*#redirect[^\[]*\[\[(.*?)([#|].*?)?\]\]', re.IGNORECASE)

# Filter out Wikipedia's non article namespaces
non_articles = re.compile(r'User:|Wikipedia:|File:|MediaWiki:|Template:|Help:|Category:|Portal:', re.IGNORECASE)


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
    off_name = 'offsets.pickle'
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



    f = open(off_name, 'rb')
    article_offsets = cPickle.load(f)
    all_file_names = cPickle.load(f)
    f.close()

    if do_output:
        newf = subprocess.Popen(PARSER_COMMAND + ' > ' + out_name, shell=True, stdin=subprocess.PIPE).stdin
    else:
        newf = None

    # process all required articles
    current_file_id = None
    f = None
    total_articles = 0
    while article_count == 'all' or article_count != 0:
        if start_article >= len(article_offsets):
            break
        (file_id, title, seek, length) = article_offsets[start_article]
        if file_id != current_file_id:
            current_file_id = file_id
            if f:
                f.close()
            f = open(all_file_names[file_id], 'r')
            if verbose:
                print 'Open:', all_file_names[file_id]
        f.seek(seek)
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
    global end_article, gallery, comment, inline_ref
    global delete_tags, line_break, entities, img, language

    if verbose:
        print "[PA] " + title

    text = end_article.sub('', text)

    text = gallery.sub('', text)
    text = comment.sub('', text)
    text = inline_ref.sub('', text)

    text = delete_tags.sub('', text)
    text = line_break.sub('\n', text)
    text = entities.sub(r'&\1;', text)
    text = language.sub('', text)
    text = img.sub('', text)

    if newf:
        newf.write(title);
        newf.write('\n__NOTOC__\n')
        newf.write(text + '\n')
        newf.write('***EOF***\n')


# run the program
if __name__ == "__main__":
    main()


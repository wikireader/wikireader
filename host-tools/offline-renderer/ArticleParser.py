#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Article Parser
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, traceback
import re
import subprocess
import time
import getopt
import os.path
import sqlite3
import TidyUp
import PrintLog
from types import *

verbose = False

PARSER_COMMAND = '(cd mediawiki-offline && php wr_parser_sa.php -)'


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options> {xml-file...}'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    print('       --xhtml=file            XHTML output [all_articles.html]')
    print('       --language=lang         Set language for PHP parser [en]')
    print('       --start=n               First artcle to process [1] (1k => 1000)')
    print('       --count=n               Number of artcles to process [all] (1k => 1000)')
    print('       --article-offsets=file  Article file offsets database input [offsets.db]')
    print('       --templates=file        Database for templates [templates.db]')
    print('       --parse-workdir=dir     Work directory for the PHP parser [/tmp]')
    print('       --just-cat              Replace php parser be "cat" for debugging')
    print('       --no-output             Do not run any parsing')
    exit(1)

def main():
    global verbose
    global PARSER_COMMAND
    global total_articles

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvx:s:c:o:t:l:jnw:',
                                   ['help', 'verbose', 'xhtml=',
                                    'start=', 'count=',
                                    'article-offsets=',
                                    'templates=',
                                    'language=',
                                    'just-cat',
                                    'no-output',
                                    'parser-workdir=',
                                    ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    out_name = 'all_articles.html'
    off_name = 'offsets.db'
    parser_workdir='/tmp'
    start_article = 1
    article_count = 'all'
    failed_articles = 0
    do_output = True
    template_name = 'templates.db'
    language = 'en'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-x', '--xhtml'):
            out_name = arg
        elif opt in ('-o', '--article-offsets'):
            off_name = arg
        elif opt in ('-t', '--templates'):
            template_name = arg
        elif opt in ('-t', '--language'):
            language = arg
        elif opt in ('-w', '--parser-workdir'):
            parser_workdir = arg
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
                usage('"{0:s}={1:s}" is not numeric'.format(opt, arg))
            if start_article < 1:
                usage('"{0:s}={1:s}" must be >= 1'.format(opt, arg))
        elif opt in ('-c', '--count'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            if arg != 'all':
                try:
                    article_count = int(arg)
                except ValueError:
                    usage('"{0:s}={1:s}" is not numeric'.format(opt, arg))
            if article_count <= 0:
                usage('"{0:s}={1:s}" must be > zero'.format(opt, arg))
        else:
            usage('unhandled option: ' + opt)

    if not os.path.isdir(parser_workdir):
        usage('workdir: {0:s} does not exist'.format(parser_workdir))

    # pass parameters to the PHP parser
    os.environ['WORKDIR'] = parser_workdir
    os.environ['LANGUAGE'] = language
    os.environ['TEMPLATE_DB'] = template_name

    offset_db = sqlite3.connect(off_name)
    offset_db.execute('pragma synchronous = 0')
    offset_db.execute('pragma temp_store = 2')
    offset_db.execute('pragma read_uncommitted = true')
    offset_db.execute('pragma cache_size = 20000000')
    offset_db.execute('pragma default_cache_size = 20000000')
    offset_db.execute('pragma journal_mode = off')

    offset_cursor = offset_db.cursor()

    if do_output:
        background_process = PARSER_COMMAND + ' > ' + out_name
    else:
        background_process = None

    # process all required articles
    out_base_name = os.path.basename(out_name) # for logging messages
    current_file_id = None
    input_file = None
    process_id = None
    total_articles = 0
    start_time = time.time()
    while article_count == 'all' or article_count != 0:
        offset_cursor.execute('select file_id, title, seek, length from offsets where article_number = ? limit 1',
                              (start_article,))
        row = offset_cursor.fetchone()
        if None == row:
            break
        (file_id, title, seek, length) = row

        if file_id != current_file_id:
            current_file_id = file_id
            if input_file:
                input_file.close()
            offset_cursor.execute('select filename from files where file_id = ? limit 1', (file_id,))
            filename = offset_cursor.fetchone()[0]
            input_file = open(filename, 'rb')
            if not input_file:
                PrintlogLog.message('Failed to open: {0:s}'.format(filename))
                current_file_id = None
                continue
            if verbose:
                PrintLog.message(u'Opened: {0:s}'.format(filename))

        try:
            input_file.seek(seek)
        except Exception, e:
            PrintLog.message(u'seek failed: e={0:!s:s}  seek={1:d}  f={2:s}'.format(e, seek, filename))
            sys.exit(1)

        # restart the background process if it fails to try to record all failing articles
        if None != background_process and None == process_id:
            process_id = subprocess.Popen(background_process, shell=True, stdin=subprocess.PIPE)

        try:
            process_article_text(current_file_id, total_articles + 1, title,
                                 input_file.read(length), process_id.stdin)
        except Exception, e:
            failed_articles += 1
            # extract from log by: grep '^!' log-file
            PrintLog.message(u'!Process failed, file: {0:s} article({1:d}): {2:s} because: {3!s:s}'
                             .format(filename, total_articles, title, e))
            trace = sys.exc_info()
            if None != trace:
                traceback.print_tb(trace[2])
            process_id.stdin.close()
            process_id.wait()
            process_id = None

        if article_count != 'all':
            article_count -= 1
        total_articles += 1
        start_article += 1
        if not verbose and total_articles % 1000 == 0:
            if 0 != failed_articles:
                failed_message = 'Failed: {0:d}'.format(failed_articles)
            else:
                failed_message = ''
            now_time = time.time()
            PrintLog.message(u'Parse[{0:s}]: {1:7.2f}s {2:10d}  {3:s}'
                             .format(out_base_name, now_time - start_time,
                              total_articles, failed_message))
            start_time = now_time

    # close files
    if input_file:
        input_file.close()

    # wait for background process to finish
    if process_id:
        process_id.stdin.close()
        process_id.wait()

    # output some statistics
    PrintLog.message(u'Parse[{0:s}]: Total:  {1:d}'.format(out_base_name, total_articles))

    # indicate failures
    if 0 != failed_articles:
        PrintLog.message(u'Parse[{0:s}]: Failed: {1:d}'.format(out_base_name, failed_articles))
        sys.exit(1)


def process_article_text(id, count, title, text, newf):
    global verbose

    if verbose:
        PrintLog.message(u'[PA {0:d}] {1:s}'.format(count, title))

    text = TidyUp.article(text)

    if newf:
        newf.write('{0:d}:'.format(id))
        newf.write(title[1:].encode('utf-8'))  # We pad the title to force the database to import strings
        newf.write('\n__NOTOC__\n')
        newf.write(text.encode('utf-8') + '\n')
        newf.write('***EOF***\n')


# run the program
if __name__ == "__main__":
    main()


#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Scan a file for bad words
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, re
import getopt
import os.path
import time
import FilterWords
import FileScanner
import PrintLog


# underscore and space
whitespaces = re.compile(r'([\s_]+)', re.IGNORECASE)


verbose = False
show_restricted = False


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options> xml-file...'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --restricted            Enable restricted output')
    print('       --verbose               Enable verbose output')
    exit(1)


def main():
    global verbose, show_restricted

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvr',
                                   ['help', 'verbose',
                                    'restricted'
                                    ])
    except getopt.GetoptError as err:
        usage(err)

    verbose = False
    show_restricted = False

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-r', '--restricted'):
            show_restricted = True
        elif opt in ('-h', '--help'):
            usage(None)
        else:
            usage('unhandled option: ' + opt)

    if [] == args:
        usage('missing argument(s)')

    processor = FileProcessing()

    limit = 'all'
    for f in args:
        limit = processor.process(f, limit)

    # record initial counts
    a = processor.article_count
    r = processor.redirect_count

    # record combined count and display statistics
    s = a + r

    for f in (sys.stdout,):
        f.write('Articles:         {0:10d}\n'.format(a))
        f.write('Redirects:        {0:10d}\n'.format(r))
        f.write('Sum:              {0:10d}\n'.format(s))
        f.write('Maybe Restricted: {0:10d}\n'.format(processor.restricted_count))
        f.write('UnRestricted:     {0:10d}\n'.format(processor.unrestricted_count))
        f.write('Restricted:       {0:10d}\n'.format(processor.restricted_count - processor.unrestricted_count))

    del processor


class FileProcessing(FileScanner.FileScanner):

    def __init__(self, *args, **kw):
        super(FileProcessing, self).__init__(*args, **kw)

        self.restricted_count = 0
        self.unrestricted_count = 0
        self.redirect_count = 0
        self.article_count = 0

        self.time = time.time()


    def title(self, category, key, title, seek):
        if self.KEY_ARTICLE != key:
            if verbose:
                PrintLog.message('Non-article: {0:s}:{1:s}'.format(category,title))
            return False

        return True


    def redirect(self, category, key, title, rcategory, rkey, rtitle, seek):
        self.redirect_count += 1
        if verbose:
            PrintLog.message('Redirect: {0:s}:{1:s} -> {2:s}:{3:s}'.format(category, title, rcategory, rtitle))


    def body(self, category, key, title, text, seek):
        global verbose, show_restricted

        restricted_title =  FilterWords.is_restricted(title)
        restricted_text =  FilterWords.is_restricted(text)
        restricted = restricted_title or restricted_text

        self.article_count += 1
        if restricted:
            self.restricted_count += 1

        if not verbose and self.article_count % 10000 == 0:
            start_time = time.time()
            PrintLog.message('{0:7.2f}s {1:10d}'.format(start_time - self.time, self.article_count))
            self.time = start_time

        if verbose:
            PrintLog.message('Title: {0:s}'.format(title))

        if restricted:
            if restricted_title:
                t_state = ' Title'
            else:
                t_state = ''

            if restricted_text:
                b_state = ' Text'
                (flag, contains) = FilterWords.find_restricted(text)
                if not flag:
                    self.unrestricted_count += 1
            else:
                b_state = ''
                contains = None
            if show_restricted:
                PrintLog.message('{0:10d} Restricted{1:s}{2:s}: {3:s}'
                                 .format(self.restricted_count, t_state, b_state, title))
                if None != contains:
                    PrintLog.message('        -> {0!s:s} {1:s}'.format(flag, contains))


# run the program
if __name__ == "__main__":
    main()

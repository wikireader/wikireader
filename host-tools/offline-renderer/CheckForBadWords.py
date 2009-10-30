#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Scan a file for bad words
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, re
import littleparser
import getopt
import os.path
import time
import FilterWords
import FileScanner


# Filter out Wikipedia's non article namespaces
non_articles = re.compile(r'(User|Wikipedia|File|Talk|MediaWiki|T(emplate)?|Help|Cat(egory)?|P(ortal)?)\s*:', re.IGNORECASE)

# redirect: <text.....#redirect.....[[title#relative link]].....
redirected_to = re.compile(r'#redirect[^\[]*\[\[(.*?)([#|].*?)?\]\]', re.IGNORECASE)

# underscore and space
whitespaces = re.compile(r'([\s_]+)', re.IGNORECASE)


verbose = False
show_restricted = False


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {xlm-file...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --restricted            Enable restricted output'
    print '       --verbose               Enable verbose output'
    exit(1)


def main():
    global verbose, show_restricted

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvr',
                                   ['help', 'verbose',
                                    'restricted'
                                    ])
    except getopt.GetoptError, err:
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
        f.write('Articles:         %10d\n' % a)
        f.write('Redirects:        %10d\n' % r)
        f.write('Sum:              %10d\n' % s)
        f.write('Maybe Restricted: %10d\n' % processor.restricted_count)
        f.write('UnRestricted:     %10d\n' % processor.unrestricted_count)
        f.write('Restricted:       %10d\n' % (processor.restricted_count - processor.unrestricted_count))

    del processor


class FileProcessing(FileScanner.FileScanner):

    def __init__(self, *args, **kw):
        super(FileProcessing, self).__init__(*args, **kw)

        self.restricted_count = 0
        self.unrestricted_count = 0
        self.redirect_count = 0
        self.article_count = 0
        self.translate = littleparser.LittleParser().translate

        self.time = time.time()


    def title(self, text, seek):
        global non_articles

        if non_articles.search(text):
            if verbose:
                print 'Non-article Title:', text
            return False

        return True


    def redirect(self, title, text, seek):
        global non_articles, redirected_to, whitespaces
        global verbose

        title = self.translate(title).strip(u'\u200e\u200f')

        match = redirected_to.search(text)
        if match:
            redirect_title = self.translate(match.group(1)).strip().strip(u'\u200e\u200f')
            redirect_title = whitespaces.sub(' ', redirect_title).strip().lstrip(':')
            if non_articles.search(text):
                if verbose:
                    print 'Non-article Redirect:', text
                return

            self.redirect_count += 1
            if verbose:
                print 'Redirect: %s -> %s' % (title.encode('utf-8'), redirect_title.encode('utf-8'))
        else:
            print 'Invalid Redirect: %s -> %s' % (title.encode('utf-8'), text.encode('utf-8'))


    def body(self, title, text, seek):
        global verbose, show_restricted

        title = self.translate(title).strip(u'\u200e\u200f')

        restricted_title =  FilterWords.is_restricted(title)
        restricted_text =  FilterWords.is_restricted(text)
        restricted = restricted_title or restricted_text

        self.article_count += 1
        if restricted:
            self.restricted_count += 1

        if not verbose and self.article_count % 10000 == 0:
            start_time = time.time()
            print '%7.2fs %10d' % (start_time - self.time, self.article_count)
            self.time = start_time

        if verbose:
            print 'Title:', title.encode('utf-8')

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
                print '%10d Restricted%s%s: %s' % (self.restricted_count, t_state, b_state, title.encode('utf-8'))
                if None != contains:
                    print '        ->', flag, contains


# run the program
if __name__ == "__main__":
    main()

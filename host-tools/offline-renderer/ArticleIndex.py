#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

from __future__ import with_statement
import os, sys, re
import struct
import littleparser
import getopt
import os.path
import time
import subprocess
import sqlite3
import FilterWords
import FileScanner
import codecs
import PrintLog


# this _must_ be in ascending ASCII sequence
KEYPAD_KEYS = """ !#$%&'()*+,-.0123456789=?@abcdefghijklmnopqrstuvwxyz"""

# to check if in order: uncomment and look at result
#for c in KEYPAD_KEYS:
#    print ord(c)


# redirect: <text.....#redirect.....[[title#relative link]].....
redirected_to = re.compile(r'#redirect[^\[]*\[\[(.*?)([#|].*?)?\]\]', re.IGNORECASE)

# Filter out Wikipedia's non article namespaces
# Note: the value of the key is ignored (only 'key in' test is used)
non_article_categories = {
    # en
    "t": 1,
    "cat": 1,
    "p": 1,
    "media": 1,
    "special": 1,
    "talk": 1,
    "user": 1,
    "user talk": 1,
    "wikipedia": 1,
    "wikipedia talk": 1,
    "file": 1,
    "file talk": 1,
    "mediawiki": 1,
    "mediawiki talk": 1,
#    "template": 1,
    "template talk": 1,
    "help": 1,
    "help talk": 1,
    "category": 1,
    "category talk": 1,
    "portal": 1,
    "portal talk": 1,

    # es
    "media": 1,
    "especial": 1,
    "discusión": 1,
    "usuario": 1,
    "usuario discusión": 1,
    "wikipedia": 1,
    "wikipedia discusión": 1,
    "archivo": 1,
    "archivo discusión": 1,
    "mediawiki": 1,
    "mediawiki discusión": 1,
#    "plantilla": 1,
    "plantilla discusión": 1,
    "ayuda": 1,
    "ayuda discusión": 1,
    "categoría": 1,
    "categoría discusión": 1,
    "portal": 1,
    "portal discusión": 1,
    "wikiproyecto": 1,
    "wikiproyecto discusión": 1,
    "anexo": 1,
    "anexo discusión": 1,

}

non_articles = re.compile(r'([^:]+)\s*:')

template_categories = {
    "template": 1,
    "plantilla": 1,
}


# underscore and space
whitespaces = re.compile(r'([\s_]+)', re.IGNORECASE)

# to catch loop in redirections
class CycleError(Exception):
    pass


verbose = False
enable_templates = True     # $$$ When this is false, templates are included as articles :/

bigram = {}


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {xlm-file...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --verbose               Enable verbose output'
    print '       --article-index=file    Article index database output [articles.db]'
    print '       --article-offsets=file  Article file offsets database output [offsets.db]'
    print '       --article-counts=file   File to store the counts [counts.text]'
    print '       --limit=number          Limit the number of articles processed'
    print '       --prefix=name           Device file name portion for .fnd/.pfx [pedia]'
    print '       --templates=file        Database for templates [templates.db]'
    exit(1)


def main():
    global verbose

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvi:o:c:t:l:p:',
                                   ['help', 'verbose',
                                    'article-index=',
                                    'article-offsets=',
                                    'article-counts=',
                                    'templates=',
                                    'limit=',
                                    'prefix='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    art_name = "articles.db"
    off_name = "offsets.db"
    cnt_name = "counts.text"
    fnd_name = 'pedia.fnd'
    pfx_name = 'pedia.pfx'
    template_name = 'templates.db'
    limit = 'all'

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
        elif opt in ('-t', '--templates'):
            template_name = arg
        elif opt in ('-l', '--limit'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            if arg != 'all':
                try:
                    limit = int(arg)
                except ValueError:
                    usage('%s=%s" is not numeric' % (opt, arg))
            if limit <= 0:
                usage('%s=%s" must be > zero' % (opt, arg))
        elif opt in ('-p', '--prefix'):
            fnd_name = arg + '.fnd'
            pfx_name = arg + '.pfx'
        else:
            usage('unhandled option: ' + opt)


    processor = FileProcessing(articles = art_name, offsets = off_name, templates = template_name)

    for f in args:
        limit = processor.process(f, limit)
        if limit != 'all' and limit <= 0:
            break

    # record initial counts
    a = processor.article_count
    r = processor.redirect_count

    # fix up redirects
    m = a + processor.resolve_redirects()

    # record combined count and display statistics
    s = a + r

    cf = open(cnt_name, 'w')

    for f in (sys.stdout, cf):
        f.write('Articles:   %10d\n' % a)
        f.write('Redirects:  %10d\n' % r)
        f.write('Sum:        %10d\n' % s)
        f.write('Merged:     %10d\n' % m)
        f.write('Difference: %10d\n' % (m - s))

        f.write('Restricted: %10d\n' % processor.restricted_count)

        f.write('Templates:  %10d\n' % processor.template_count)
        f.write('rTemplates: %10d\n' % processor.template_redirect_count)

    cf.close()

    output_fnd(fnd_name, processor)
    output_pfx(pfx_name)
    del processor



def generate_bigram(text):
    global bigram
    if len(text) > 2:
        try:
            if text[0].lower() in KEYPAD_KEYS and text[1].lower() in KEYPAD_KEYS:
                bigram[text[0:2]] += 1
        except KeyError:
            bigram[text[0:2]] = 1

    if len(text) > 4:
        try:
            if text[2].lower() in KEYPAD_KEYS and text[3].lower() in KEYPAD_KEYS:
                bigram[text[2:4]] += 1
        except KeyError:
            bigram[text[2:4]] = 1


class FileProcessing(FileScanner.FileScanner):

    def __init__(self, *args, **kw):
        super(FileProcessing, self).__init__(*args, **kw)

        self.article_db_name = kw['articles']
        self.article_import = self.article_db_name + '.import'

        self.offset_db_name = kw['offsets']
        self.offset_import = self.offset_db_name + '.import'
        self.file_import = self.offset_db_name + '.files'

        self.template_db_name = kw['templates']

        for filename in [self.article_db_name,
                         self.article_import,
                         self.offset_db_name,
                         self.offset_import,
                         self.template_db_name,
                         self.file_import]:
            if os.path.exists(filename):
                os.remove(filename)

        self.restricted_count = 0
        self.redirect_count = 0
        self.article_count = 0
        self.template_count = 0
        self.template_redirect_count = 0

        self.all_titles = []

        self.translate = littleparser.LittleParser().translate
        self.redirects = {}

        self.articles = {}
        self.offsets = {}
        self.is_template = False

        self.time = time.time()

        self.template_db = sqlite3.connect(self.template_db_name)
        self.template_db.execute('pragma synchronous = 0')
        self.template_db.execute('pragma temp_store = 2')
        self.template_db.execute('pragma read_uncommitted = true')
        self.template_db.execute('pragma cache_size = 20000000')
        self.template_db.execute('pragma default_cache_size = 20000000')
        self.template_db.execute('pragma journal_mode = off')
        self.template_db.execute('''
create table templates (
    title varchar primary key,
    body varchar
)
''')
        self.template_db.execute('''
create table redirects (
    title varchar primary key,
    redirect varchar
)
''')
        self.template_db.commit()
        self.template_cursor = self.template_db.cursor()


    def __del__(self):
        PrintLog.message('Flushing databases')
        self.template_db.commit()
        self.template_cursor.close()
        self.template_db.close()

        PrintLog.message('Writing: files')
        start_time = time.time()
        i = 0
        with open(self.file_import, 'w') as f:
            for filename in self.file_list:
                f.write('%d\t%s\n' % (i, filename))
                i += 1
        PrintLog.message('Time: %ds' % (time.time() - start_time))

        PrintLog.message('Writing: articles')
        start_time = time.time()
        with open(self.article_import, 'w') as f:
            for title in self.articles:
                (article_number, fnd_offset, restricted) = self.articles[title]
                f.write('~' + title.encode('utf-8'))    # force string
                f.write('\t%d\t%d\t%d\n' % (article_number, fnd_offset, restricted))
        PrintLog.message('Time: %ds' % (time.time() - start_time))

        PrintLog.message('Writing: offsets')
        start_time = time.time()
        with open(self.offset_import, 'w') as f:
            for article_number in self.offsets:
                (file_id, title, seek, length) = self.offsets[article_number]
                f.write('%d\t%d\t' % (article_number, file_id))
                f.write('~' + title.encode('utf-8'))    # force string
                f.write('\t%d\t%d\n' % (seek, length))
        PrintLog.message('Time: %ds' % (time.time() - start_time))


        PrintLog.message('Loading: articles')
        start_time = time.time()
        p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + self.article_db_name, shell=True, stdin=subprocess.PIPE)
        p.stdin.write("""
create table articles (
    title varchar primary key,
    article_number integer,
    fnd_offset integer,
    restricted varchar
);

pragma synchronous = 0;
pragma temp_store = 2;
pragma locking_mode = exclusive;
pragma cache_size = 20000000;
pragma default_cache_size = 20000000;
pragma journal_mode = memory;

.mode tab
.import %s articles
.exit
""" % self.article_import)
        p.stdin.close()
        p.wait()
        PrintLog.message('Time: %ds' % (time.time() - start_time))

        PrintLog.message('Loading: offsets and files')
        start_time = time.time()
        p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + self.offset_db_name, shell=True, stdin=subprocess.PIPE)
        p.stdin.write("""
create table offsets (
    article_number integer primary key,
    file_id integer,
    title varchar,
    seek integer,
    length integer
);

create table files (
    file_id integer primary key,
    filename varchar
);

pragma synchronous = 0;
pragma temp_store = 2;
pragma locking_mode = exclusive;
pragma cache_size = 20000000;
pragma default_cache_size = 20000000;
pragma journal_mode = memory;

.mode tab
.import %s offsets
.import %s files
.exit
""" % (self.offset_import, self.file_import))
        p.stdin.close()
        p.wait()
        PrintLog.message('Time: %ds' % (time.time() - start_time))


    def title(self, text, seek):
        global non_articles, non_article_categories
        global template_categories
        global verbose
        global enable_templates

        match = non_articles.search(text)
        if enable_templates:
            self.is_template = match and match.group(1).lower() in template_categories
            if self.is_template:
                if verbose:
                    PrintLog.message('Template Title: %s' % text)
                return True

        if match and match.group(1).lower() in non_article_categories:
            if verbose:
                PrintLog.message('Non-article Title: %s' %text)
            return False

        return True


    def redirect(self, title, text, seek):
        global non_articles, non_article_categories, redirected_to, whitespaces
        global verbose

        title = self.translate(title).strip(u'\u200e\u200f')

        match = redirected_to.search(text)
        if match:
            redirect_title = self.translate(match.group(1)).strip().strip(u'\u200e\u200f')
            redirect_title = whitespaces.sub(' ', redirect_title).strip().lstrip(':')

            if self.is_template:
                if ':' not in redirect_title:
                    PrintLog.message('Invalid template redirect: %s -> %s'
                                     % (title, redirect_title))
                    return
                t1 = title.split(':', 1)[1].lower()
                tr = redirect_title.split(':', 1)[1].lower()

                if t1 != tr:
                    self.template_cursor.execute('insert into redirects (title, redirect) values(?, ?)',
                                                 ['~%d~%s' % (self.file_id(), t1),
                                                  '~%d~%s' % (self.file_id(), tr)])

                self.template_redirect_count += 1
                return

            match = non_articles.search(text)
            if match and match.group(1).lower() in non_article_categories:
                if verbose:
                    PrintLog.message('Non-article Redirect: %s' % text)
                return

            if '' == redirect_title:
                PrintLog.message('Empty Redirect for: %s' % title)
            else:
                self.redirects[title] = redirect_title
                self.redirect_count += 1
                if verbose:
                    PrintLog.message('Redirect: %s -> %s' % (title, redirect_title))
        else:
            text = self.translate(text).strip(u'\u200e\u200f')
            PrintLog.message('Invalid Redirect: %s -> %s\n' % (title, text))


    def body(self, title, text, seek):
        global verbose

        title = self.translate(title).strip(u'\u200e\u200f')

        if self.is_template:
            t1 = title.split(':', 1)[1].lower()
            t_body = self.translate(text).strip(u'\u200e\u200f')
            self.template_cursor.execute('insert into templates (title, body) values(?, ?)',
                                         ['~%d~%s' % (self.file_id(), t1), '~' + t_body])
            self.template_count += 1
            return

        restricted = FilterWords.is_restricted(title) or FilterWords.is_restricted(text)

        self.article_count += 1

        # do closer inspection to see if realy restricted
        if restricted:
            (restricted, bad_words) = FilterWords.find_restricted(text)

        if restricted:
            self.restricted_count += 1

        if not verbose and self.article_count % 10000 == 0:
            start_time = time.time()
            PrintLog.message('%7.2fs %10d' % (start_time - self.time, self.article_count))
            self.time = start_time

        generate_bigram(title)

        if verbose:
            if restricted:
                PrintLog.message('Restricted Title: %s' % title)
                PrintLog.message('  --> %s' % bad_words)
            else:
                PrintLog.message('Title: %s' % title)

        self.offsets[self.article_count] = (self.file_id(), title, seek, len(text))

        if self.set_index(title, (self.article_count, -1, restricted)): # -1 == pfx place holder
            PrintLog.message('Duplicate Title: %s ' % title)


    def resolve_redirects(self):
        """add redirect to article_index"""
        count = 0
        for item in self.redirects:
            try:
                self.set_index(item, self.find(item))
                count += 1
            except KeyError:
                PrintLog.message('Unresolved redirect: %s -> %s' % (item, self.redirects[item]))
            except CycleError:
                PrintLog.message('Cyclic redirect: %s -> %s' % (item, self.redirects[item]))
        return count


    def set_index(self, title, data):
        """returns false if the key did not already exist"""
        if type(title) == str:
            title = unicode(title, 'utf-8')
        result = title in self.articles
        self.articles[title] = data
        return result


    def get_index(self, title):
        if type(title) == str:
            title = unicode(title, 'utf-8')
        return self.articles[title]


    def all_indices(self):
        return self.articles.keys()


    def find(self, title, level = 0):
        """get index from article title

        also handles redirects
        returns: [index, fnd]
        """
        if '' == title:
            raise CycleError('Empty title detected')
        if level > 10:
            raise CycleError('Redirect cycle: ' + title)
        try:
            title = self.redirects[title]
        except KeyError:
            title = self.redirects[title[0].swapcase() + title[1:]]

        try:
            result = self.get_index(title)
        except KeyError:
            try:
                result = self.get_index(title[0].swapcase() + title[1:])
            except:
                result = self.find(title, level + 1)
        return result


import unicodedata
def strip_accents(s):
    if type(s) == str:
        s = unicode(s, 'utf-8')
    return ''.join((c for c in unicodedata.normalize('NFD', s) if unicodedata.category(c) != 'Mn'))


def bigram_encode(title):
    global bigram

    result = ''
    title = strip_accents(title)

    while len(title) >= 2:
        if title[0].lower() in KEYPAD_KEYS:

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
        if title[0].lower() in KEYPAD_KEYS:
            result += chr(ord(title[0]))
        #else:
        #    result += '?'
    return result


def output_fnd(filename, article_index):
    """create bigram table"""
    global bigram
    global index_matrix

    PrintLog.message('Writing bigrams: %s' % filename)
    start_time = time.time()
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

    PrintLog.message('Time: %ds' % (time.time() - start_time))

    # create pfx matrix and write encoded titles

    #article_list = [strip_accents(k) for k in article_index.keys()]
    #article_list.sort(key = lambda x: strip_accents(x).lower())

    def sort_key(key):
        global KEYPAD_KEYS
        return ''.join(c for c in strip_accents(key).lower() if c in KEYPAD_KEYS)

    PrintLog.message('Sorting titles')
    start_time = time.time()

    article_list = [ (sort_key(title), title) for title in article_index.all_indices() ]
    article_list.sort()

    PrintLog.message('Time: %ds' % (time.time() - start_time))

    PrintLog.message('Writing matrix: %s ' % filename)
    start_time = time.time()

    index_matrix = {}
    index_matrix['\0\0\0'] = out_f.tell()
    for stripped_title, title in article_list:
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
        (article_number, dummy, restricted) = article_index.get_index(title)
        article_index.set_index(title, (article_number, offset, restricted))
        out_f.write(struct.pack('Ib', article_number, 0) + bigram_encode(title) + '\0')

    out_f.close()
    PrintLog.message('Time: %ds' % (time.time() - start_time))


def output_pfx(filename):
    """output the pfx matrix"""
    global index_matrix

    PrintLog.message('Writing: %s' % filename)
    start_time = time.time()
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
    PrintLog.message('Time: %ds' % (time.time() - start_time))


# run the program
if __name__ == "__main__":
    main()

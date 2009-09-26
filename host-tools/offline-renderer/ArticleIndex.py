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
import time
import subprocess
import sqlite3
import FilterWords
import FileScanner


tparser = littleparser.LittleParser() # for handling titles

# this _must_ be in ascending ASCII sequence
KEYPAD_KEYS = """ !#$%&'()*+,-.0123456789=?@abcdefghijklmnopqrstuvwxyz"""

# to check if in order: uncomment and look at result
#for c in KEYPAD_KEYS:
#    print ord(c)


# redirect: <text.....#redirect.....[[title#relative link]].....
redirected_to = re.compile(r'#redirect[^\[]*\[\[(.*?)([#|].*?)?\]\]', re.IGNORECASE)

# Filter out Wikipedia's non article namespaces
non_articles = re.compile(r'(User|Wikipedia|File|Talk|MediaWiki|T(emplate)?|Help|Cat(egory)?|P(ortal)?)\s*:', re.IGNORECASE)

# underscore and space
whitespaces = re.compile(r'([\s_]+)', re.IGNORECASE)


verbose = False
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
    exit(1)


def main():
    global verbose

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvi:o:c:l:p:',
                                   ['help', 'verbose',
                                    'article-index=',
                                    'article-offsets=',
                                    'article-counts=',
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


    processor = FileProcessing(articles = art_name, offsets = off_name)

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
        if os.path.exists(self.article_db_name):
            os.remove(self.article_db_name)
#        self.article_db = sqlite3.connect(filename)
#        self.article_db.execute("""
#create table articles (
#    title varchar primary key,
#    article_number integer,
#    fnd_offset integer,
#    restricted varchar
#)""")

        self.offset_db_name = kw['offsets']
        if os.path.exists(self.offset_db_name):
            os.remove(self.offset_db_name)
#        self.offset_db = sqlite3.connect(filename)
#        self.offset_db.execute("""
#create table offsets (
#    article_number integer primary key,
#    title varchar,
#    file_id integer,
#    seek integer,
#    length integer
#)""")
#        self.offset_db.execute("""
#create table files (
#    file_id integer primary key,
#    filename varchar
#)""")
#
#        for db in [self.article_db, self.offset_db]:
#            db.execute('pragma synchronous = 0')
#            db.execute('pragma temp_store = 2')
#            db.execute('pragma locking_mode = exclusive')
#            db.execute('pragma cache_size = 20000000')
#            db.execute('pragma default_cache_size = 20000000')
#            db.execute('pragma journal_mode = memory')

        self.restricted_count = 0
        self.redirect_count = 0
        self.article_count = 0

        self.all_titles = []

        self.translate = littleparser.LittleParser().translate
        self.redirects = {}

        self.articles = {}
        self.offsets = {}

        self.time = time.time()

    def __del__(self):
        print 'Flushing databases'

#        print 'Writing: files'
#        i = 0
#        for filename in self.file_list:
#            self.offset_db.execute('insert into files(file_id, filename) values (?, ?)', (i, filename))
#            i += 1
#
#
#        print 'Writing: articles'
#        t = time.time()
#        self.article_db.executemany('insert into articles(title, article_number, fnd_offset, restricted) '
#                                    'values (?, ?, ?, ?)',
#                                    [ ((title,) + self.articles[title]) for title in self.articles ])
#
#        print 'Time: %ds' % (time.time() - t)
#
#        print 'Writing: offsets'
#        t = time.time()
#        self.offset_db.executemany('insert into offsets(article_number, title, file_id, seek, length) '
#                                    'values (?, ?, ?, ?, ?)',
#                                    [(article_number,) + self.offsets[article_number] for article_number in self.offsets])
#        print 'Time: %ds' % (time.time() - t)

        print 'Writing: files'
        t = time.time()
        i = 0
        file_import = self.offset_db_name + '.files'
        with open(file_import, 'w') as f:
            for filename in self.file_list:
                f.write('%d\t%s\n' % (i, filename))
                i += 1
        print 'Time: %ds' % (time.time() - t)

        print 'Writing: articles'
        article_import = self.article_db_name + '.import'
        t = time.time()
        with open(article_import, 'w') as f:
            for title in self.articles:
                (article_number, fnd_offset, restricted) = self.articles[title]
                f.write(title.encode('utf-8'))
                f.write('\t%d\t%d\t%d\n' % (article_number, fnd_offset, restricted))
        print 'Time: %ds' % (time.time() - t)

        print 'Writing: offsets'
        offset_import = self.offset_db_name + '.import'
        t = time.time()
        with open(offset_import, 'w') as f:
            for article_number in self.offsets:
                (file_id, title, seek, length) = self.offsets[article_number]
                f.write('%d\t%d\t' % (article_number, file_id))
                f.write(title.encode('utf-8'))
                f.write('\t%d\t%d\n' % (seek, length))
        print 'Time: %ds' % (time.time() - t)


        print 'Loading: articles'
        t = time.time()
        p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + self.article_db_name, shell=True, stdin=subprocess.PIPE).stdin
        p.write("""
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
""" % article_import)
        p.close()
        print 'Time: %ds' % (time.time() - t)

        print 'Loading: offsets and files'
        t = time.time()
        p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + self.offset_db_name, shell=True, stdin=subprocess.PIPE).stdin
        p.write("""
create table offsets (
    article_number integer primary key,
    title varchar,
    file_id integer,
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
""" % (offset_import, file_import))
        p.close()
        print 'Time: %ds' % (time.time() - t)



#        print 'Writing: files'
#        t = time.time()
#        i = 0
#        with open('/tmp/file.sql', 'w') as f:
#            for filename in self.file_list:
#                f.write('%d\t%s\n' % (i, filename))
#                i += 1
#        print 'Time: %ds' % (time.time() - t)
#
#        print 'Writing: articles'
#        t = time.time()
#        with open('/tmp/article.sql', 'w') as f:
#            for title in self.articles:
#                (article_number, fnd_offset, restricted) = self.articles[title]
#                f.write(title.encode('utf-8'))
#                f.write('\t%d\t%d\t%d\n' % (article_number, fnd_offset, restricted))
#        print 'Time: %ds' % (time.time() - t)
#
#        print 'Writing: offsets'
#        t = time.time()
#        with open('/tmp/offset.sql', 'w') as f:
#            for article_number in self.offsets:
#                (file_id, title, seek, length) = self.offsets[article_number]
#                f.write('%d\t%d\t' % (article_number, file_id))
#                f.write(title.encode('utf-8'))
#                f.write('\t%d\t%d\n' % (seek, length))
#        print 'Time: %ds' % (time.time() - t)

#        if None != self.article_db:
#            self.article_db.commit()
#            self.article_db.close()
#            self.article_db = None
#        if None != self.offset_db:
#            self.offset_db.commit()
#            self.offset_db.close()
#            self.offset_db = None


    def title(self, text, seek):
        global non_articles

        if non_articles.search(text):
            if verbose:
                print 'Non-article Title:', text
            return False

        return True


    def redirect(self, title, text, seek):
        global non_articles, redirected_to, whitespaces
        match = redirected_to.search(text)
        if match:
            redirect_title = self.translate(match.group(1)).strip().strip(u'\u200e\u200f')
            redirect_title = whitespaces.sub(' ', redirect_title).strip()
            redirect_title.lstrip(':')
            if non_articles.search(text):
                if verbose:
                    print 'Non-article Redirect:', text
                return

            self.redirects[title] = redirect_title
            self.redirect_count += 1
            if verbose:
                print 'Redirect: %s -> %s' % (title.encode('utf-8'), redirect_title.encode('utf-8'))
        else:
            print 'Invalid Redirect: %s -> %s' % (title.encode('utf-8'), text.encode('utf-8'))


    def body(self, title, text, seek):

        title = self.translate(title).strip(u'\u200e\u200f')

        restricted = is_restricted(title) # or is_restricted(text)

        self.article_count += 1
        if restricted:
            self.restricted_count += 1

        if not verbose and self.article_count % 10000 == 0:
            t = time.time()
            print '%7.2fs %10d' % (t - self.time, self.article_count)
            self.time = t

        generate_bigram(title)

        if verbose:
            if restricted:
                print 'Restricted Title:', title.encode('utf-8')
            else:
                print 'Title:', title.encode('utf-8')

        self.offsets[self.article_count] = (self.file_id(), title, seek, len(text))

        #self.offset_db.execute('insert into offsets (article_number, file_id, title, seek, length) values (?, ?, ?, ?, ?)',
         #                      [self.article_count, self.file_id(), title, seek, len(text)])

        if self.set_index(title, (self.article_count, -1, restricted)): # -1 == pfx place holder
            print 'Duplicate Title:', title.encode('utf-8')


    def resolve_redirects(self):
        """add redirect to article_index"""
        count = 0
        for item in self.redirects:
            try:
                self.set_index(item, self.find(item))
                count += 1
            except KeyError:
                print 'Unresolved redirect:', item, '->', self.redirects[item]
        return count


    def set_index(self, title, data):
        """returns false if the key did not already exist"""
        if type(title) == str:
            title = unicode(title, 'utf-8')
        result = title in self.articles
        self.articles[title] = data
        return result

    def Xset_index(self, title, data):
        """returns false if the key did not already exist"""
        key = title.encode('utf-8')
        result = True
        #try:
        #    item = self.article_db[key]
        #except KeyError:
        #    result = False
        #self.article_db[title.encode('utf-8')] = str(data)
        #self.article_db[title.encode('utf-8')] = str(data)
        self.article_db.execute('insert or replace into articles (title, article_number, fnd_offset, restricted) '
                                'values (?, ?, ?, ?)', (title,) + data)
        result = False # ***DEBUG*** how to set this?

        return result


    def get_index(self, title):
        #c = self.article_db.cursor()
        #c.execute('select article_number, fnd_offset, restricted from articles where title = ? limit 1', [title])
        #result = c.fetchone()
        #c.close()
        if type(title) == str:
            title = unicode(title, 'utf-8')
        return self.articles[title]


    def all_indices(self):
        #c = self.article_db.cursor()
        #c.execute("select title from articles")
        #for row in c:
        #    yield row[0]
        #c.close()
        return self.articles.keys()


    def find(self, title):
        """get index from article title

        also handles redirects
        returns: [index, fnd]
        """
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
                result = self.find(title)
        return result



non_letters = re.compile('[\d\W]+')

def is_restricted(text):
    """check if text contains any restricted words"""
    global non_letters, max_score
    score = 0
    w = frozenset(non_letters.split(text))
    contains = w & FilterWords.BAD_WORDS

    return len(contains) > 0

Xmax_score = 1
def Xfilter(text):
    """check if text contains any restricted words"""
    global non_letters, max_score
    score = 0
    contains = {}
    for w in non_letters.split(text):
        word = w.lower()
        if word in FilterWords.filter_words:
            score += FilterWords.filter_words[word]
            if word in contains:
                contains[word] += 1
            else:
                contains[word] = 1

    return score < max_score


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

    print 'Writing:', filename
    t = time.time()
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
    article_list = [ i for i in article_index.all_indices() ]
    #article_list = [strip_accents(k) for k in article_index.keys()]
    article_list.sort(key = lambda x: strip_accents(x).lower())

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
        (article_number, dummy, restricted) = article_index.get_index(title)
        article_index.set_index(title, (article_number, offset, restricted))
        out_f.write(struct.pack('Ib', article_number, 0) + bigram_encode(title) + '\0')

    out_f.close()
    print 'Time: %ds' % time.time() - t


def output_pfx(filename):
    """output the pfx matrix"""
    global index_matrix

    print 'Writing:', filename
    t = time.time()
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
    print 'Time: %ds' % time.time() - t


# run the program
if __name__ == "__main__":
    main()

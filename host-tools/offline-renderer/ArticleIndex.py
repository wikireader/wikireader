#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

from __future__ import with_statement
import os
import sys
import struct
from LittleParser import LittleParser
import urllib
import getopt
import os.path
import time
import subprocess
import sqlite3
import FilterWords
import FileScanner
import TidyUp
import PrintLog
import LanguageTranslation
import SearchKey
from SegmentedFile import SegmentedFileWriter


# maximum string lengths for FND file
# when not truncating the actual title can be twice this length (+1 for the '\0')
MAXIMUM_TITLE_LENGTH =  63 # c-code is 64 including '\0'
MAXIMUM_TITLE_ACTUAL = 255 # c-code is 256 including '\0'
FND_FILE_SEGMENT_SIZE = 20 * 1024 * 1024

# to catch loop in redirections
class CycleError(Exception):
    pass


verbose = False
enable_templates = True     # $$$ When this is false, templates are included as articles :/
error_flag = False          # indicates error in indexing, but processing will still continue
                            # to find more errors

bigram = {}


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options> xml-file...'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    print('       --article-index=file    Article index database output [articles.db]')
    print('       --article-offsets=file  Article file offsets database output [offsets.db]')
    print('       --article-counts=file   File to store the counts [counts.text]')
    print('       --language=<xx>         Set language for index conversions [en]')
    print('       --limit=number          Limit the number of articles processed')
    print('       --prefix=name           Device file name portion for .fnd/.pfx [pedia]')
    print('       --ignore-templates=file File of templates to ignore(no default)')
    print('       --templates=file        Database for templates [templates.db]')
    print('       --truncate-title        Set when not using language links to save space')
    exit(1)


def main():
    global verbose
    global error_flag


    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvi:o:c:t:I:l:p:L:T',
                                   ['help', 'verbose',
                                    'article-index=',
                                    'article-offsets=',
                                    'article-counts=',
                                    'templates=',
                                    'ignore-templates=',
                                    'limit=',
                                    'prefix=',
                                    'language=',
                                    'truncate-title',
                                    ])
    except getopt.GetoptError as err:
        usage(err)

    verbose = False
    art_name = "articles.db"
    off_name = "offsets.db"
    cnt_name = "counts.text"
    fnd_name = 'pedia{0:s}.fnd'
    pfx_name = 'pedia.pfx'
    template_name = 'templates.db'
    ignore_templates_name = None
    limit = 'all'
    language = 'en'             # some languages may require special processing
    truncate_title = False      # set tru when not using language links

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
        elif opt in ('-I', '--ignore-templates'):
            ignore_templates_name = arg
            if not os.path.exists(ignore_templates_name):
                usage(u'ignore-templates file: {0:s} does not exist'.format(ignore_templates_name))
        elif opt in ('-T', '--truncate-title'):
            truncate_title = True
        elif opt in ('-l', '--limit'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            if arg != 'all':
                try:
                    limit = int(arg)
                except ValueError:
                    usage('"{0:s}={1:s}" is not numeric'.format(opt, arg))
            if limit <= 0:
                usage('"{0:s}={1:s}" must be > zero'.format(opt, arg))
        elif opt in ('-p', '--prefix'):
            fnd_name = arg + '{0:s}.fnd'
            pfx_name = arg + '.pfx'
        elif opt in ('-L', '--language'):
            language = arg
        else:
            usage('unhandled option: ' + opt)

    if [] == args:
        usage('Missing argument(s)')

    ignored_templates = {}
    if None != ignore_templates_name:
        with open(ignore_templates_name) as f:
            for l in f.readlines():
                line = unicode(l, 'utf-8').strip()
                if line.startswith('#'):
                    continue
                if '' != line:
                    ignored_templates[line] = True

    language_convert = LanguageTranslation.LanguageNormal()
    if 'ja' == language:
        language_convert = LanguageTranslation.LanguageJapanese()

    processor = FileProcessing(articles = art_name, offsets = off_name,
                               templates = template_name,
                               ignored_templates = ignored_templates,
                               language = language_convert)

    for f in args:
        if verbose:
             PrintLog.message('process: {0:s}'.format(f))
        else:
            pass
        limit = processor.process(f, limit)
        if verbose:
             PrintLog.message('process: {0:s} returned: {0:s}'.format(limit))
        else:
            pass
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
        f.write('Articles:   {0:10d}\n'.format(a))
        f.write('Redirects:  {0:10d}\n'.format(r))
        f.write('Sum:        {0:10d}\n'.format(s))
        f.write('Merged:     {0:10d}\n'.format(m))
        f.write('Difference: {0:10d}\n'.format(m - s))

        f.write('Restricted: {0:10d}\n'.format(processor.restricted_count))

        f.write('Templates:  {0:10d}\n'.format(processor.template_count))
        f.write('rTemplates: {0:10d}\n'.format(processor.template_redirect_count))

        f.write('Characters: {0:10d}\n'.format(processor.total_character_count))

    cf.close()

    output_fnd(fnd_name, processor, language_convert, truncate_title)
    output_pfx(pfx_name)
    del processor

    # return non-zero status if there have been any errors
    if error_flag:
        PrintLog.message('*** ERROR in Index build')
        PrintLog.message('***   Currently "Duplicate Title" is the only condition that causes this error')
        PrintLog.message('***   Likely "license.xml" or "terms.xml" file duplicates a title in main wiki file')
        PrintLog.message('***   Manually edit "license.xml" or "terms.xml" file to change the title')
        sys.exit(1)


def upper_case_first_char(text):
    """upper case the first character of a siring and leave the rest unchanged

    this is used for the form of templates used in wiktionary"""
    if "" == text:
        return ""
    return text[0].upper() + text[1:]


def generate_bigram(text):
    """create bigram from pairs of characters"""
    global bigram

    if len(text) > 2:
        try:
            if SearchKey.is_valid_character(text[0]) and SearchKey.is_valid_character(text[1]):
                bigram[text[0:2]] += 1
        except KeyError:
            bigram[text[0:2]] = 1

    if len(text) > 4:
        try:
            if SearchKey.is_valid_character(text[2]) and SearchKey.is_valid_character(text[3]):
                bigram[text[2:4]] += 1
        except KeyError:
            bigram[text[2:4]] = 1


class FileProcessing(FileScanner.FileScanner):

    def __init__(self, *args, **kw):
        super(FileProcessing, self).__init__(*args, **kw)

        self.language_processor = kw['language']

        self.article_db_name = kw['articles']
        self.article_import = self.article_db_name + '.import'

        self.offset_db_name = kw['offsets']
        self.offset_import = self.offset_db_name + '.import'
        self.file_import = self.offset_db_name + '.files'

        self.template_db_name = kw['templates']
        self.ignored_templates = kw['ignored_templates']

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

        self.convert = LittleParser().convert
        self.redirects = {}

        self.articles = {}
        self.offsets = {}

        self.total_character_count = 0

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
        PrintLog.message(u'Flushing databases')
        self.template_db.commit()
        self.template_cursor.close()
        self.template_db.close()

        PrintLog.message(u'Writing: files')
        start_time = time.time()
        i = 0
        with open(self.file_import, 'w') as f:
            for filename in self.file_list:
                f.write('{0:d}\t{1:s}\n'.format(i, filename))
                i += 1
        PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))

        PrintLog.message(u'Writing: articles')
        start_time = time.time()
        with open(self.article_import, 'w') as f:
            for title in self.articles:
                (article_number, fnd_offset, restricted, is_redirect) = self.articles[title]
                f.write('~' + title.encode('utf-8'))    # force string
                f.write('\t{0:d}\t{1:d}\t{2:d}\t{3:d}\n'.format(article_number, fnd_offset, restricted, is_redirect))
        PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))

        PrintLog.message(u'Writing: offsets')
        start_time = time.time()
        with open(self.offset_import, 'w') as f:
            for article_number in self.offsets:
                (file_id, title, seek, length, accumulated) = self.offsets[article_number]
                f.write('{0:d}\t{1:d}\t'.format(article_number, file_id))
                f.write('~' + title.encode('utf-8'))    # force string
                f.write('\t{0:d}\t{1:d}\t{2:d}\n'.format(seek, length, accumulated))
        PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))


        PrintLog.message(u'Loading: articles')
        start_time = time.time()
        p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + self.article_db_name, shell=True, stdin=subprocess.PIPE)
        p.stdin.write("""
create table articles (
    title varchar primary key,
    article_number integer,
    fnd_offset integer,
    restricted integer,
    is_redirect integer
);

pragma synchronous = 0;
pragma temp_store = 2;
pragma locking_mode = exclusive;
pragma cache_size = 20000000;
pragma default_cache_size = 20000000;
pragma journal_mode = memory;

.mode tabs
.import {0:s} articles
.exit
""".format(self.article_import))
        p.stdin.close()
        p.wait()
        PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))

        PrintLog.message(u'Loading: offsets and files')
        start_time = time.time()
        p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + self.offset_db_name, shell=True, stdin=subprocess.PIPE)
        p.stdin.write("""
create table offsets (
    article_number integer primary key,
    file_id integer,
    title varchar,
    seek integer,
    length integer,
    accumulated integer
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

.mode tabs
.import {0:s} offsets
.import {1:s} files
.exit
""".format(self.offset_import, self.file_import))
        p.stdin.close()
        p.wait()
        PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))


    def title(self, category, key, title, seek):
        global verbose
        global enable_templates

        if self.KEY_ARTICLE == key:
            return True

        if enable_templates and self.KEY_TEMPLATE == key:
            if verbose:
                PrintLog.message(u'Template Title: {0:s}'.format(unicode(title, 'utf-8')))
            return True

        return False


    def redirect(self, category, key, title, rcategory, rkey, rtitle, seek):
        global verbose

        title = self.convert(title).strip(u'\u200e\u200f')

        rtitle = self.convert(rtitle).strip().strip(u'\u200e\u200f')

        # redirected title may contain '%xx' items - treat as unicode sequence
        # if it fails just keep the %xx sequences intact since it must represent
        # either real %xx or some unknowable coding scheme
        try:
            rtitle = unicode(urllib.unquote(rtitle.encode('utf-8')), 'utf-8').strip().strip(u'\u200e\u200f')
        except UnicodeDecodeError:
            pass

        rtitle = SearchKey.compact_spaces(rtitle).lstrip(':').strip()

        if self.KEY_TEMPLATE == key:
            if title != rtitle:
                title = unicode(category, 'utf-8').capitalize() + ':' + upper_case_first_char(title)
                rtitle = unicode(rcategory, 'utf-8').capitalize() + ':' + upper_case_first_char(rtitle)
                self.template_cursor.execute(u'insert or replace into redirects (title, redirect) values(?, ?)',
                                             [u'~{0:d}~{1:s}'.format(self.file_id(), title),
                                              u'~{0:d}~{1:s}'.format(self.file_id(), rtitle)])

            self.template_redirect_count += 1
            return

        if self.KEY_ARTICLE != key or self.KEY_ARTICLE != rkey:
            if verbose:
                PrintLog.message(u'Non-article Redirect: {0:s}[{1:d}]:{2:s} ->  {3:s}[{4:d}]:{5:s}'
                                 .format(unicode(category, 'utf-8'), key, title,
                                         unicode(rcategory, 'utf-8'), rkey, rtitle))
            return

        if '' == rtitle:
            PrintLog.message(u'Empty Redirect for: {0:s}[{1:d}]:{2:s}'.format(category, key, title))
        else:
            self.redirects[title] = rtitle
            self.redirect_count += 1

            for t in self.language_processor.translate(title):
                generate_bigram(t)

            if verbose:
                PrintLog.message(u'Redirect: {0:s}[{1:d}]:{2:s} ->  {3:s}[{4:d}]:{5:s}'
                                 .format(category, key, title, rcategory, rkey, rtitle))


    def body(self, category, key, title, text, seek):
        global verbose
        global error_flag

        title = self.convert(title).strip(u'\u200e\u200f')

        if self.KEY_TEMPLATE == key:
            if title not in self.ignored_templates:
                title = unicode(category, 'utf-8').capitalize() + ':' + upper_case_first_char(title)
                t_body = TidyUp.template(text)
                self.template_cursor.execute(u'insert or replace into templates (title, body) values(?, ?)',
                                             [u'~{0:d}~{1:s}'.format(self.file_id(), title), u'~' + t_body])
                self.template_count += 1
            return

        restricted = FilterWords.is_restricted(title) or FilterWords.is_restricted(text)

        self.article_count += 1

        # do closer inspection to see if really restricted
        if restricted:
            (restricted, bad_words) = FilterWords.find_restricted(text)

        if restricted:
            self.restricted_count += 1

        if self.article_count % 10000 == 0:
            start_time = time.time()
            PrintLog.message(u'Index: {0:7.2f}s {1:10d}'.format(start_time - self.time, self.article_count))
            self.time = start_time

        for t in self.language_processor.translate(title):
            generate_bigram(t)

        if verbose:
            if restricted:
                PrintLog.message(u'Restricted Title: {0:s}'.format(title))
                PrintLog.message(u'  --> {0:s}'.format(bad_words))
            else:
                PrintLog.message(u'Title: {0:s}'.format(title))
                pass

        character_count = len(text)
        self.total_character_count += character_count
        self.offsets[self.article_count] = (self.file_id(), title, seek, character_count, self.total_character_count)

        if self.set_index(title, (self.article_count, -1, restricted, False)): # -1 == place holder
            PrintLog.message(u'ERROR: Duplicate Title: {0:s}'.format(title))
            error_flag = True


    def resolve_redirects(self):
        """add redirect to article_index"""
        global verbose
        count = 0
        if verbose:
            PrintLog.message(u'Resolving redirects')
        else:
            pass
        for item in self.redirects:
            try:
                self.set_index(item, self.find(item)[:3] + (True,))
                count += 1
                if verbose and count % 1000 == 0:
                    PrintLog.message(u'Redirects resolved: {0:d}'.format(count))
                else:
                    pass
            except KeyError:
                PrintLog.message(u'Unresolved redirect: {0:s} -> {1:s}'.format(item, self.redirects[item]))
            except CycleError:
                PrintLog.message(u'Cyclic redirect: {0:s} -> {1:s}'.format(item, self.redirects[item]))
        if verbose:
            PrintLog.message(u'Total redirects resolved: {0:d}'.format(count))
        else:
            pass
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


def bigram_encode(title):
    """encode a title in bigram form"""
    global bigram

    result = ''
    title = SearchKey.strip_accents(title)

    while len(title) >= 2:
        if SearchKey.is_valid_character(title[0]):

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
        if SearchKey.is_valid_character(title[0]):
            result += chr(ord(title[0]))
        #else:
        #    result += '?'

    return SearchKey.compact_spaces(result)


index_matrix = None # ensure initialised

def output_fnd(filename_format, article_index, language_processor, truncate_title):
    """create bigram table"""
    global bigram
    global index_matrix
    global MAXIMUM_TITLE_LENGTH
    global MAXIMUM_TITLE_ACTUAL
    global FND_FILE_SEGMENT_SIZE

    start_time = time.time()
    out_f = SegmentedFileWriter(filename_format, FND_FILE_SEGMENT_SIZE)
    PrintLog.message(u'Writing bigrams: {0:s}'.format(out_f.current_filename))

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

    PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))

    # create pfx matrix and write encoded titles

    PrintLog.message(u'Sorting titles')
    start_time = time.time()

    ####@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@####
    unique_articles = {}
    for article in [ (SearchKey.make_key(translated_title[:MAXIMUM_TITLE_LENGTH]), title)
                     for title in article_index.all_indices()
                     for translated_title in language_processor.translate(title) ]:
        unique_articles[article] = 1

    article_list = sorted(unique_articles.keys())

    PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))

    PrintLog.message(u'Writing matrix: {0:s}'.format(out_f.current_filename))
    start_time = time.time()

    index_matrix = {}
    index_matrix['\0\0\0'] = out_f.tell()

    previous_bigram_title = ''
    previous_utf8_title = ''
    mod_counter = 0

    for stripped_title, title in article_list:

        bigram_title = bigram_encode(stripped_title)[:MAXIMUM_TITLE_LENGTH]
        (article_number, dummy, restricted, is_redirect) = article_index.get_index(title)

        if '' == bigram_title and is_redirect:
            continue

        utf8_title = title.encode('utf-8')
        if truncate_title:
            utf8_title = utf8_title[:MAXIMUM_TITLE_LENGTH]
        else:
            utf8_title = utf8_title[:MAXIMUM_TITLE_ACTUAL]

        offset = out_f.tell()
        article_index.set_index(title, (article_number, offset, restricted, is_redirect))

        key3 = (stripped_title[0:3] + '\0\0\0')[0:3].lower()
        key2 = key3[0:2] + '\0'
        key1 = key3[0:1] + '\0\0'
        if key1 not in index_matrix:
            index_matrix[key1] = offset
        if key2 not in index_matrix:
            index_matrix[key2] = offset
        if key3 not in index_matrix:
            index_matrix[key3] = offset

        if 0 == mod_counter & 0x0f:
            bigram_common_length = 0
            utf8_common_length = 0
        else:
            bigram_common_length = common_prefix_length(previous_bigram_title, bigram_title)
            utf8_common_length = common_prefix_length(previous_utf8_title, utf8_title)
        mod_counter += 1

        previous_bigram_title = bigram_title
        previous_utf8_title = utf8_title

        if bigram_common_length > 1:
            bigram_title = chr(bigram_common_length - 1) + bigram_title[bigram_common_length:]
        if utf8_common_length > 1:
            utf8_title = chr(utf8_common_length - 1) + utf8_title[utf8_common_length:]

        out_f.write(struct.pack('<I', article_number) + '\0' + bigram_title + '\0' + utf8_title + '\0')

    PrintLog.message(u'Final segment: {0:s}'.format(out_f.current_filename))
    out_f.close()
    PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))


def common_prefix_length(s1, s2, max = 32):
    l1 = len(s1)
    l2 = len(s2)
    if 0 == l1 or 0 == l2 or s1[0] != s2[0]:
        return 0
    size = min(l1, l2, max)
    for i in range(1, size):
        if s1[i] != s2[i]:
            return i
    return size


def output_pfx(filename):
    """output the pfx matrix"""
    global index_matrix

    PrintLog.message(u'Writing: {0:s}'.format(filename))
    start_time = time.time()
    out_f = open(filename, 'wb')
    list = '\0' + SearchKey.all_characters()
    for k1 in list:
        for k2 in list:
            for k3 in list:
                key = k1+k2+k3
                if key in index_matrix:
                    offset = index_matrix[key]
                else:
                    offset = 0
                out_f.write(struct.pack('<I', offset))

    out_f.close()
    PrintLog.message(u'Time: {0:7.1f}s'.format(time.time() - start_time))


# run the program
if __name__ == "__main__":
    main()

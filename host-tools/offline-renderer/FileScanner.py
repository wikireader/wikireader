#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Read an process the XML file
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys, os
import re
import getopt
import FilterWords
import RedirectedTo


class FileScanner(object):

    def __init__(self, *args, **kw):
        #super(FileScanner, self).__init__(*args, **kw)
        super(FileScanner, self).__init__()
        self.file_list = []
        self.category_to_key = {}
        self.key_to_category = {}
        self.current_file_id = -1  # no file yet

    KEY_ARTICLE  = 0
    KEY_TEMPLATE = 10

    def file_id(self):
        return self.current_file_id


    def current_filename(self):
        return self.file_list[self.current_file_id]


    def all_file_names(self):
        return self.file_list


    def namespace(self, key, text):
        pass


    def title(self, category, key, title, seek):
        return True


    def redirect(self, category, key, title, rcategory, rkey, rtitle, seek):
        pass


    def body(self, category, key, title, text, seek):
        pass


    namespaces_start = '<namespaces>'
    namespaces_end = '</namespaces>'

    namespace_start = '<namespace key="'
    namespace_stop = '/>'
    namespace_cont = '">'

    namespace_end = '</namespace>'

    title_start = '<title>'
    title_end = '</title>'
    title_end_len = len(title_end)

    text_start = '<text '
    text_cont = 'xml:space="preserve">'
    text_stop = '/>'

    text_end = '</text>'

    # en: redirect: <text.....#redirect.....[[title#relative link]].....
    # es: redirección ""
    #redirected_to = re.compile(r'#\s*(redirect|redirecci..n)[^\[]*\[\[(.*?)([#|].*?)?\]\]', re.IGNORECASE)

    StateMachine = {
        'start': [
            (namespaces_start, len(namespaces_start),  'category_start', 'spaces'),
            (title_start, len(title_start), 'drop', 'title'),
            ],

        'spaces': [
            (namespaces_end, len(namespaces_end),  'drop', 'start'),
            (namespace_start, len(namespace_start),  'drop', 'key'),
            ],

        'key': [
            (namespace_stop, len(namespace_stop), 'drop', 'spaces'),
            (namespace_cont, len(namespace_cont), 'key', 'ns'),
            ],

        'ns': [
            (namespace_end, len(namespace_end), 'namespace', 'spaces'),
            ],

        'title': [
            (title_end, len(title_end), 'title', 'text'),
            ],

        'text': [
            (text_start, len(text_start), 'drop', 'prebody'),
            ],

        'prebody': [
            (text_cont, len(text_cont), 'drop', 'body'),
            (text_stop, len(text_stop), 'zero', 'start'),
            ],

        'body': [
            (text_end, len(text_end), 'body', 'start'),
            ]
        }

    def process(self, filename, limit):
        self.file_list += [filename]
        self.current_file_id = len(self.file_list) - 1

        block = ''
        seek = 0
        key = None
        category = None
        title = None
        file = open(filename, 'r')
        end = False
        more = True
        wanted = True
        CurrentState = self.StateMachine['start']

        run = True
        while run:
            if more or (not end and len(block) < 1024):
                more = False
                block2 = file.read(65536)
                if len(block2) == 0:
                    end = True
                else:
                    block += block2

            pos = -1
            state = None
            for s in CurrentState:
                p = block.find(s[0])
                #print 'P:"%s" %d' % (s[0], p)
                if p >= 0:
                    #print 'y:%d:"%s"' % (p, block[p:p + 10])
                    if pos < 0 or p < pos:
                        pos = p
                        state = s
            #print 'ZZZ: %d %s' % (pos, repr(state))
            #print 'block:', block

            if None == state:
                if end:
                    return limit
                else:
                    more = True
            else:
                (tag, length, proc, next) = state
                #print 'CS "%s"=%d "%s" ->"%s"' % (tag, length, proc, next)

                CurrentState = self.StateMachine[next]
                flag = False
                if 'key' == proc:
                    key = block[:pos].strip()
                elif 'namespace' == proc:
                    category =  block[:pos].strip().lower()
                    key = int(key)
                    self.category_to_key[category] = key
                    self.key_to_category[key] = category
                    self.namespace(key, category)
                    key = None
                elif 'title' == proc:
                    (category, key, title) = self.get_category(block[:pos].strip())
                    wanted = self.title(category, key, title, seek)
                elif 'body' == proc:
                    body =  block[:pos].strip()
                    flag = True
                    if '#' in body[0:10] or '＃' in body[0:10]:
                        #print 'M[%s]' % body
                        match = RedirectedTo.regex.match(body)
                        if wanted and match:
                            (rcategory, rkey, rtitle) = self.get_category(match.group(2).strip())
                            self.redirect(category, key, title, rcategory, rkey, rtitle, seek)
                            flag = False
                elif 'zero' == proc:
                    #print 'ZERO'
                    flag = True
                    body = ''
                elif 'category_start' == proc:
                    self.category_to_key = {}
                    self.key_to_category = {}

                if wanted and flag:
                    self.body(category, key, title, body, seek)
                    if limit != 'all':
                        limit -= 1
                        if limit <= 0:
                            run = False
                            break
                    title = None

                block = block[pos + length:]
                seek += pos + length

        return limit


    def get_category(self, title):
        """split title into category, key, title"""

        if ':' in title:
            (category, t) = title.split(':', 1)
            category = category.strip().lower()
            t = t.strip()
            if category in self.category_to_key:
                key = self.category_to_key[category]
                return (category, key, t)
        return ('', 0, title)



class MyTestScanner(FileScanner):

    def __init__(self, *args, **kw):
        super(MyTestScanner, self).__init__(*args, **kw)
        self.count = 0
        self.articles = 0
        self.article_index = {}


    def namespace(self, key, text):
        print 'namespace "%s"->"%s"' % (key, text)


    def title(self, category, key, title, seek):
        self.count += 1
        print 'T:%d %d : %s[%d]:%s' % (self.count, seek, category, key, title)
        if self.KEY_ARTICLE != key:
            return False
        return True


    def redirect(self, category, key, title, rcategory, rkey, rtitle, seek):
        #pass
        print 'R:%d %d : %s[%d]:%s -> %s[%d]:%s' % (self.count, seek, category, key, title,
                                                    rcategory, rkey, rtitle)


    def body(self, category, key, title, text, seek):
        if not filter(title, text):
            self.articles += 1
            self.article_index[title] = [self.articles, seek, len(text)]
            print 'B:%d %d [%s[%d]%s] : %s' % (self.count, seek, category, key, title, text[:100])


def filter(title, text):

    (restricted, contains) = FilterWords.find_restricted(title + text)

    if restricted:
        print 'TITLE: "%s" restricted: {%s}' % (title, contains)

    return restricted


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> {xml-file...}' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --count=n               Number of article to process [all]'
    print '       --limit=number          Limit the number of articles processed'
    print '       --prefix=name           Device file name portion for .fnd/.pfx [pedia]'
    print '       --templates=file        Database for templates [templates.db]'
    exit(1)


def main():
    global verbose
    global debug

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvc:d:',
                                   ['help', 'verbose',
                                    'count=',
                                    'debug='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    debug = 0
    count = 'all'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-d', '--debug'):
            try:
                debug = int(arg)
            except ValueError:
                usage('%s=%s" is not numeric' % (opt, arg))
        elif opt in ('-c', '--count'):
            if arg[-1] == 'k':
                arg = arg[:-1] + '000'
            if arg != 'all':
                try:
                    count = int(arg)
                except ValueError:
                    usage('%s=%s" is not numeric' % (opt, arg))
            if count <= 0:
                usage('%s=%s" must be > zero' % (opt, arg))
        else:
            usage('unhandled option: ' + opt)

    if len(args) == 0:
        usage('no files to process')

    scanner = MyTestScanner()

    for f in args:
        print 'Processing file: %s' % f
        count = scanner.process(f, count)
        if 0 == count:
            break


# run the program
if __name__ == "__main__":
    main()

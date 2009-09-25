#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys
import re
import FilterWords


class FileScanner(object):

    def __init__(self, *args, **kw):
        super(FileScanner, self).__init__(*args, **kw)
        self.file_list = []
        self.current_file_id = -1  # no file yet


    def file_id(self):
        return self.current_file_id


    def current_filename(self):
        return self.file_list[self.current_file_id]


    def all_file_names(self):
        return self.file_list


    def title(self, text, seek):
        return True


    def redirect(self, title, text, seek):
        pass


    def body(self, title, text, seek):
        pass

    title_start = '<title>'
    title_start_len = len(title_start)

    title_end = '</title>'
    title_end_len = len(title_end)

    text_start = '<text'
    text_start_len = len(text_start)

    text_prefix = 'xml:space="preserve">'
    text_prefix_len = len(text_prefix)

    text_end = '</text>'
    text_end_len = len(text_end)

    control = [
        (title_start, title_start_len, None),
        (title_end, title_end_len, 1),
        (text_start, text_start_len, None),
        (text_end, text_end_len, 2),
        ]

    def process(self, filename, limit):
        self.file_list += [filename]
        self.current_file_id = len(self.file_list) - 1

        block = ''
        position = 0
        title = None
        wanted = False
        file = open(filename, 'r')

        run = True
        while run:
            for tag, size, fn in self.control:
                pos = -1
                while pos < 0:
                    pos = block.find(tag)
                    if pos >= 0:
                        break
                    block2 = file.read(65536)
                    if len(block2) == 0:
                        return
                    block += block2

                if fn == 1:
                    p = 0
                    while block[p].isspace():
                        p += 1
                    title =  block[p:pos].rstrip()
                    wanted = self.title(title, position + p)
                elif fn == 2 and wanted:
                    p = block.find(self.text_prefix)
                    if p >= 0:
                        p += self.text_prefix_len
                    else:
                        p = 0
                    while block[p].isspace():
                        p += 1
                    body = block[p:pos].rstrip()
                    if '#' == body[0] and 'redirect' == body[1:9].lower():
                        self.redirect(title, body, position + p)
                    else:
                        self.body(title, body, position + p)

                    title = None
                    if limit != 'all':
                        limit -= 1
                        if limit <= 0:
                            run = False
                            break

                l = pos + size
                position += l
                block = block[l:]
        file.close()
        return limit


class foo(FileScanner):

    def __init__(self):
        self.count = 0
        self.articles = 0
        self.article_index = {}

    def title(self, text, seek):
        self.count += 1
        if ':' in text:
            return False
        #print 'T:%d %d : %s' % (self.count, seek, text)
        return True


    def redirect(self, title, text, seek):
        pass
        #print 'S:%d %d [%s] : %s' % (self.count, seek, title, text[:100])


    def body(self, title, text, seek):
        if not filter(text, title):
            self.articles += 1
            self.article_index[title] = [self.articles, seek, len(text)]
            #print 'B:%d %d [%s] : %s' % (self.count, seek, title, text[:100])



non_letters = re.compile('[\d\W]+')
max_score = 1

def filter(text, title):
    global non_letters, bad_words, max_score
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

    if score >= max_score:
        print 'TITLE: %s *** SCORE=%d *** WORDS {%s}' % (title, score, contains)

    return score < max_score

def main():
    p = foo()

    #f = open('../../xml-file-samples/filter_test.xml', 'r')
    f = open('/home/mosko/Documents/xml_files/enwiki-20090909-pages-articles.xml', 'r')
    p.process(f)
    f.close()


# run the program
if __name__ == "__main__":
    main()

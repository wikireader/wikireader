#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Word wrap a tex strings
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys
import os


class WordWrap():
    """word wrapping class"""

    def __init__(self, char_width_funtion):
        self.buffer = []
        self.char_width_funtion = char_width_funtion


    def append(self, text, face, url):
        if type(text) != unicode:
            text = unicode(text, 'utf8')
        leading_space = text[0] == ' '
        trailing_space = text[-1] == ' '
        words = text.split()
        space_len = self.char_width_funtion(' ', face)
        space = (' ', face, url, space_len, [space_len])
        if leading_space:
            try:
                if ' ' == self.buffer[-1][0]:
                    leading_space = False
            except IndexError:
                pass
        if leading_space:
            self.buffer.append(space)
        for w in words:
            word_len = [self.char_width_funtion(c, face) for c in w]
            self.buffer.append((w, face, url, sum(word_len), word_len))
            self.buffer.append(space)
        if self.buffer != [] and not trailing_space and self.buffer[-1][0] == ' ':
            del self.buffer[-1]


    def AppendImage(self, width, height, data, url):
        self.buffer.append(('@', (width, height, data), url, width, [width]))


    def dump(self):
        print 'B:', self.buffer


    def out(self):
        try:
            if ' ' == self.buffer[-1][0]:
                del self.buffer[-1]
        except IndexError:
            pass
        sys.stdout.write('"')
        for b in self.buffer:
            sys.stdout.write(b[0])
        sys.stdout.write('"\n')


    def split(self, item, width):
        # do not attempt to split a single wide character
        # this should not occur, but handle it anyway
        if len(item[0]) == 1:
            return (item, None)

        text = item[0]
        lengths = item[4]
        i = 0
        sum = 0
        for w in lengths:
           if sum + w > width:
               break
           sum += w
           i += 1
        return ((text[:i], item[1], item[2], sum, lengths[:i]),
                (text[i:], item[1], item[2], item[3] - sum, lengths[i:]))


    def wrap(self, width):
        if [] == self.buffer:
            return []

        while self.buffer[0][0] == ' ':
            del self.buffer[0]
            if self.buffer == []:
                return []

        if self.buffer[0][3] > width and len(self.buffer[0][0]) > 1:
            (r, self.buffer[0]) = self.split(self.buffer[0], width)
            return [r]

        result = [self.buffer[0]]
        w = result[0][3]
        i = 1
        for word in self.buffer[1:]:
            w += word[3]
            if w > width:
                break
            result.append(word)
            i += 1

        if i > 0:
            del self.buffer[:i]

        if result[-1][0] == ' ':
            del result[-1]

        return result


    def have(self):
        return self.buffer != []


# some testing
def main():

    def make_link(url, x0, x1):
        print '\033[1;33mLink: "%s" %d -> %d => %d\033[0m' % (url, x0, x1, (x1 - x0))

    def cwidth(c, face):
        if c == ' ':
            return 1
        return 2

    b = WordWrap(cwidth)
    default = '\033[0m'
    grey = '\033[1;30m'
    pink = '\033[1;31m'
    green = '\033[1;32m'
    yellow = '\033[1;33m'
    blue = '\033[1;34m'
    purple = '\033[1;35m'
    cyan = '\033[1;36m'
    white = '\033[1;37m'
    red = '\033[1;38m'

    colours = {
        'n': default,
        'b': green,
        'i': blue,
        'bi': purple,
        None: red,
        }
    b.append('     hello world I am some text', 'n', None)
    b.append('   this is another bit ', 'n', None)
    b.append('that is also add', 'n', 'A Link')
    b.append('ed ', 'i', None)
    b.append('in ', 'n', None)
    b.append('     1234567890abcdefghijklmnopqrstuvwxyz ', 'bi', None)
    b.append('    and another line here is: 1234567890abcdefghijklmnopqrstuvwxyz & more', 'n', None)
    b.append(' some bold text ', 'b', None)
    b.append(' and ', 'n', None)
    b.append(' micro', 'n', 'micro')
    b.append('SDHC', 'n', 'SD card')
    b.append(' a VeryVeryVery', 'n', None)
    b.append('LongLinkWithoutSpacesThatIsCutIntoSeveralLines', 'n', 'nothing')
    b.append('VeryVeryVeryNothingNothingNothing', 'n', None)

    b.append('The expression list is evaluated once; it should yield an iterable ', 'n', None)
    b.append('object. An iterator is created for the result of the ', 'n', None)
    b.append('expression_list. The suite is then executed once for each item ', 'n', None)
    b.append('provided by the iterator, in the order of ascending indices. Each ', 'n', 'sdfdsf')
    b.append('item in turn is assigned to the target list using the standard rules ', 'n', None)
    b.append('for assignments, and then the suite is executed. When the items are ', 'n', None)
    b.append('exhausted (which is immediately when the sequence is empty), the ', 'n', None)
    b.append('suite in the else clause, if present, is executed, and the loop ', 'n', None)
    b.append('and yes it is. this ', 'n', None)
    b.AppendImage(1, 7, '@@@@', None)
    b.AppendImage(101, 7, '@@@@', None)
    b.AppendImage(102, 7, '@@@@', None)
    b.append(' is an image', 'n', None)


    b.dump()
    b.out()
    print '      "         1         2         3"'
    print '      "123456789012345678901234567890"'
    while b.have():
        url = None
        x = 0
        url_x0 = 0
        l = b.wrap(30)
        t = default
        for i in l:
            if url != i[2]:
                if url != None:
                    make_link(url, url_x0, x)
                    t += default
                url = i[2]
                if url != None:
                    url_x0 = x
                    t += red
            if url == None:
                if tuple == type(i[1]):
                    (width, height, data) = i[1]
                    t += "{%d.%d:%s}" % (width, height, data)
                else:
                    t += colours[i[1]]
                    t += i[0]
            x += i[3]
        if url != None:
            make_link(url, url_x0, x)
            t += default

        print ('Wrap: "%s"' + default) % t


# run the program
if __name__ == "__main__":
    main()

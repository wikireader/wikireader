#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Converting entities to unicode
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

from HTMLParser import HTMLParser
import unicodedata
import htmlentitydefs
import re
import codecs
import sys

entities = re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE)

# remove anything inside of end tags
fix_end1 = re.compile(r'</([a-zA-Z]+)\s*/>', re.IGNORECASE)
fix_end2 = re.compile(r'</([a-zA-Z]+)\s+[^>]+>', re.IGNORECASE)

class LittleParser(HTMLParser):
    """Translate text

    handles all of these:
      &eacute;
      &#1234;
      &amp;mu;
    """
    def __init__ (self):
        HTMLParser.__init__(self)
        self.buffer = u''

    def handle_charref(self, name):
        self.buffer += unichr(int(name))

    def handle_entityref(self, name):
        self.buffer += unichr(htmlentitydefs.name2codepoint[name])

    def handle_data(self, data):
        if type(data) == unicode:
            self.buffer += data
        else:
            self.buffer += unicode(data, 'utf-8')

    def translate(self, text):
        global entities

        if type(text) != unicode:
            text = unicode(text, 'utf-8')
        self.reset()
        self.buffer = u''
        unq = entities.sub(r'&\1;', text)
        
        unq = fix_end1.sub(r'</\1>', text)
        unq = fix_end2.sub(r'</\1>', text)
        
        try:
            self.feed(unq)
            self.close()
        except KeyError:
            print 'failed on: "%s" using-> "%s"' % (repr(text), repr(unq))
            return unq

        if type(self.buffer) == unicode:
            return self.buffer
        return unicode(self.buffer, 'utf-8')


# tests
def main():
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout)
    p = LittleParser().translate
    text = 'start test: [&egrave;] [&#1234;] [&eacute;] [%20] [%ff]  [&mu;] [&amp;mu;]  [&egrave;] end:test'
    correct = u'start test: [\xe8] [\u04d2] [\xe9] [%20] [%ff]  [\u03bc] [\u03bc]  [\xe8] end:test'
    result = p(text)
    print text
    print result
    print repr(result)
    if correct == result:
        print 'PASS:'
    else:
        print 'FAIL: mismatch'


# run the program
if __name__ == "__main__":
    main()


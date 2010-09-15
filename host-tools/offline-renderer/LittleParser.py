#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
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
lessthan = re.compile(r'<')

class LittleParser(HTMLParser):
    """Convert text

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

    def convert(self, text):
        global entities

        if type(text) != unicode:
            text = unicode(text, 'utf-8')
        self.reset()
        self.buffer = u''
        unq = entities.sub(r'&\1;', text)
        unq = lessthan.sub(r'&lt;', unq)

        try:
            self.feed(unq)
            self.close()
        except KeyError:
            #print('failed on: "{0!r:s}" using-> "{1:!r:s}"'.format(text, unq))
            return unq

        if type(self.buffer) == unicode:
            return self.buffer.strip()
        return unicode(self.buffer, 'utf-8').strip()


# tests
def main():
    p = LittleParser().convert
    text = '''
start test:
  [&egrave;] [&#1234;] [&eacute;] [%20] [%ff] [&nbsp;]
  [&mu;] [&amp;mu;] [&lt;/br/&gt;] [&egrave;] [</br/>]
  [&lt;noinclude&gt;]
end:test
'''
    correct = u'''
start test:
  [\xe8] [\u04d2] [\xe9] [%20] [%ff] [\xa0]
  [\u03bc] [\u03bc] [</br/>] [\xe8] [</br/>]
  [<noinclude>]
end:test
'''
    result = p(text)
    print('Text: {0:s}'.format(text))
    print('Result: {0:s}'.format(result.encode('utf-8')))
    print('Repr: {0!r:s}'.format(result))
    if correct == result:
        print('PASS:')
    else:
        print('FAIL: mismatch')


# run the program
if __name__ == "__main__":
    main()


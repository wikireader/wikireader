#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

from HTMLParser import HTMLParser
import unicodedata
import htmlentitydefs
import urllib

class LittleParser(HTMLParser):
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
        self.reset()
        self.buffer = u''
        self.feed(urllib.unquote(text))
        return self.buffer

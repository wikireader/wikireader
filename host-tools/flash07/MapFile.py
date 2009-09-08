#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import os.path
import sys
import re

class MapFile:
    MAP_LINE_RE = re.compile(r'^\s*(0x[0-9a-f]+)\s+(\S+)\s*(#.*)?$', re.IGNORECASE)
    MAP_COMMENT_RE = re.compile(r'^\s*(#.*)?$', re.IGNORECASE)

    def __init__(self, filename, romSize = 65536):
        self.rom = []
        self.status = True
        for line in open(filename, 'rt'):
            m = MapFile.MAP_LINE_RE.match(line)
            c = MapFile.MAP_COMMENT_RE.match(line)
            if m:
                offset = eval(m.group(1))
                file = m.group(2)
                if '*ERASE' == file:
                    data = '\xff'
                else:
                    if not os.path.isfile(file):
                        file = os.path.join(os.path.dirname(filename), file)
                    if not os.path.isfile(file):
                        print 'Missing program: "%s"' % m.group(2)
                        self.status = False
                    data = open(file, 'rb').read()
                data_len = len(data)
                self.rom.append((offset, file, data_len, data))
            elif not c:
                print 'invalid map line:', line
                self.status = False

    def items(self):
      return self.rom

    def ok(self):
      return self.status

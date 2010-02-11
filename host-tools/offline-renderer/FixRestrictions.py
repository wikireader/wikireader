#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Fix the resrictions flag bit
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys, os
import struct
import os.path
import sqlite3


idx_file = 'image/pedia.idx'
art_file = 'work/articles.db'

article_db = sqlite3.connect(art_file)
article_db.execute('pragma synchronous = 0')
article_db.execute('pragma temp_store = 2')
article_db.execute('pragma read_uncommitted = true')
article_db.execute('pragma cache_size = 20000000')
article_db.execute('pragma default_cache_size = 20000000')
article_db.execute('pragma journal_mode = off')



index = open(idx_file, 'r+b')

c = article_db.cursor()
c.execute('select article_number, fnd_offset, title, restricted from articles')

r_count = 0
f_count = 0

for article_number, fnd_offset, title, restricted in c.fetchall():

    article_number = int(article_number)
    restricted = bool(int(restricted))

    offset = (article_number - 1) * 12 + 4

    index.seek(offset)
    (byte,) = struct.unpack('I', index.read(4))

    flag = (byte & 0x80000000) != 0


    if flag != restricted:
        if restricted:
            r_count += 1
            print('Restrict[{0:d}]: {1:s}'.format(r_count, title[1:].encode('utf-8')))
            byte |= 0x80000000
        else:
            f_count += 1
            print('Free[{0:d}]: {1:s}'.format(f_count, title[1:].encode('utf-8')))
            byte &= 0x7fffffff

        index.seek(offset)
        index.write(struct.pack('I', byte))

c.close()

index.close()

print('Became Restricted: {0:10d}'.format(r_count))
print('Became Free:       {0:10d}'.format(f_count))

#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, re
import os.path
import struct


in_format = 'pedia%d.idx-tmp'
out_name = 'pedia.idx'

out = open(out_name, 'wb')

article_count = 0
i = 0
data = {}
while True:
    in_name = in_format % i
    if not os.path.isfile(in_name):
        break
    print 'combining: %s' % in_name
    data[i] = open(in_name, 'rb').read()
    article_count += len(data[i]) / 12 # sizeof(struct)
    i += 1

out.write(struct.pack('L', article_count))

for j in range(i):
    out.write(data[j])

out.close()


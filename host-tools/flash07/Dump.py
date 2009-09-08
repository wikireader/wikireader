#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import sys

def hex(list):
    a = 0
    nl = False
    for b in list:
        if 0 == a & 0x0f:
            if nl:
                sys.stdout.write('\n')
            nl = True
            sys.stdout.write('%04x  ' % a)
        sys.stdout.write('%02x ' % (ord(b)))
        a = a + 1
    sys.stdout.write('\n')

def hexcmp(list1, list2):
    a = 0
    for a in range(0, len(list1)):
        if 0 == a & 0x07:
            sys.stdout.write('\n%04x  ' % a)
        b1 = ord(list1[a])
        b2 = ord(list2[a])
        if b1 == b2:
            sys.stdout.write('%02x     ' % b1)
        else:
            sys.stdout.write('%02x[%02x] ' % (b1, b2))
        a = a + 1
    sys.stdout.write('\n')

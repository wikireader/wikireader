#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>

import os, sys, re, subprocess, hashlib


# sys.argv[1] is the mediawiki syntax XML file

if len(sys.argv) > 1:
    f = open(sys.argv[1], "r")
else:
    f = sys.stdin

newf = None

line = f.readline()
flg  = False

x1 = re.compile(r'</?title>')

while line:

    if "<title>Template:" in line:

        title = x1.sub('', line).strip()
        template_hash = hashlib.md5(title[9:].lower()).hexdigest()
        newf = open("generated/templates/%s.mwt" % template_hash, "w")

    if newf != None and "<text xml:space=\"preserve\">" in line:
        x = re.compile(r'<text xml:space="preserve">')
        line = x.sub('', line)
        flg = True

    if flg and "</text>" in line:
        flg = False
        newf.close();
        newf = None

    if flg:
        newf.write(line)

    line = f.readline()

f.close()

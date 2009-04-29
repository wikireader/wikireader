#!/usr/bin/env python

"""
A better sort utility... better because:
    - It ignores the file offset/pointer
    - It prefers shorter matches
   
(not better because it is not memory efficient)
"""

import sys

lines = []

OFFSET_SIZE = 5

def text_cmp(_a,_b):
    a = _a[0].upper()
    b = _b[0].upper()

    if a == b:
        return 0
    if a < b:
        return -1
    if a > b:
        return 1

for line in sys.stdin.readlines():
    # take a line: 'Alina Kabaieva6b6318\n'
    # and split into ('Alina Kabaieva', '6b6318')
    line = line.strip()
    lines.append((
                 line[:-OFFSET_SIZE],
                 line[-OFFSET_SIZE + 1:]))

lines.sort(text_cmp)
for line in lines:
    print "%s%s" % (line[0], line[1])

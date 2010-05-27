#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Filter bad words
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import re
import string


# text must be lower case
# the number is the maximum number of occurances
# i.e. zero => word is not allowed in text
FILTER_WEIGHTS = {
    u'{{性的}}': 0,    # this template occurs in Japanese articles that are restricted
    u'pornograph': 3,
    u'x-rated': 2,
    u'dildo': 2,
    u'erotic': 3,
    u'bdsm': 1,
    u'felching': 1,
    u'pegging': 2,
    u'cum shot': 1,
    u'anilingus': 1,
    u'deep-throat': 2,
    u'fellatio': 1,
    u'adult-video': 2,
    u'adult-entertainment': 2,
    u'son-of-a-bitch': 2,
    u'dickhead': 3,
    u'fuck': 3,
    u'cunt': 3,
}

BAD_WORDS = FILTER_WEIGHTS.keys()

NON_LETTERS = re.compile('[' + string.whitespace + ']+')

def find_restricted(text):
    """check if text contains any restricted words"""

    global NON_LETTERS, BAD_WORDS

    if unicode != type(text):
        text = unicode(text, 'utf-8')

    score = 0
    contains = {}
    for word in NON_LETTERS.split(text.lower()):
        for bad in BAD_WORDS:
            if word.startswith(bad):
                if bad not in contains:
                    contains[bad] = 1
                else:
                    contains[bad] += 1
    restrict = False
    for word in contains:
        if contains[word] > FILTER_WEIGHTS[word]:
            restrict = True
            break
    return (restrict, contains)


def is_restricted(text):
    """check if text contains any restricted words"""

    global BAD_WORDS

    if unicode != type(text):
        text = unicode(text, 'utf-8')

    text = text.lower()
    size = len(text)
    for word in BAD_WORDS:
        i = 0
        while i < size:
            p = text.find(word, i)
            if 0 == p:
                return True
            elif p > 0:
                if not text[p - 1].isalpha():
                    return True
                i = p + size
            else:
                break
    return False

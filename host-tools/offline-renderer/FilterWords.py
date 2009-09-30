#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import re


# text must be lower case
# the number is the maximum number of occurances
FILTER_WEIGHTS = {
    "pornograph": 3,
    "x-rated": 2,
    "dildo": 2,
    "erotic": 3,
    "bdsm": 1,
    "felching": 1,
    "pegging": 2,
    "cum shot": 1,
    "anilingus": 1,
    "deep-throat": 2,
    "fellatio": 1,
    "adult-video": 2,
    "adult-entertainment": 2,
    "son-of-a-bitch": 2,
    "dickhead": 3,
    "fuck": 3,
    "cunt": 3,
}

BAD_WORDS = FILTER_WEIGHTS.keys()

NON_LETTERS = re.compile('[-\d\W]+')

def find_restricted(text):
    """check if text contains any restricted words"""

    global NON_LETTERS, BAD_WORDS

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


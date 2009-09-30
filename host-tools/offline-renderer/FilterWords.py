#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import re


# must be lower case
FILTER_WEIGHTS = {
    "porn": 1,
    "x rated": 1,
    "x-rated": 1,
    "dildo": 1,
    "erotic": 1,
    "bdsm": 1,
    "felching": 1,
    "pegging": 1,
    "cumshot": 1,
    "cum shot": 1,
    "cum-shot": 1,
    "anilingus": 1,
    "deep-throat": 1,
    "fellatio": 1,
    "adult-video": 1,
    "adult-entertainment": 1,
    "son-of-a-bitch": 1,
    "slut": 1,
    "dickhead": 1,
    "bitch": 1,
    "shit": 1,
    "whore": 1,
    "arsehole": 1,
    "bastard": 1,
    "fuck": 1,
    "cunt": 1,
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
    return contains


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


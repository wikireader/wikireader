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
    "porn": 1,
    "x rated": 0,
    "x-rated": 0,
    "dildo": 0,
    "erotic": 1,
    "bdsm": 0,
    "felching": 0,
    "pegging": 0,
    "cumshot": 0,
    "cum shot": 0,
    "cum-shot": 0,
    "anilingus": 0,
    "deep-throat": 0,
    "fellatio": 0,
    "adult-video": 0,
    "adult-entertainment": 0,
    "son-of-a-bitch": 0,
    "slut": 0,
    "dickhead": 1,
    "bitch": 1,
    "shit": 3,
    "whore": 0,
    "arsehole": 1,
    "bastard": 1,
    "fuck": 0,
    "cunt": 0,
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


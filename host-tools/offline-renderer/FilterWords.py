#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

FILTER_WEIGHTS = {
    "porn": 1,
    "porno": 1,
    "pornography": 1,
    "pornographic": 1,
    "x-rated": 1,
    "xxx": 1,
    "dildo": 1,
    "erotic": 1,
    "erotica": 1,
    "BDSM": 1,
    "felching": 1,
    "pegging": 1,
    "cum": 1,
    "cumshot": 1,
    "soixante-neuf": 1,
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
    "prick": 1,
    "twat": 1,
    "bastard": 1,
    "wanker": 1,
    "shag": 1,
    "fuck": 1,
    "motherfucker": 1,
    "cunt": 1,
}

BAD_WORDS = FILTER_WEIGHTS.keys()
BAD_WORDS_SET  = frozenset(BAD_WORDS)

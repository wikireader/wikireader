#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Tidy up and remove unwanted wiki markup
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys
import re


# Regular expressions for parsing the XML
subs = [
    # surely the code has one level of escapes so can just globally substitute
    (re.compile(r'&lt;', re.IGNORECASE), r'<'),
    (re.compile(r'&gt;', re.IGNORECASE), r'>'),
    (re.compile(r'&quot;', re.IGNORECASE), r'"'),
    # &amp; must be last
    (re.compile(r'&amp;', re.IGNORECASE), r'&'),

    # remove external links
    (re.compile(r'\s*(==\s*External\s+links\s*==.*)$', re.IGNORECASE + re.DOTALL), ''),

    # remove pictures
    (re.compile(r'\s*<gallery>.*?</gallery>', re.IGNORECASE + re.DOTALL), ''),

    # remove references
    (re.compile(r'<ref\s+name.*?/>', re.IGNORECASE), ''),

    # remove comments and multi-line references
    (re.compile(r'(<!--.*?-->)|(<ref.*?</ref>)', re.IGNORECASE + re.DOTALL), ''),

    # change br to newline
    (re.compile(r'<br[\s"a-zA-Z0-9=]*/?>', re.IGNORECASE), '\n'),

    # remove files and images
    #(re.compile(r'\[\[(file|image):.*$', re.IGNORECASE + re.MULTILINE), ''),

    # remove links to other languages
    (re.compile(r'\[\[\w\w:(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE), ''),

    # Wikipedia's installed Parser extension tags
    # <categorytree>, <charinsert>, <hiero>, <imagemap>, <inputbox>, <poem>,
    # <pre>, <ref>, <references>, <source>, <syntaxhighlight> and <timeline>
    # All referenced using special characters
    # Remove some of these
    (re.compile(r'\s*<timeline>.*?</timeline>', re.IGNORECASE + re.DOTALL), ''),
    (re.compile(r'\s*<imagemap>.*?</imagemap>', re.IGNORECASE + re.DOTALL), ''),
    (re.compile(r'\s*<noinclude>.*?</noinclude>', re.IGNORECASE + re.DOTALL), ''),
    (re.compile(r'<references[\s"a-zA-Z0-9=]*/?>', re.IGNORECASE), ''),

    # remove div
    (re.compile(r'<div\s+style="clear:\s+both;">\s*</div>', re.IGNORECASE), ''),

    # remove unwanted tags
    (re.compile(r'</?\s*(poem|source|pre)\s*>', re.IGNORECASE), ''),

    # convert &lt;tag&gt; to <tag>
    #(re.compile(r'(<|&lt;)(/?)\s*(math|nowiki|table|sub|sup|small|noinclude)\s*(>|&gt;)', re.IGNORECASE), r'<\2\3>'),

    # fix entities
    #(re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE), r'&\1;'),

    # fix broken lists <li/> -> </li>
    (re.compile(r'<li\s*/>', re.IGNORECASE), r'</li>'),

    # change % so php: wr_parser_sa does not convert them
    (re.compile(r'%', re.IGNORECASE), r'%25'),
]


def tidy(text):
    """Private: generic tidy up routine"""
    global subs

    # convert to unicode errors substituting bad sequences to '�'
    if unicode != type(text):
        while True:
            try:
                text = unicode(text, 'utf-8')
                break
            except UnicodeDecodeError, error:
                (_, _, start, stop, _) = error
                text = text[:start] + '\xef\xbf\xbd' + text[stop:]

    text = text.strip().strip(u'\u200e\u200f')
    for e,r in subs:
        text = e.sub(r, text)

    return text


def article(text):
    """Tidy up article text"""
    return tidy(text)


def template(text):
    """Tidy up template text"""
    return tidy(text)


def main():
    """reserved for tests"""
    pass


# run the program
if __name__ == "__main__":
    main()

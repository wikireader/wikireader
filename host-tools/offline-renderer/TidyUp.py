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
    # remove external links
    (re.compile(r'\s*(==\s*External\s+links\s*==.*)$', re.IGNORECASE + re.DOTALL), ''),

    # remove pictures
    (re.compile(r'\s*(<|&lt;)gallery(>|&gt;).*?(<|&lt;)/gallery(>|&gt;)', re.IGNORECASE + re.DOTALL), ''),

    # remove references
    (re.compile(r'((<|&lt;)ref\s+name.*?/(>|&gt;))', re.IGNORECASE), ''),

    # remove comments and multi-line references
    (re.compile(r'((<|&lt;|&amp;lt;)!--.*?--(>|&gt;|&amp;gt;)|(<|&lt;)ref.*?(<|&lt;)/ref(>|&gt;))',
                re.IGNORECASE + re.DOTALL), ''),

    # change br to newline
    (re.compile(r'(<|&lt;)br[\s"a-zA-Z0-9=]*/?(>|&gt;)', re.IGNORECASE), '\n'),

    # remove files and images
    (re.compile(r'\[\[(file|image):.*$', re.IGNORECASE + re.MULTILINE), ''),

    # remove links to ther languages
    (re.compile(r'\[\[\w\w:(\[\[[^\]\[]*\]\]|[^\]\[])*\]\]', re.IGNORECASE), ''),

    # Wikipedia's installed Parser extension tags
    # <categorytree>, <charinsert>, <hiero>, <imagemap>, <inputbox>, <poem>,
    # <pre>, <ref>, <references>, <source>, <syntaxhighlight> and <timeline>
    # All referenced using special characters
    # Ex: <timeline> --> &lt;timeline&gt;
    # For now, we're only going to remove <timeline>
    (re.compile(r'\s*(<|&lt;)timeline(>|&gt;).*?(<|&lt;)/timeline(>|&gt;)', re.IGNORECASE + re.DOTALL), ''),
    (re.compile(r'\s*(<|&lt;)imagemap(>|&gt;).*?(<|&lt;)/imagemap(>|&gt;)', re.IGNORECASE + re.DOTALL), ''),
    (re.compile(r'(<|&lt;)references[\s"a-zA-Z0-9=]*/?(>|&gt;)', re.IGNORECASE), ''),

    # remove div
    (re.compile(r'&lt;div\s+style=&quot;clear:\s+both;&quot;&gt;\s*&lt;/div&gt;', re.IGNORECASE), ''),

    # remove unwanted tags
    (re.compile(r'(<|&lt;)/?(poem|source|pre)(>|&gt;)', re.IGNORECASE), ''),

    # convert &lt;tag&gt; to <tag>
    (re.compile(r'&lt;(/?)(math|nowiki|table|sub|sup|small|noinclude)&gt;', re.IGNORECASE), r'<\1\2>'),

    # fix entities
    (re.compile(r'&amp;([a-zA-Z]{2,8});', re.IGNORECASE), r'&\1;'),

    # change % so php: wr_parser_sa does not convert them
    (re.compile(r'%', re.IGNORECASE), r'%25'),
]


def tidy(text):
    """Private: generic tidy up routine"""
    global subs

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

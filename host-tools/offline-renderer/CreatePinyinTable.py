#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Convert the SCIM pinyin file to a Python dictionary
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os
import sys
import string
import re
import getopt
import PrintLog


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options>'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    print('       --input=file            Source SCIM Pinyin table [pinyin_table.txt]')
    print('       --output=file           Output Python file [PinyinTable.py]')
    exit(1)


DIGIT_RE = re.compile(r'\d')

VOWELS = {
    u'a': u'āáăàȧ',
    u'e': u'ēéĕèė',
    u'i': u'īíĭìi',
    u'o': u'ōóŏòȯ',
    u'u': u'ūúŭùů',
    }


def make_pinyin(text):
    """convert text like an3 to ăn"""
    global DIGIT_RE
    global VOWELS
    m = DIGIT_RE.search(text)
    if m:
        text = text[:m.end(0)]
        tone = int(text[-1]) - 1
        text = text[:-1]
    else:
        tone = 4
    for i in range(0, len(text)):
        try:
            if text[i] in u'iu' and text[i+1] in u'aeiou':
                text = text[:i + 1] + VOWELS[text[i + 1]][tone] + text[i + 2:]
                break
        except IndexError:
            pass
        if text[i] in u'aeiou':
            text = text[:i] + VOWELS[text[i]][tone] + text[i + 1:]
            break
    return text


def generate_line(f, k, d):
    """output one line of Python data"""

    f.write(u'u\'{0:s}\': ['.format(k).encode('utf-8'))
    p = d.pop(0)
    f.write(u'u\'{0:s}\''.format(p).encode('utf-8'))
    for p in d:
        f.write(u',{1:s}u\'{0:s}\''.format(p,' ' * (6 - len(p))).encode('utf-8'))
    f.write(u'],{0:s}'.format(' ' * (6 - len(p))))


def generate_output(filename, items_per_line, pinyin):
    """Create a Python module"""

    with open(filename, 'wb') as f:
        f.write("""#! /usr/bin/env python
# -*- coding: utf-8 -*-
# generated file - do not modify

# this is a simple dictionary using the CJK character as the key
# the data is a list of alternative pronunciations in Pinyin
# with accented vowels to indicate the tone

""")
        i = 0
        multiples = {}
        f.write('pinyin = {')
        pre_space = '\n    '
        for k, d in pinyin.items():
            if 1 != len(d):
                multiples[k] = d
                continue
            elif 0 == i:
                f.write(pre_space)
                i = items_per_line
            generate_line(f, k, d)
            i -= 1

        for k, d in multiples.items():
            f.write(pre_space)
            generate_line(f, k, d)

        f.write('\n}\n')


def main():
    """ main processing"""

    global verbose

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvi:o:',
                                   ['help', 'verbose',
                                    'input=',
                                    'output=',
                                    ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    input_file_name = 'pinyin_table.txt'
    output_file_name = 'PinyinTable.py'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-i', '--input'):
            input_file_name = arg
        elif opt in ('-o', '--output'):
            output_file_name = arg
        else:
            usage('unhandled option: ' + opt)

    if [] != args:
        usage('Extraneous argument(s)')


    PrintLog.message(u'Reading Data File: {0:s}'.format(input_file_name))

    errors = False
    pinyin = {}

    with open(input_file_name, 'rb') as f:
        PrintLog.message(u'File Header: {0:s}'.format(f.readline().strip()))
        PrintLog.message(u'File Version: {0:s}'.format(f.readline().strip()))
        expected_lines = int(f.readline())
        line_count = 0
        char_count = 0
        for line in f:
            line_count += 1
            n = line.strip().split()
            phonetic = make_pinyin(n.pop(0))
            item_count = int(n.pop(0))
            if len(n) != item_count:
                PrintLog.message(u'Error: incorrect item count, expected: {0:d} got: {1:d}'.format(item_count, len(n)))
                errors = True
                break
            for s in n:
                cjk = unicode(s, 'utf-8')[0]
                if cjk in pinyin:
                    pinyin[cjk] += [phonetic]
                else:
                    pinyin[cjk] = [phonetic]
                char_count += 1
        if line_count == expected_lines:
            PrintLog.message(u'Counted CJK glyphs: {0:d}'.format(char_count))
            PrintLog.message(u'Expected Lines: {0:d}'.format(expected_lines))
            PrintLog.message(u'Counted Lines: {0:d}'.format(line_count))
        else:
            PrintLog.message(u'Error: linecount miosmatch: {0:d} != {1:d}'.format(expected_lines, line_count))
            errors = True

    if errors:
        PrintLog.message(u'Error: failed to read data file')
        return 1
    else:
        PrintLog.message(u'Data Read Completed Sucessfully')

    text = u'欧洲，软件＋互联网[用统一码]  歐洲，軟體及網際網路[讓統一碼] ABC 西安 先'
    expected = u'ōuzhōu，ruănjiàn＋hùliánwăng[yòngtŏngyīmă]  ōuzhōu，ruăntĭjíwăngjìwănglù[ràngtŏngyīmă] ABC xīān xiān'

    result = u''
    for c in text:
        if c in pinyin:
            result += pinyin[c][0]
        else:
            result += c

    if result == expected:
        PrintLog.message(u'Creating: {0:s}'.format(output_file_name))

        generate_output(output_file_name, 6, pinyin)

        PrintLog.message(u'Finished: {0:s}'.format(output_file_name))

    else:
        PrintLog.message(u'Error in test:')
        PrintLog.message(u'input:    {0:s}'.format(text))
        PrintLog.message(u'output:   {0:s}'.format(result))
        PrintLog.message(u'expected: {0:s}'.format(expected))
        return 2

    return 0


# run the program
if __name__ == "__main__":
    sys.exit(main())

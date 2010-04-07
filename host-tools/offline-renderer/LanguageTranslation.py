#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Converting Asian language titles to phonetic representation
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os
import sys
import string

class LanguageProcessor(object):

    def translate(self, text):
        PrintLog.message('Virual function called')
        sys.exit(1)


class LanguageNull(LanguageProcessor):
    """no-op class"""

    def translate(self, text):
        """null translation => only strip spaces"""
        return text.strip()


class Furigana(LanguageProcessor):
    """Convert Japanese to Romaji"""

    KANA_TO_ROMAN = {

        u'ア': 'a',         u'イ': 'i',         u'ウ': 'u',         u'エ': 'e',         u'オ': 'o',
        u'カ': 'ka',        u'キ': 'ki',        u'ク': 'ku',        u'ケ': 'ke',        u'コ': 'ko',
        u'ガ': 'ga',        u'ギ': 'gi',        u'グ': 'gu',        u'ゲ': 'ge',        u'ゴ': 'go',

        u'サ': 'sa',        u'シ': 'shi',       u'ス': 'su',        u'セ': 'se',        u'ソ': 'so',
        u'ザ': 'za',        u'ジ': 'ji',        u'ズ': 'zu',        u'ゼ': 'ze',        u'ゾ': 'zo',
        u'タ': 'ta',        u'チ': 'chi',       u'ツ': 'tsu',       u'テ': 'te',        u'ト': 'to',

        u'ダ': 'da',        u'ヂ': 'di',        u'ヅ': 'du',        u'デ': 'de',        u'ド': 'do',
        u'ナ': 'na',        u'ニ': 'ni',        u'ヌ': 'nu',        u'ネ': 'ne',        u'ノ': 'no',
        u'ハ': 'ha',        u'ヒ': 'hi',        u'フ': 'fu',        u'ヘ': 'he',        u'ホ': 'ho',

        u'バ': 'ba',        u'ビ': 'bi',        u'ブ': 'bu',        u'ベ': 'be',        u'ボ': 'bo',
        u'パ': 'pa',        u'ピ': 'pi',        u'プ': 'pu',        u'ペ': 'pe',        u'ポ': 'po',
        u'マ': 'ma',        u'ミ': 'mi',        u'ム': 'mu',        u'メ': 'me',        u'モ': 'mo',

        u'ヤ': 'ya',                            u'ユ': 'yu',                           u'ヨ': 'yo',
        u'ラ': 'ra',        u'リ': 'ri',        u'ル': 'ru',        u'レ': 're',        u'ロ': 'ro',
        u'ワ': 'wa',                                                                    u'ヲ': 'wo',

        u'ン': 'nn',

        u'ー': '-',

        u'ウァ': 'wha',      u'ウィ': 'whi',                               u'ウェ': 'whe',        u'ウォ': 'who',
        u'ヴァ': 'va',       u'ヴィ': 'vi',         u'ヴ':   'vu',         u'ヴェ': 've',         u'ヴォ': 'vo',
        u'チャ': 'cya',      u'チィ': 'cyi',        u'チュ': 'cyu',        u'チェ': 'cye',        u'チョ': 'cyo',

        u'ニャ': 'nya',      u'ニィ': 'nyi',        u'ニュ': 'nyu',        u'ニェ': 'nye',        u'ニョ': 'nyo',
        u'シャ': 'sya',      u'シィ': 'syi',        u'シュ': 'syu',        u'シェ': 'sye',        u'ショ': 'syo',
        u'キァ': 'kya',      u'キィ': 'kyi',        u'キュ': 'kyu',        u'キェ': 'kye',        u'キョ': 'kyo',

        u'テャ': 'tha',      u'ティ': 'thi',        u'テュ': 'thu',        u'テェ': 'the',        u'テョ': 'tho',
        u'ヒャ': 'hya',      u'ヒィ': 'hyi',        u'ヒュ': 'hyu',        u'ヒェ': 'hye',        u'ヒョ': 'hyo',
        u'ミャ': 'mya',      u'ミィ': 'myi',        u'ミュ': 'myu',        u'ミェ': 'mye',        u'ミョ': 'myo',

        u'リャ': 'rya',      u'リィ': 'ryi',        u'リュ': 'ryu',        u'リェ': 'rye',        u'リョ': 'ryo',
        u'ジャ': 'ja',       u'ジィ': 'jyi',        u'ジュ': 'ju',         u'ジェ': 'je' ,        u'ジョ': 'jo',
        u'ギャ': 'gya',      u'ギィ': 'gyi',        u'ギュ': 'gyu',        u'ギェ': 'gye',        u'ギョ': 'gyo',

        u'ビャ': 'bya',      u'ビィ': 'byi',        u'ビュ': 'byu',        u'ビェ': 'bye',        u'ビョ': 'byo',
        u'ピャ': 'pya',      u'ピィ': 'pyi',        u'ピュ': 'pyu',        u'ピェ': 'pye',        u'ピョ': 'pyo',
        u'クァ': 'kha',      u'クィ': 'khi',        u'クゥ': 'khu',        u'クェ': 'khe',        u'クォ': 'kho',

        u'グァ': 'gha',      u'グィ': 'ghi',        u'グゥ': 'ghu',        u'グェ': 'ghe',        u'グォ': 'gho',
        u'ファ': 'fa',       u'フィ': 'fi',                               u'フェ': 'fe',         u'フォ': 'fo',
        u'フャ': 'fya',                            u'フュ': 'fyu',                              u'フョ': 'fyo',

        u'デァ': 'dha',      u'ディ': 'dhi',        u'デュ': 'dhu',        u'デェ': 'dhe',        u'デョ': 'dho',
        u'ツァ': 'tsa',      u'ツィ': 'tsi',                              u'ツェ': 'tse',        u'ツォ': 'tso',
        }


    def __init__(self, *args, **kw):
        super(Furigana, self).__init__(*args, **kw)

        import MeCab         # load Japanese dictionary interface

        self.mecab = MeCab.Tagger('-Ochasen')


    def romanise(self, text):
        """private method for converting Japanese phonetics to Romaji"""

        if type(text) != unicode:
            text = unicode(text, "utf-8")

        result = ''
        i = 0
        duplicate = False
        last = len(text) - 1
        while i <= last:
            key = text[i:i + 2] # extract a pair of phonetics
            if not (i < last and key in self.KANA_TO_ROMAN):
                key = text[i]

            if key in self.KANA_TO_ROMAN:
                s = self.KANA_TO_ROMAN[key]
                i += len(key) - 1
                if duplicate:
                    s = s[0] + s
                    duplicate = False
                result += s
            elif u'ッ' == key:
                duplicate = True
            else:
                result += key
                duplicate = False
            i += 1

        return result


    def translate(self, text):
        """take Japanese string and convert to Roman letters"""

        result = ''

        for text in text.split():

            if type(text) == unicode:
                text = text.encode('utf-8')
            n = self.mecab.parseToNode(text)

            while n:

                if n.surface == '':
                    n = n.next
                    continue

                feature = unicode(n.feature,'utf-8').split(',')

                if len(feature) < 8 or feature[7] == '*':
                    r = self.romanise(n.surface)
                else:
                    r = self.romanise(feature[7])

                result += r
                n = n.next

            result += ' '

        return result.strip()

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
import os.path
import unicodedata
import PinyinTable
try:
    import MeCab
except:
    print('error: Missing python module: python-mecab')
    print('       sudo apt-get install python-mecab mecab-ipadic-utf8')
    exit(1)

if '' != sys.argv[0]:
    user_dictionary_path = os.path.dirname(sys.argv[0])
else:
    user_dictionary_path = '.'


class LanguageProcessor(object):

    EQUIVALENTS = {
        u'æ': u'ae',
        u'ƀ': u'b',
        u'ƃ': u'b',
        u'ɓ': u'b',
        u'ƈ': u'c',
        u'đ': u'd',
        u'ȡ': u'd',
        u'ð': u'eth',
        u'ħ': u'h',
        u'ĳ': u'ij',
        u'ŀ': u'l',
        u'ł': u'l',
        u'ŉ': u'n',
        u'ƞ': u'n',
        u'ŋ': u'ng',
        u'ɔ': u'o',
        u'ø': u'o',
        u'œ': u'oe',
        u'ȣ': u'ou',
        u'ß': u's',
        u'ſ': u's',
        u'ŧ': u't',
        u'þ': u'th',
        u'ȝ': u'y',
        u'ȥ': u'z',
        u'ƶ': u'z',
        u'×': u'*',
        u'÷': u'/',
        }

    def __init__(self, *args, **kw):
        """create new instance"""

        try:
            self.cjk_convert = kw['cjk_convert']
        except KeyError:
            self.cjk_convert = True

        for k, d in self.EQUIVALENTS.items():
            u = k.upper()
            if u not in self.EQUIVALENTS:
                self.EQUIVALENTS[u] = d.upper()


    def append_translations(self, result1, result2, delimiter):
        """take Japanese string and convert to Roman letters"""

        result = []
        if len(result1) == 0:
            if isinstance(result2, list):
                result = result2
            else:
                result.append(result2)
        else:
            for r1 in result1:
                if isinstance(result2, list):
                    for r2 in result2:
                        result.append(r1 + delimiter + r2)
                else:
                    result.append(r1 + delimiter + result2)

        return result


    def translate(self, text):
        """base translation using unicode tables"""
        if unicode != type(text):
            text = unicode(text, 'utf-8')
        text = text.strip()
        result = []
        for c in text:
            try:
                n = unicodedata.name(c).split() + ['None', 'None', 'None', 'None']
            except ValueError:
                n = ('Nothing', 'None', 'None', 'None')

            character_class = n[0]
            is_letter = 'LETTER' == n[1] or 'LETTER' == n[2]
            is_small = 'SMALL' == n[1] or 'SMALL' == n[2]
            is_capital = 'CAPITAL' == n[1] or 'CAPITAL' == n[2]

            if 'HANGUL' == character_class:
                result = self.append_translations(result, n[2], '')
            elif character_class in ['HIRAGANA', 'KATAKANA']:
                if self.cjk_convert and is_letter:
                    # attempt to convert Japanese phonetic when doing Chinese->Pinyin
                    if is_small:
                        result = self.append_translations(result, n[3], '')
                    else:
                        result = self.append_translations(result, n[2], '')
                else:
                    result = self.append_translations(result, c, '')
            elif self.cjk_convert and 'CJK' == character_class:
                # use all of the list of phonetics available
                p = []
                try:
                    if len(PinyinTable.pinyin[c]) > 0:
                        for pp in PinyinTable.pinyin[c]:
                            if pp not in p:
                                p.append(pp)
                except KeyError:
                    p.append(unicodedata.normalize('NFD', c))
                result = self.append_translations(result, p, '') 
            elif character_class in ['GREEK', 'COPTIC']:
                try:
                    g = n[3][0]
                    if is_small:
                        g = g.lower()
                    result = self.append_translations(result, g, '')
                except IndexError:
                    result = self.append_translations(result, c, '')
            elif 'CYRILLIC' == character_class:
                try:
                    if 'SHORT' == n[3]:
                        g = n[4]
                    else:
                        g = n[3]
                    if g in ['HARD', 'SOFT']:
                        pass
                    else:
                        if len(g) >= 2:
                            if 'E' == g[0]:
                                g = g[1:]
                            elif g[0] not in u'AEIOUY':
                                g = g[:-1]
                            if g[0] in u'G':
                                g = g[:-1]
                        if is_small:
                            g = g.lower()
                        result = self.append_translations(result, g, '')
                except IndexError:
                    result = self.append_translations(result, c, '')
            else:
                for c in unicodedata.normalize('NFD', c):
                    if c in self.EQUIVALENTS:
                        result = self.append_translations(result, self.EQUIVALENTS[c], '')
                    else:
                        result = self.append_translations(result, c, '')
        return result


class LanguageNormal(LanguageProcessor):
    """no-op class"""

    def __init__(self, *args, **kw):
        """create new instance"""
        super(LanguageNormal, self).__init__(*args, **kw)

    def translate(self, text):
        """normal translation to alphabetic"""
        return super(LanguageNormal, self).translate(text)


class LanguageJapanese(LanguageProcessor):
    """Convert Japanese to Romaji"""

    PHONETIC = {

        # Katakana

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
        u'ワ': 'wa',                                                u'ヱ': 'we',        u'ヲ': 'wo',

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

        # Hiragana

        u'あ': 'a',         u'い': 'i',         u'う': 'u',         u'え': 'e',         u'お': 'o',
        u'か': 'ka',        u'き': 'ki',        u'く': 'ku',        u'け': 'ke',        u'こ': 'ko',
        u'が': 'ga',        u'ぎ': 'gi',        u'ぐ': 'gu',        u'げ': 'ge',        u'ご': 'go',

        u'さ': 'sa',        u'し': 'shi',       u'す': 'su',        u'せ': 'se',        u'そ': 'so',
        u'ざ': 'za',        u'じ': 'ji',        u'ず': 'zu',        u'ぜ': 'ze',        u'ぞ': 'zo',
        u'た': 'ta',        u'ち': 'chi',       u'つ': 'tsu',       u'て': 'te',        u'と': 'to',

        u'だ': 'da',        u'ぢ': 'di',        u'づ': 'du',        u'で': 'de',        u'ど': 'do',
        u'な': 'na',        u'に': 'ni',        u'ぬ': 'nu',        u'ね': 'ne',        u'の': 'no',
        u'は': 'ha',        u'ひ': 'hi',        u'ふ': 'fu',        u'へ': 'he',        u'ほ': 'ho',

        u'ば': 'ba',        u'び': 'bi',        u'ぶ': 'bu',        u'べ': 'be',        u'ぼ': 'bo',
        u'ぱ': 'pa',        u'ぴ': 'pi',        u'ぷ': 'pu',        u'ぺ': 'pe',        u'ぽ': 'po',
        u'ま': 'ma',        u'み': 'mi',        u'む': 'mu',        u'め': 'me',        u'も': 'mo',

        u'や': 'ya',                            u'ゆ': 'yu',                            u'よ': 'yo',
        u'ら': 'ra',        u'り': 'ri',        u'る': 'ru',        u'れ': 're',        u'ろ': 'ro',
        u'わ': 'wa',        u'ゐ': 'wi',                            u'ゑ': 'we',        u'を': 'wo',

        u'ん': 'nn',

        u'ー': '-',

        u'うぁ': 'wha',      u'うぃ': 'whi',                               u'うぇ': 'whe',        u'うぉ': 'who',
        u'ゔぁ': 'va',       u'ゔぃ': 'vi',         u'ゔ':   'vu',         u'ゔぇ': 've',         u'ゔぉ': 'vo',
        u'チゃ': 'cya',      u'チぃ': 'cyi',        u'チゅ': 'cyu',        u'チぇ': 'cye',        u'チょ': 'cyo',

        u'にゃ': 'nya',      u'にぃ': 'nyi',        u'にゅ': 'nyu',        u'にぇ': 'nye',        u'にょ': 'nyo',
        u'しゃ': 'sya',      u'しぃ': 'syi',        u'しゅ': 'syu',        u'しぇ': 'sye',        u'しょ': 'syo',
        u'きぁ': 'kya',      u'きぃ': 'kyi',        u'きゅ': 'kyu',        u'きぇ': 'kye',        u'きょ': 'kyo',

        u'てゃ': 'tha',      u'てぃ': 'thi',        u'てゅ': 'thu',        u'てぇ': 'the',        u'てょ': 'tho',
        u'ひゃ': 'hya',      u'ひぃ': 'hyi',        u'ひゅ': 'hyu',        u'ひぇ': 'hye',        u'ひょ': 'hyo',
        u'みゃ': 'mya',      u'みぃ': 'myi',        u'みゅ': 'myu',        u'みぇ': 'mye',        u'みょ': 'myo',

        u'りゃ': 'rya',      u'りぃ': 'ryi',        u'りゅ': 'ryu',        u'りぇ': 'rye',        u'りょ': 'ryo',
        u'じゃ': 'ja',       u'じぃ': 'jyi',        u'じゅ': 'ju',         u'じぇ': 'je' ,        u'じょ': 'jo',
        u'ぎゃ': 'gya',      u'ぎぃ': 'gyi',        u'ぎゅ': 'gyu',        u'ぎぇ': 'gye',        u'ぎょ': 'gyo',

        u'びゃ': 'bya',      u'びぃ': 'byi',        u'びゅ': 'byu',        u'びぇ': 'bye',        u'びょ': 'byo',
        u'ぴゃ': 'pya',      u'ぴぃ': 'pyi',        u'ぴゅ': 'pyu',        u'ぴぇ': 'pye',        u'ぴょ': 'pyo',
        u'くぁ': 'kha',      u'くぃ': 'khi',        u'くぅ': 'khu',        u'くぇ': 'khe',        u'くぉ': 'kho',

        u'ぐぁ': 'gha',      u'ぐぃ': 'ghi',        u'ぐぅ': 'ghu',        u'ぐぇ': 'ghe',        u'ぐぉ': 'gho',
        u'ふぁ': 'fa',       u'ふぃ': 'fi',                               u'ふぇ': 'fe',         u'ふぉ': 'fo',
        u'ふゃ': 'fya',                            u'ふゅ': 'fyu',                              u'ふょ': 'fyo',

        u'でぁ': 'dha',      u'でぃ': 'dhi',        u'でゅ': 'dhu',        u'でぇ': 'dhe',        u'でょ': 'dho',
        u'つぁ': 'tsa',      u'つぃ': 'tsi',                              u'つぇ': 'tse',        u'つぉ': 'tso',

        }


    def __init__(self, *args, **kw):
        """intitialise MeCab library"""
        global user_dictionary_path

        super(LanguageJapanese, self).__init__(*args, cjk_convert=False, **kw)

        user_dictionary = os.path.join(user_dictionary_path, 'user.dic')

        assert os.path.exists(user_dictionary)
        self.mecab = MeCab.Tagger('-O chasen -u ' + user_dictionary)


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
            if not (i < last and key in self.PHONETIC):
                key = text[i]

            if key in self.PHONETIC:
                s = self.PHONETIC[key]
                i += len(key) - 1
                if duplicate:
                    s = s[0] + s
                    duplicate = False
                result += s
            elif key in u'ッっ':
                duplicate = True
            else:
                result += key
                duplicate = False
            i += 1

        return result

    def trans(self, text):
        """translate Kanji to phonetic array"""

        result = {}
        if type(text) == unicode:
            text = text.encode('utf-8')

        n = self.mecab.parseToNode(text)
        slen = n.sentence_length;
        processed_len = 0
        for i in range(slen + 1):
            b = n.begin_node_list(i)
            e = n.end_node_list(i)
            if i >= processed_len:
                max_len = 0
                while b:
                    if '' != b.surface and b.length >= max_len:
                        if i not in result:
                            result[i] = {}
                        f =  b.feature.split(',')
                        if max_len <= b.length:
                            if len(f) < 8 or f[7] == '*':
                                r = self.romanise(b.surface)
                            else:
                                r = self.romanise(f[7])
                            if max_len < b.length:
                                if max_len > 0:
                                    result[i] = {}
                                max_len = b.length
                            result[i][(b.length, b.surface, r)] = True
                    b = b.bnext
                    processed_len = i + max_len
        return result


    def process(self, table, offset, kanji, katakana):
        """resolve phonetic array to list of all combinations"""

        if offset not in table:
            return [(-1, kanji, katakana)]

        result = []

        for item in table[offset]:
            for suffix in self.process(table, offset + item[0], kanji + item[1], katakana + item[2]):
                result.append((offset + item[0], suffix[1], suffix[2]))

        return result


    def get_phonetics(self, text):
        """count unique phoneics"""
        global verbose
        s = self.trans(text)
        r = self.process(s, 0, '', '')
        t = {}

        # remove duplicates
        for item in r:
            t[item[2]] = item[1]

        # display translations
        result = []
        for key, data in t.items():
            romaji = ''
            for c in key:
                romaji = romaji + c
            result.append(romaji)
        return result


    def translate(self, text):
        """take Japanese string and convert to Roman letters"""

        result = []

        for text in super(LanguageJapanese, self).translate(text):
            for tt in text.split():
                if type(tt) == unicode:
                    tt = tt.encode('utf-8')
                phonetics = self.get_phonetics(tt)
                result = super(LanguageJapanese, self).append_translations(result, phonetics, ' ')

        return result


def test_items(strings, translate):
    for lang, text in strings:
        print(u'\n{lang:s}  in: {src:s}'.format(lang=lang, src=text).encode('utf-8'))
        count = 0
        for t in translate(text):
            count += 1
            print(u'out {cnt:d}: {dst:s}'.format(cnt=count, dst=t).encode('utf-8'))


def main():
    """perform tests"""
    texts = [
        ('da', u'farvandsovervågning, søredning, isbrydning, forureningsbekæmpelse på havet'),
        ('is', u'um rannsóknaraðferðir vísindamanna og ádeilu á Danmörku og var þá um tíma ÞÁAÐ'),
        ('de', u'Άρκτος, arktós, Arktus (‚[Großer] Bär‘, für '),
        ('cs', u'je rovnoběžka, která vede východo-západně ve směru zemské rotace [skrýt] Čtyři světové strany'),
        ('ko', u'질량이 태양과 비슷한 별들은'),
        ('ja', u'GFDLのみでライセンスされたコンテンツ（あらゆる文章、ファイルを含む）の受け入れが禁止となりました。'),
        ('ja2', u'2004年新潟県中越地震    孫正義  孫悟空  孫子   バラク・オバマ   スタぴか'),
        ('qq', u'ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿĀāĂăĄąĆćĈĉĊċČčĎďĐđĒēĔĕĖėĘęĚěĜĝĞğĠġĢģ'),
        ('q1', u'ĤĥĦħĨĩĪīĬĭĮįİıĲĳĴĵĶķĸĹĺĻļĽľĿŀŁłŃńŅņŇňŉŊŋŌōŎŏŐőŒœŔŕŖŗŘřŚśŜŝŞşŠšŢţŤťŦŧŨũŪūŬŭŮůŰűŲųŴŵŶŷŸŹźŻżŽžſƀƁƂƃƄƅƆƇƈ'),
        ('q2', u'ƉƊƋƌƍƎƏƐƑƒƓƔƕƖƗƘƙƚƛƜƝƞƟƠơƢƣƤƥƦƧƨƩƪƫƬƭƮƯưƱƲƳƴƵƶƷƸƹƺƻƼƽƾƿǀǁǂǃǄǅǆǇǈǉǊǋǌǍǎǏǐǑǒǓǔǕǖǗǘǙǚǛǜǝǞǟǠǡǢǣǤǥǦǧǨǩǪǫǬǭǮǯ'),
        ('q3', u'ǰǱǲǳǴǵǶǷǸǹǺǻǼǽǾǿȀȁȂȃȄȅȆȇȈȉȊȋȌȍȎȏȐȑȒȓȔȕȖȗȘșȚțȜȝȞȟȠȡȢȣȤȥȦȧȨȩȪȫȬȭȮȯȰȱȲȳȴȵȶȷȸȹȺȻȼȽȾȿɀɁɂɃɄɅɆɇɈɉɊɋɌɍɎɏ'),
        ('q4', u'ɐɑɒɓɔɕɖɗɘəɚɛɜɝɞɟɠɡɢɣɤɥɦɧɨɩɪɫɬɭɮɯɰɱɲɳɴɵɶɷɸɹɺɻɼɽɾɿʀʁʂʃʄʅʆʇʈʉʊʋʌʍʎʏʐʑʒʓʔʕʖʗʘʙʚʛʜʝʞʟʠʡʢʣʤʥʦʧʨʩʪʫʬʭʮʯ'),
        ('el', u'Ά·ΈΉΊ΋Ό΍ΎΏΐΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡ΢ΣΤΥΦΧΨΩΪΫάέήίΰαβγδεζηθικλμνξοπρςστυφχψωϊϋόύώϏϐϑϒϓϔϕϖϗϘϙϚϛϜϝϞϟϠϡ'),
        ('coptic', u'ϢϣϤϥϦϧϨϩϪϫϬϭϮϯϰϱϲϳϴϵ϶ϷϸϹϺϻϼϽϾϿ'),
        ('el1', u'ἀἁἂἃἄἅἆἇἈἉἊἋἌἍἎἏἐἑἒἓἔἕ἖἗ἘἙἚἛἜἝ἞἟ἠἡἢἣἤἥἦἧἨἩἪἫἬἭἮἯἰἱἲἳἴἵἶἷἸἹἺἻἼἽἾἿὀὁὂὃὄὅ὆὇ὈὉὊὋὌὍ὎὏ὐὑὒὓὔὕὖὗ὘Ὑ὚Ὓ὜Ὕ὞Ὗ'),
        ('el2', u'ὠὡὢὣὤὥὦὧὨὩὪὫὬὭὮὯὰάὲέὴήὶίὸόὺύὼώ὾὿ᾀᾁᾂᾃᾄᾅᾆᾇᾈᾉᾊᾋᾌᾍᾎᾏᾐᾑᾒᾓᾔᾕᾖᾗᾘᾙᾚᾛᾜᾝᾞᾟᾠᾡᾢᾣᾤᾥᾦᾧᾨᾩᾪᾫᾬᾭᾮᾯᾰᾱᾲᾳᾴ᾵ᾶᾷᾸᾹᾺΆᾼ᾽ι᾿῀῁'),
        ('el3', u'ῂῃῄ῅ῆῇῈΈῊΉῌ῍῎῏ῐῑῒΐ῔῕ῖῗῘῙῚΊ῜῝῞῟ῠῡῢΰῤῥῦῧῨῩῪΎῬ῭΅`῰῱ῲῳῴ῵ῶῷῸΌῺΏῼ´῾'),
        ('zh', u'欧洲，软件＋互联网[用统一码]  歐洲，軟體及網際網路[讓統一碼] ABC 西安 先'),
        ('ru', u'Является административным центром Лозовской городской совет, в который, кроме того, входят'),
        ('ru1', u'а б в г д е ё ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я'),
        ('ru2', u'А Б В Г Д Е Ё Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я'),
        ]

    print(u'\nNormal translation\n==================')
    test_items(texts, LanguageNormal().translate)

    print(u'\nJapnese translation\n====================')
    test_items(texts, LanguageJapanese().translate)


# run the program
if __name__ == "__main__":
    main()

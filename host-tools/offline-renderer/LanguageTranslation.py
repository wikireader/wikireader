#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Converting Asian language titles to phonetic representation
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os
import sys
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
        u'ー': '-',
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


    def translate(self, text, force = False):
        """base translation using unicode tables"""
        if unicode != type(text):
            text = unicode(text, 'utf-8')
        text = text.strip()
        result = []
        for c in text:
            try:
                n = unicodedata.name(c).lower().split() + ['NONE', 'NONE', 'NONE', 'NONE']
            except ValueError:
                n = ('NOTHING', 'NONE', 'NONE', 'NONE')

            character_class = n[0]
            is_letter = 'letter' == n[1] or 'letter' == n[2]
            is_small = 'small' == n[1] or 'small' == n[2]
            is_capital = 'capital' == n[1] or 'capital' == n[2]

            if 'hangul' == character_class:
                result = self.append_translations(result, n[2], '')
            elif character_class in ['hiragana', 'katakana']:
                if (force or self.cjk_convert) and is_letter:
                    # attempt to convert Japanese phonetic when doing Chinese->Pinyin
                    if is_small:
                        result = self.append_translations(result, n[3], '')
                    else:
                        result = self.append_translations(result, n[2], '')
                else:
                    result = self.append_translations(result, c, '')
            elif self.cjk_convert and 'cjk' == character_class:
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
            elif character_class in ['greek', 'coptic']:
                try:
                    g = n[3][0]
                    if not is_small or is_capital:
                        g = g.upper()
                    result = self.append_translations(result, g, '')
                except IndexError:
                    result = self.append_translations(result, c, '')
            elif 'cyrillic' == character_class:
                try:
                    if 'short' == n[3]:
                        g = n[4]
                    else:
                        g = n[3]
                    if g in ['hard', 'soft']:
                        pass
                    else:
                        if len(g) >= 2:
                            if g[0] in u'e':
                                g = g[1:]
                            elif g[0] not in u'aeiouy':
                                g = g[:-1]
                            if g[0] in u'g':
                                g = g[:-1]
                        if not is_small or is_capital:
                            g = g.upper()
                        result = self.append_translations(result, g, '')
                except IndexError:
                    result = self.append_translations(result, c, '')
            else:
                for c in unicodedata.normalize('NFD', c):
                    if c in self.EQUIVALENTS:
                        c = self.EQUIVALENTS[c]
                    if not is_small or is_capital:
                            c = c.upper()
                    result = self.append_translations(result, c, '')

        if result is None or [] == result or '' == result:
            return ['']

        return result


class LanguageNormal(LanguageProcessor):
    """no-op class"""

    def __init__(self, *args, **kw):
        """create new instance"""
        super(type(self), self).__init__(*args, **kw)

    def translate(self, text):
        """normal translation to alphabetic"""
        return super(type(self), self).translate(text)


class LanguageJapanese(LanguageProcessor):
    """Convert Japanese to Romaji"""

    def __init__(self, *args, **kw):
        """intitialise MeCab library"""
        global user_dictionary_path

        super(type(self), self).__init__(*args, cjk_convert=False, **kw)

        user_dictionary = os.path.join(user_dictionary_path, 'user.dic')

        assert os.path.exists(user_dictionary)
        self.mecab = MeCab.Tagger('-O chasen -u ' + user_dictionary)


    def romanise(self, text):
        """private method for converting Japanese phonetics to Romaji"""

        result = super(type(self), self).translate(text, True)
        return result[0]


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
                        if len(f) < 8 and f[1] == '数':
                            pass
                        elif max_len <= b.length:
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

        for text in super(type(self), self).translate(text):
            for tt in text.split():
                if type(tt) == unicode:
                    tt = tt.encode('utf-8')
                phonetics = self.get_phonetics(tt)
                result = super(type(self), self).append_translations(result, phonetics, ' ')

        if result is None or [] == result or '' == result:
            return ['']

        return result


def test_items(strings, translate):
    for lang, text in strings:
        print(u'\n{lang:s}  in: {src:s}'.format(lang=lang, src=text).encode('utf-8'))
        count = 0
        translated_list =  translate(text)
        if translated_list is None:
            raise 'translate returned None'

        if [] == translated_list:
            raise 'translate returned []'

        for t in translated_list:
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
        ('ja3', u'Ъ'),
        ('ja4', u'国際的な協力の下に規制薬物に係る不正行為を助長する行為等の防止を図るための麻薬及び向精神薬取締法等の特例等に関する法律'),
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

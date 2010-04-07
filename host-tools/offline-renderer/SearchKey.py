#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Convert string to search key
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys
import re
import unicodedata

# this _must_ be in ascending ASCII sequence
KEYPAD_KEYS = """ !#$%&'()*+,-.0123456789=?@abcdefghijklmnopqrstuvwxyz"""

# underscore and space
whitespaces = re.compile(r'([\s_]+)', re.IGNORECASE)


def make_key(text):
    """filter out only the chacters available on the keypad"""

    global whitespaces

    result = ''.join(c for c in strip_accents(text).strip().lower() if c in KEYPAD_KEYS)
    return compact_spaces(result)


def all_characters():
    """string of all allowed characters in a search key"""
    return KEYPAD_KEYS


def is_valid_character(c):
    """test if a single character is a valid search key character"""
    return c.lower() in KEYPAD_KEYS


def compact_spaces(text):
    """condense runs of spaces"""
    global whitespaces

    return whitespaces.sub(' ', text).strip()


def strip_accents(text):
    """convert all accented [a-zA-Z] to their unaccented form"""

    if type(text) == str:
        text = unicode(text, 'utf-8')

    return ''.join((c for c in unicodedata.normalize('NFD', text) if unicodedata.category(c) != 'Mn'))


def test_keypad_keys():
    """check that data stucture is correct"""

    global KEYPAD_KEYS

    error_count = 0
    previous_character = 'NONE'
    previous_ord = 0
    # to check if in order: uncomment and look at result
    for c in KEYPAD_KEYS:
        value = ord(c)
        if value <= previous_ord:
            print('error "{0!r:s}" = {1:d} <= "{2!r:s}" = {3:d}'.format(c, value, previous_character, previous_ord))
            error_count += 1
        previous_ord = value
    print('total error count = {0:d}'.format(error_count))


def test_strip_accents():
    """test strip_accents function"""

    source = u"""
      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  2x     !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
  3x  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
  4x  @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
  5x  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
  6x  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
  7x  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~

  Ax     ¡  ¢  £  ¤  ¥  ¦  §  ¨  ©  ª  «  ¬  ­  ®  ¯
  Bx  °  ±  ²  ³  ´  µ  ¶  ·  ¸  ¹  º  »  ¼  ½  ¾  ¿
  Cx  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
  Dx  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
  Ex  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
  Fx  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ

  Ax     Ą  ĸ  Ŗ  ¤  Ĩ  Ļ  §  ¨  Š  Ē  Ģ  Ŧ  ­  Ž  ¯
  Bx  °  ą  ˛  ŗ  ´  ĩ  ļ  ˇ  ¸  š  ē  ģ  ŧ  Ŋ  ž  ŋ
  Cx  Ā  Á  Â  Ã  Ä  Å  Æ  Į  Č  É  Ę  Ë  Ė  Í  Î  Ī
  Dx  Đ  Ņ  Ō  Ķ  Ô  Õ  Ö  ×  Ø  Ų  Ú  Û  Ü  Ũ  Ū  ß
  Ex  ā  á  â  ã  ä  å  æ  į  č  é  ę  ë  ė  í  î  ī
  Fx  đ  ņ  ō  ķ  ô  õ  ö  ÷  ø  ų  ú  û  ü  ũ  ū  ˙
"""

    correct = u"""
      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  2x     !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
  3x  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
  4x  @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
  5x  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
  6x  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
  7x  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~

  Ax     ¡  ¢  £  ¤  ¥  ¦  §  ¨  ©  ª  «  ¬  ­  ®  ¯
  Bx  °  ±  ²  ³  ´  µ  ¶  ·  ¸  ¹  º  »  ¼  ½  ¾  ¿
  Cx  A  A  A  A  A  A  Æ  C  E  E  E  E  I  I  I  I
  Dx  Ð  N  O  O  O  O  O  ×  Ø  U  U  U  U  Y  Þ  ß
  Ex  a  a  a  a  a  a  æ  c  e  e  e  e  i  i  i  i
  Fx  ð  n  o  o  o  o  o  ÷  ø  u  u  u  u  y  þ  y

  Ax     A  ĸ  R  ¤  I  L  §  ¨  S  E  G  Ŧ  ­  Z  ¯
  Bx  °  a  ˛  r  ´  i  l  ˇ  ¸  s  e  g  ŧ  Ŋ  z  ŋ
  Cx  A  A  A  A  A  A  Æ  I  C  E  E  E  E  I  I  I
  Dx  Đ  N  O  K  O  O  O  ×  Ø  U  U  U  U  U  U  ß
  Ex  a  a  a  a  a  a  æ  i  c  e  e  e  e  i  i  i
  Fx  đ  n  o  k  o  o  o  ÷  ø  u  u  u  u  u  u  ˙
"""

    converted = strip_accents(source)
    if correct == converted:
        print('Accents stripped sucessfully')
    else:
        print('Differences encountered')
        print('Source:')
        print(source)
        print('Converted:')
        print(converted)


def test_make_key():
    """test make_key function"""

    source = """!  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
    0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
    @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
    P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
    `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
    p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~
    """
    correct = """! # $ % & ' ( ) * + , - . 0 1 2 3 4 5 6 7 8 9 = ? @ a b c d e f g h i j k l m n o p q r s t u v w x y z a b c d e f g h i j k l m n o p q r s t u v w x y z"""

    converted = make_key(source)
    if correct == converted:
        print('Filtered sucessfully')
    else:
        print('Differences encountered')
        print('Source:')
        print(source)
        print('Converted:')
        print(converted)


def main():
    """perform tests"""
    test_keypad_keys()
    test_strip_accents()
    test_make_key()


# run the program
if __name__ == "__main__":
    main()

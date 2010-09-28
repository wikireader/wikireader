/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Holger Hans Peter Freyther <zecke@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LANGUAGES_H
#define LANGUAGES_H

#define JAMO_INDEX_EXCLUDE_BASE 0x0000FFFF
#define JAMO_INDEX_BASE 0x00010000
#define INITIAL_JAMO_BASE 0x00010000
#define MEDIAL_JAMO_BASE 0x00020000
#define FINAL_JAMO_BASE 0x00040000
#define INITIAL_JAMO_COUNT 19
#define MEDIAL_JAMO_COUNT 21
#define FINAL_JAMO_COUNT 28

enum
{
    STATE_INITIAL,
    STATE_AFTER_INITIAL_JAMO,
    STATE_AFTER_MEDIAL_JAMO,
};

int zh_jp_to_english(char *sEnglish, int maxLenEnglish, char *sHiragana, int *lenHiragana);
char *get_hiragana(char *in_str, int len, int *used_len);
char *get_english(char *in_str, int len, int *used_len);
void hiragana_romaji_conversion(char *search_string_per_language, int *search_str_per_language_len);
int replace_japanese_sonant(char *search_string_per_language, int *search_str_per_language_len, char *search_string, int *search_str_len);
int replace_hiragana_backward(char *search_string_per_language, int *search_str_per_language_len, char *search_string, int *search_str_len);
void alphabet_to_korean_jamo(char *jamo, char eng);
int english_to_korean(char *out_str, int max_out_len, char *in_str, int *in_len);
int english_to_korean_phonetic(char *out_str, int max_out_len, char *in_str, int *in_len);

#endif

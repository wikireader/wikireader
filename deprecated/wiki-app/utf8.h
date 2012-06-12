/*
 * Copyright (c) 2009 Openmoko Inc.
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

#ifndef UTF8_H
#define UTF8_H

#ifndef ucs4_t
#define ucs4_t unsigned int
#endif
ucs4_t UTF8_to_UCS4(unsigned char **pUTF8);
void UCS4_to_UTF8(ucs4_t u, unsigned char *sUTF8);
void get_last_utf8_char(char *out_utf8_char, char *utf8_str, int utf8_str_len);
void get_first_utf8_char(char *out_utf8_char, char *utf8_str, int utf8_str_len);
char *next_utf8_char(char *utf8_str);
void utf8_char_toupper(unsigned char *out, unsigned char *in);
unsigned char *full_alphabet_to_half(unsigned char *full, int *used_len);
unsigned char *half_alphabet_to_full(unsigned char c);
#endif

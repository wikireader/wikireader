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

#ifndef BIGRAM_H
#define BIGRAM_H

#define SUPPORTED_SEARCH_CHARS " !#$%&'()*+,-.0123456789abcdefghijklmnopqrstuvwxyz=?@"
// SERCH_CHR_COUNT needs to be strlen(SUPPORTED_SEARCH_CHARS) + 1
#define SEARCH_CHR_COUNT 54
#define SIZE_BIGRAM_BUF (128 * 2)
int bigram_char_idx(char c);
void bigram_encode(char *outStr, char *inStr);
void bigram_decode(char *outStr, char *inStr, int lenMax);
#ifdef WIKIPCF
void init_bigram(FILE *fd);
#else
void init_bigram(int fd);
#endif
void init_char_idx();
int is_supported_search_char(char c);
int search_string_cmp(char *title, char *search, int len);

#endif

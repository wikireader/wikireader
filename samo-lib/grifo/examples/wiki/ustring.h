/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#if !defined(_USTRING_H_)
#define _USTRING_H_ 1

#include <string.h>

#define ustrlen(s) ((int)strlen((const char *)(s)))
#define ustrcpy(d, s) (strcpy((char *)(d), (const char *)(s)))
#define ustrncpy(d, s, n) (strncpy((char *)(d), (const char *)(s), (n)))
#define ustrcmp(d, s) (strcmp((const char *)(d), (const char *)(s)))
#define ustrncmp(d, s, n) (strncmp((const char *)(d), (const char *)(s), (n)))
#define ustrchr(s, c) ((unsigned char *)strchr((const char *)(s), (c)))

#endif

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

#include "ustring.h"
#include "utf8.h"

ucs4_t UTF8_to_UCS4(const unsigned char **pUTF8)
{
	ucs4_t c0, c1, c2, c3;

	/* if 0 returned, it is not a invalid UTF8 character.  The pointer moves to the second byte. */
	c0 = 0;
	if (**pUTF8)
	{
		c0 = (ucs4_t)**pUTF8;
		(*pUTF8)++;
		if (c0 & 0x80) /* multi-byte UTF8 char */
		{
			if ((c0 & 0xE0) == 0xC0) /* 2-byte UTF8 */
			{
				c1 = **pUTF8;
				if ((c1 & 0xC0) == 0x80)
				{
					(*pUTF8)++;
					c0 = ((c0 & 0x1F) << 6) + (c1 & 0x3F);
				}
				else
					c0 = 0; /* invalid UTF8 character */
			}
			else if ((c0 & 0xF0) == 0xE0) /* 3-byte UTF8 */
			{
				c1 = **pUTF8;
				c2 = *(*pUTF8 + 1);
				if ((c1 & 0xC0) == 0x80 && (c2 & 0xC0) == 0x80)
				{
					(*pUTF8) += 2;
					c0 = ((c0 & 0x0F) << 12) + ((c1 & 0x3F) << 6) + (c2 & 0x3F);
				}
				else
					c0 = 0; /* invalid UTF8 character */
			}
			else if ((c0 & 0xF1) == 0xF0) /* 4-byte UTF8 */
			{
				c1 = **pUTF8;
				c2 = *(*pUTF8 + 1);
				c3 = *(*pUTF8 + 2);
				if ((c1 & 0xC0) == 0x80 && (c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80)
				{
					(*pUTF8) += 3;
					c0 = ((c0 & 0x07) << 18) + ((c1 & 0x3F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F) ;
				}
				else
					c0 = 0; /* invalid UTF8 character */
			}
			else
				c0 = 0; /* invalid UTF8 character */
		}
	}
	return c0;
}

void UCS4_to_UTF8(ucs4_t u, unsigned char *sUTF8)
{
	if (u < 0x80)
	{
		sUTF8[0] = (unsigned char)u;
		sUTF8[1] = '\0';
	}
	else if (u < 0x800)
	{
		sUTF8[0] = (unsigned char)(0xC0 | (u >> 6));
		sUTF8[1] = (unsigned char)(0x80 | (u & 0x3F));
		sUTF8[2] = '\0';
	}
	else if (u < 0x10000)
	{
		sUTF8[0] = (unsigned char)(0xE0 | (u >> 12));
		sUTF8[1] = (unsigned char)(0x80 | ((u & 0xFFF) >> 6));
		sUTF8[2] = (unsigned char)(0x80 | (u & 0x3F));
		sUTF8[3] = '\0';
	}
	else if (u < 0x110000)
	{
		sUTF8[0] = (unsigned char)(0xF0 | (u >> 18));
		sUTF8[1] = (unsigned char)(0x80 | ((u & 0x3FFFF) >> 12));
		sUTF8[2] = (unsigned char)(0x80 | ((u & 0xFFF) >> 6));
		sUTF8[3] = (unsigned char)(0x80 | (u & 0x3F));
		sUTF8[4] = '\0';
	}
	else
	{
		sUTF8[0] = '\0';
	}
}

void get_last_utf8_char(unsigned char *out_utf8_char, const unsigned char *utf8_str, int utf8_str_len)
{
	int i;
	int j = 0;

	if (utf8_str_len > 0)
	{
		i = utf8_str_len - 1;
		while (i >= 0 && (utf8_str[i] & 0xC0) == 0x80)
			i--;

		while (i < utf8_str_len && j < 4)
			out_utf8_char[j++] = utf8_str[i++];
	}

	out_utf8_char[j] = '\0';
}

void get_first_utf8_char(unsigned char *out_utf8_char, const unsigned char *utf8_str, int utf8_str_len)
{
	int len;
	int i = 0;

	if (utf8_str_len > 0)
	{
		if ((utf8_str[0] & 0xE0) == 0xC0) /* 2-byte UTF8 */
		{
			len = 2;
		}
		else if ((utf8_str[0] & 0xF0) == 0xE0) /* 3-byte UTF8 */
		{
			len = 3;
		}
		else if ((utf8_str[0] & 0xF1) == 0xF0) /* 4-byte UTF8 */
		{
			len = 4;
		}
		else
			len = 1;

		for (i = 0; i < len && i < utf8_str_len; i++)
			out_utf8_char[i] = utf8_str[i];
	}

	out_utf8_char[i] = '\0';
}

const unsigned char *next_utf8_char(const unsigned char *utf8_str)
{
	int len;

	if ((utf8_str[0] & 0xE0) == 0xC0) /* 2-byte UTF8 */
	{
		len = 2;
	}
	else if ((utf8_str[0] & 0xF0) == 0xE0) /* 3-byte UTF8 */
	{
		len = 3;
	}
	else if ((utf8_str[0] & 0xF1) == 0xF0) /* 4-byte UTF8 */
	{
		len = 4;
	}
	else
		len = 1;

	while (len && *utf8_str)
	{
		len--;
		utf8_str++;
	}
	return utf8_str;
}

void utf8_char_toupper(unsigned char *out, const unsigned char *in)
{
	if ('a' <= *in && *in <= 'z')
	{
		out[0] = in[0] + ('A' - 'a');
		out[1] = '\0';
	}
	else if (!ustrncmp(in, "æ", 2))
	{
		ustrcpy(out, "Æ");
	}
	else if (!ustrncmp(in, "å", 2))
	{
		ustrcpy(out, "Å");
	}
	else if (!ustrncmp(in, "ø", 2))
	{
		ustrcpy(out, "Ø");
	}
	else
		ustrcpy(out, in);
}

unsigned char *full_alphabet_to_half(const unsigned char *full, int *used_len)
{
	static unsigned char half[5];

	memset(half, 0, sizeof(half));
	if (full[0] == 0xEF && full[1] == 0xBD && 0x81 <= full[2] && full[2] <= 0x9A)
	{
		if (used_len)
			*used_len = 3;
		half[0] = 'a' + (full[2] - 0x81);
	}
	else if (full[0] == 0xEF && full[1] == 0xBC && full[2] == 0x8D) // full width -
	{
		if (used_len)
			*used_len = 3;
		half[0] = '-';
	}
	else if (full[0] == 0xEF && full[1] == 0xBC && full[2] == 0x8C) // full width ,
	{
		if (used_len)
			*used_len = 3;
		half[0] = ',';
	}
	else if (full[0] == 0xE2 && full[1] == 0x80 && full[2] == 0xA7) // full width .
	{
		if (used_len)
			*used_len = 3;
		half[0] = '.';
	}
	else
	{
		unsigned char first_utf8_char[5];
		int len_first_char;

		get_first_utf8_char(first_utf8_char, full, ustrlen(full));
		len_first_char = ustrlen(first_utf8_char);
		if (used_len)
			*used_len = len_first_char;
		memcpy(half, first_utf8_char, len_first_char);
	}
	return half;
}

unsigned char *half_alphabet_to_full(unsigned char c)
{
	static unsigned char full[5];
	if ('a' <= c && c <= 'z')
	{
		full[0] = 0xEF;
		full[1] = 0xBD;
		full[2] = 0x81 + (c - 'a');
		full[3] = '\0';
		return full;
	}
	else if (c == '-')
	{
		full[0] = 0xEF;
		full[1] = 0xBC;
		full[2] = 0x8D;
		full[3] = '\0';
		return full;
	}
	else if (c == ',')
	{
		full[0] = 0xEF;
		full[1] = 0xBC;
		full[2] = 0x8C;
		full[3] = '\0';
		return full;
	}
	else if (c == '.')
	{
		full[0] = 0xE2;
		full[1] = 0x80;
		full[2] = 0xA7;
		full[3] = '\0';
		return full;
	}
	else
		return NULL;
}

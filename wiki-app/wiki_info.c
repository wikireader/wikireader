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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc-simple.h>
#include <file-io.h>
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "wiki_info.h"
#include "search.h"
#include "search_hash.h"
#include "msg.h"

WIKI_LIST wiki_list[] =
{
	{ 1,  1, WIKI_CAT_ENCYCLOPAEDIA, "en", "enpedia", KEYBOARD_CHAR, 0},
	{ 2,  2, WIKI_CAT_ENCYCLOPAEDIA, "es", "espedia", KEYBOARD_CHAR, 0},
	{ 3,  3, WIKI_CAT_ENCYCLOPAEDIA, "fr", "frpedia", KEYBOARD_CHAR, 0},
	{ 4,  4, WIKI_CAT_ENCYCLOPAEDIA, "de", "depedia", KEYBOARD_CHAR, 0},
	{ 5,  5, WIKI_CAT_ENCYCLOPAEDIA, "nl", "nlpedia", KEYBOARD_CHAR, 0},
	{ 6,  6, WIKI_CAT_ENCYCLOPAEDIA, "pt", "ptpedia", KEYBOARD_CHAR, 0},
	{ 7,  7, WIKI_CAT_ENCYCLOPAEDIA, "fi", "fipedia", KEYBOARD_CHAR, 0},
	{ 8,  8, WIKI_CAT_ENCYCLOPAEDIA, "ja", "japedia", KEYBOARD_PHONE_STYLE_JP, 0},
	{ 9,  8, WIKI_CAT_ENCYCLOPAEDIA, "ja", "japedia", KEYBOARD_CHAR_JP, 2},
	{10,  9, WIKI_CAT_ENCYCLOPAEDIA, "da", "dapedia", KEYBOARD_CHAR, 0},
	{11, 10, WIKI_CAT_ENCYCLOPAEDIA, "no", "nopedia", KEYBOARD_CHAR, 0},
	{12, 11, WIKI_CAT_ENCYCLOPAEDIA, "hu", "hupedia", KEYBOARD_CHAR, 0},
	{13, 12, WIKI_CAT_ENCYCLOPAEDIA, "ko", "kopedia", KEYBOARD_CHAR_KO, 0},
	{14, 13, WIKI_CAT_ENCYCLOPAEDIA, "el", "elpedia", KEYBOARD_CHAR, 0},
	{15, 14, WIKI_CAT_ENCYCLOPAEDIA, "ru", "rupedia", KEYBOARD_CHAR, 0},
	{16, 15, WIKI_CAT_ENCYCLOPAEDIA, "zhs", "zhpedia", KEYBOARD_CHAR, 0},
//	{17, 15, WIKI_CAT_ENCYCLOPAEDIA, "zht", "zhpedia", KEYBOARD_PHONE_STYLE_TW, 2},
	{17, 15, WIKI_CAT_ENCYCLOPAEDIA, "zht", "z_pedia", KEYBOARD_PHONE_STYLE_TW, 2},
	{18, 16, WIKI_CAT_ENCYCLOPAEDIA, "cy", "cypedia", KEYBOARD_CHAR, 0},
	{19, 17, WIKI_CAT_ENCYCLOPAEDIA, "pl", "plpedia", KEYBOARD_CHAR, 0},
	{20, 18, WIKI_CAT_ENCYCLOPAEDIA, "simple", "e0pedia", KEYBOARD_CHAR, 0},
	{21, 19, WIKI_CAT_QUOTE,         "en", "enquote", KEYBOARD_CHAR, 0},
	{22, 20, WIKI_CAT_BOOKS,         "en", "enbooks", KEYBOARD_CHAR, 0},
	{23, 21, WIKI_CAT_DICTIONARY,    "en", "endict", KEYBOARD_CHAR, 0},
};
#define MAX_WIKIS (sizeof(wiki_list) / sizeof(WIKI_LIST))

extern int search_interrupted;
int nWikiCount = 0;
int aWikiInfoIdx[MAX_WIKIS_PER_DEVICE]; // index to wiki_info[]
char *aWikiNls[MAX_WIKIS_PER_DEVICE];
long aWikiNlsLen[MAX_WIKIS_PER_DEVICE];
int nCurrentWiki = -1; // index to aWikiInfoIdx[]
bool bWikiIsJapanese = false;
bool bWikiIsKorean = false;
bool bWikiIsTC = false;
KEYBOARD_MODE default_keyboard = KEYBOARD_CHAR;
int rendered_wiki_selection_count = -1;
int current_article_wiki_id = 0;
WIKI_LICENSE_DRAW aWikiLicenseDraw[MAX_WIKIS_PER_DEVICE];
char *pWikiIni = NULL;
unsigned int lenWikiIni = 0;
unsigned int sizeWikiIni = 0;
extern int bShowPositioner;

char *get_nls_key_value(char *key, char *key_pairs, long key_pairs_len, int wiki_nls_idx);
int get_wiki_idx_from_serial_id(int wiki_serial_id);

void init_wiki_info(void)
{
	int i;
	int fd;
	char sFilePath[20];
	char *p;
	int nWikiSerialId;

	for (i = 0; i < MAX_WIKIS && nWikiCount < MAX_WIKIS_PER_DEVICE; i++)
	{
		sprintf(sFilePath, "%s/wiki.idx", wiki_list[i].wiki_folder);
		fd = wl_open(sFilePath, WL_O_RDONLY);
		if (fd >= 0)
		{
			aWikiInfoIdx[nWikiCount] = i;
			nWikiCount++;
			wl_close(fd);
		}
	}

	if (nWikiCount > 0)
	{
		nCurrentWiki = 0;
		fd = wl_open("wiki.ini", WL_O_RDONLY);
		if (fd >= 0)
		{
			wl_fsize(fd, &lenWikiIni);
			sizeWikiIni = lenWikiIni + 16;  // reserve space for wiki_id key pair
			pWikiIni = malloc_simple(sizeWikiIni, MEM_TAG_INDEX_M1);
			if (pWikiIni)
			{
				memset(pWikiIni, 0, sizeWikiIni);
				wl_read(fd, pWikiIni, lenWikiIni);
				wl_close(fd);
				pWikiIni[lenWikiIni] = '\0';
				p = pWikiIni;
				while (*p)
				{
					if (*p == '\r' || *p == '\n')
						*p = '\0';
					p++;
				}
				p = get_nls_key_value("positioner", pWikiIni, lenWikiIni, 0);
				if (*p)
					bShowPositioner = atoi(p);
				p = get_nls_key_value("wiki_id", pWikiIni, lenWikiIni, 0);
				if (*p)
				{
					nWikiSerialId = atoi(p);
					if (nWikiSerialId > 0)
						nCurrentWiki = get_wiki_idx_from_serial_id(nWikiSerialId);
					if (nCurrentWiki < 0)
						nCurrentWiki = 0;
				}
				else
				{
					lenWikiIni = 0; // if no wiki_id entry in wiki.ini, reset the content of wiki.ini
					nCurrentWiki = 0;
				}
			}
		}
		for (i = 0; i < nWikiCount; i++)
		{
			aWikiNlsLen[i] = -1;
			aWikiLicenseDraw[i].lines = 0;
		}
		if (!strcmp(wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_lang, "ja"))
			bWikiIsJapanese = true;
		else
			bWikiIsJapanese = false;
		if (!strcmp(wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_lang, "zht")) // Simplified Chinese Pinyin IME does not need special conversion
			bWikiIsTC = true;
		else
			bWikiIsTC = false;
		if (!strcmp(wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_lang, "ko"))
			bWikiIsKorean = true;
		else
			bWikiIsKorean = false;
		default_keyboard = wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_default_keyboard;
		keyboard_set_mode(default_keyboard);
	}
	else
		fatal_error("No wiki found");
}

int get_wiki_idx_by_lang_link(char *lang_link_str)
{
	int len;
	char *p, *q;
	int i;
	int current_wiki_cat = -1;

	p = strchr(lang_link_str, ':');
	q = strchr(lang_link_str, '#');
	if (!p || (q && q < p))
		p = q;

	if (p)
	{
		int wiki_idx = get_wiki_idx_from_id(current_article_wiki_id);
		current_wiki_cat = wiki_list[aWikiInfoIdx[wiki_idx]].wiki_cat;
		len = p - lang_link_str;
		for (i = 0; i < nWikiCount; i++)
		{
			if (current_wiki_cat == wiki_list[aWikiInfoIdx[i]].wiki_cat && !strncmp(lang_link_str, wiki_list[aWikiInfoIdx[i]].wiki_lang, len))
				return i;
		}
	}
	return -1;
}

bool wiki_lang_exist(char *lang_link_str)
{
	if (get_wiki_idx_by_lang_link(lang_link_str) >= 0)
		return true;
	else
		return false;
}

bool wiki_is_cjk()
{
	return bWikiIsJapanese || bWikiIsTC || bWikiIsKorean;
}

bool wiki_is_japanese()
{
	return bWikiIsJapanese;
}

bool wiki_is_TC()
{
	return bWikiIsTC;
}

bool wiki_is_korean()
{
	return bWikiIsKorean;
}

KEYBOARD_MODE wiki_default_keyboard()
{
	return default_keyboard;
}
uint32_t wiki_lang_link_search(char *lang_link_str)
{
	uint32_t article_idx = 0;
	int nTempCurrentWiki = nCurrentWiki;
	char *p, *q;

	search_interrupted = 0;
	if ((nCurrentWiki = get_wiki_idx_by_lang_link(lang_link_str)) >= 0)
	{
		init_search_hash();
		p = strchr(lang_link_str, ':');
		q = strchr(lang_link_str, '#');
		if (!p || (q && q < p))
			p = q;
		if (p)
		{
			if (*p == '#') // actual title is different than title for search
			{
				q = strchr(p + 1, CHAR_LANGUAGE_LINK_TITLE_DELIMITER); // locate the actual title
				if (!q)
					q = p;
			}
			else
				q = p;

			article_idx = get_article_idx_by_title(p + 1, q + 1);
			if (article_idx)
				article_idx |= wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_id << 24;
		}
	}
	nCurrentWiki = nTempCurrentWiki;
	return article_idx;
}
char *get_wiki_file_path(int nWikiIdx, char *file_name)
{
	static char sFilePath[32];


	sprintf(sFilePath, "%s/%s", wiki_list[aWikiInfoIdx[nWikiIdx]].wiki_folder, file_name);
	return sFilePath;
}

int get_wiki_count(void)
{
	return nWikiCount;
}

int get_wiki_idx_from_id(int wiki_id)
{
	int i;

	if (wiki_id <= 0)
		return nCurrentWiki;

	for (i = 0; i < nWikiCount; i++)
	{
		if (wiki_list[aWikiInfoIdx[i]].wiki_id == wiki_id)
			return i;
	}
	return -1;
}

int get_wiki_idx_from_serial_id(int wiki_serial_id)
{
	int i;

	for (i = 0; i < nWikiCount; i++)
	{
		if (wiki_list[aWikiInfoIdx[i]].wiki_serial_id == wiki_serial_id)
			return i;
	}
	return -1;
}

int get_wiki_id_from_idx(int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aWikiInfoIdx[wiki_idx]].wiki_id;
	}
	return 0;
}

int get_wiki_serial_id_from_idx(int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aWikiInfoIdx[wiki_idx]].wiki_serial_id;
	}
	return 0;
}

char *get_nls_key_value(char *key, char *key_pairs, long key_pairs_len, int wiki_nls_idx)
{
	int i, j;
	int key_len;
	int bFound = 0;
	char local_key[64];

	if (wiki_nls_idx)
		sprintf(local_key, "%s%d", key, wiki_nls_idx); // try the key name with wiki_name_idx first
	else
		strcpy(local_key, key);

	key_len = strlen(local_key);
	i = 0;
	while (i < key_pairs_len - key_len - 1 && !bFound)
	{
		for (j = 0; j < key_len; j++)
		{
			if (local_key[j] != key_pairs[i + j])
				break;
		}
		i += j;
		if (j == key_len && key_pairs[i] == '=')
		{
			bFound = 1;
		}
		else
		{
			while (key_pairs[i] != '\0')
				i++;
			while (i < key_pairs_len - key_len - 1 && key_pairs[i] == '\0')
				i++;
		}
	}
	if (bFound)
		return &key_pairs[i + 1];
	else if (wiki_nls_idx)
		return get_nls_key_value(key, key_pairs, key_pairs_len, 0); // try the key name without wiki_name_idx
	else
		return "";
}

char *get_nls_text(char *key)
{
	int fd;
	unsigned int nSize;
	char *p;

	if (nCurrentWiki < 0)
		return "";
	else
	{
		if (aWikiNlsLen[nCurrentWiki] < 0)
		{
			fd = wl_open(get_wiki_file_path(nCurrentWiki, "wiki.nls"), WL_O_RDONLY);
			if (fd >= 0)
			{
				wl_fsize(fd, &nSize);
				aWikiNlsLen[nCurrentWiki] = nSize;
				aWikiNls[nCurrentWiki] = malloc_simple(nSize + 1, MEM_TAG_INDEX_M1);
				if (aWikiNls[nCurrentWiki])
				{
					wl_read(fd, aWikiNls[nCurrentWiki], nSize);
					wl_close(fd);
					aWikiNls[nCurrentWiki][nSize] = '\0';
					p = aWikiNls[nCurrentWiki];
					while (*p)
					{
						if (*p == '\r' || *p == '\n')
							*p = '\0';
						p++;
					}
				}
				else
					aWikiNlsLen[nCurrentWiki] = 0;
			}
			else
			{
				aWikiNlsLen[nCurrentWiki] = 0;
			}
		}

		if (aWikiNlsLen[nCurrentWiki] == 0)
			return "";

		return get_nls_key_value(key, aWikiNls[nCurrentWiki], aWikiNlsLen[nCurrentWiki], wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_nls_idx);
	}
}

char *get_lang_link_display_text(char *lang_link_str)
{
	int nTempCurrentWiki = nCurrentWiki;
	static char lang_str[3];
	char *p = NULL;

	if ((nCurrentWiki = get_wiki_idx_by_lang_link(lang_link_str)) >= 0)
	{
		p = get_nls_text("lang_str");
		if (p[0] == '\0')
			p = NULL;
	}

	if (!p)
	{
		memcpy(lang_str, lang_link_str, 2);
		lang_str[2] = '\0';
		p = lang_str;
	}

	nCurrentWiki = nTempCurrentWiki;
	return p;
}

void wiki_selection(void)
{
	rendered_wiki_selection_count = 0;
	render_wiki_selection_with_pcf();
}

char *get_wiki_name(int idx)
{
	int nTempCurrentWiki = nCurrentWiki;
	char *pName;

	nCurrentWiki = idx;
	pName = get_nls_text("wiki_name");
	nCurrentWiki = nTempCurrentWiki;
	return pName;
}

void wiki_ini_insert_keypair(char *key, char *keyval)
{
	char *p = get_nls_key_value(key, pWikiIni, lenWikiIni, 0);

	if (*p)
	{
		if (strlen(p) < strlen(keyval))
		{
			int diff = strlen(keyval) - strlen(p);
			int move_size;

			if (lenWikiIni + diff < sizeWikiIni)
			{
				move_size = lenWikiIni - (p - pWikiIni);
				if (move_size > 0)
					memmove(p + diff, p, move_size);
			}
			lenWikiIni += diff;
		}
		if (p + strlen(keyval) < pWikiIni + sizeWikiIni - 1)
		{
			memset(p, 0, strlen(keyval) + 1);
			memcpy(p, keyval, strlen(keyval));
		}
	}
	else if (lenWikiIni + strlen(key) + strlen(keyval) + 2 < sizeWikiIni)
	{
		if (lenWikiIni)
			pWikiIni[lenWikiIni++] = '\0'; // make sure the new key pair start with a new line
		memcpy(&pWikiIni[lenWikiIni], key, strlen(key));
		lenWikiIni += strlen(key);
		pWikiIni[lenWikiIni++] = '=';
		memcpy(&pWikiIni[lenWikiIni], keyval, strlen(keyval));
		lenWikiIni += strlen(keyval);
		pWikiIni[lenWikiIni] = '\0';
	}
}

void set_wiki(int idx)
{
	int fd;
	char sWikiId[10];
	int i;
	char prev_c = 0;

	nCurrentWiki = idx;
	if (!strcmp(wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_lang, "ja"))
		bWikiIsJapanese = true;
	else
		bWikiIsJapanese = false;
	if (!strcmp(wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_lang, "zht"))
		bWikiIsTC = true;
	else
		bWikiIsTC = false;
	if (!strcmp(wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_lang, "ko"))
		bWikiIsKorean = true;
	else
		bWikiIsKorean = false;
	default_keyboard = wiki_list[aWikiInfoIdx[nCurrentWiki]].wiki_default_keyboard;
	fd = wl_open("wiki.ini", WL_O_CREATE);
	if (fd >= 0)
	{
		sprintf(sWikiId, "%d", get_wiki_serial_id_from_idx(nCurrentWiki));
		wiki_ini_insert_keypair("wiki_id", sWikiId);
		for (i = 0; i < lenWikiIni; i++)
		{
			if (pWikiIni[i] == '\0')
			{
				if (prev_c != '\0')
					wl_write(fd, "\n", 1);
			}
			else
				wl_write(fd, &pWikiIni[i], 1);
			prev_c = pWikiIni[i];
		}
		wl_close(fd);
	}
}

void nls_replace_text(char *replace_str, char *out_str)
{
	if (!strcmp(replace_str, "title"))
	{
		extract_title_from_article(NULL, out_str);
		while (*out_str)
		{
			if (*out_str == ' ')
				*out_str = '_';
			out_str++;
		}

	}
	else
	{
		strcpy(out_str, replace_str);
	}
}

WIKI_LICENSE_DRAW *wiki_license_draw()
{
	int wiki_idx = get_wiki_idx_from_id(current_article_wiki_id);
	int nTempCurrentWiki = nCurrentWiki;

	if (wiki_idx >= 0)
		nCurrentWiki = wiki_idx;
//	if (!aWikiLicenseDraw[wiki_idx].lines)
	{
		int y = 0;
		int x = 0;
		char draw_buf[MAX_LICENSE_TEXT_PIXEL_LINES * LCD_BUF_WIDTH_BYTES];
		unsigned char *pLicenseText = get_nls_text("license_text");

		unsigned char footer_buffer[40];
		memset(footer_buffer, '\0', sizeof(footer_buffer));
		memset(footer_buffer, ' ', 5);

		int fd = wl_open(get_wiki_file_path(nCurrentWiki, "wiki.ftr"), WL_O_RDONLY);
		if (fd >= 0)
		{
			unsigned char c = ' ';
			unsigned char *p = &footer_buffer[5];
			while (' ' == c) {
				wl_read(fd, &c, sizeof(c));
			}
			while (c >= ' ' && p < &footer_buffer[sizeof(footer_buffer) - 1]) {
				*p++ = c;
				wl_read(fd, &c, sizeof(c));
			}
			wl_close(fd);
		}
		unsigned char *footer = footer_buffer;

		char sLicenseTextSegment[MAX_LICENSE_TEXT_LEN];
		unsigned char *pLicenseTextSegment;
		int line_height = pcfFonts[LICENSE_TEXT_FONT - 1].Fmetrics.linespace;
		char str[256];  // possible buffer overflow
		unsigned char *p, *q;
		int bInLink = 0;
		int nLinkArticleId = 0;
		int width;
		int start_x, start_y, end_x, end_y;
		int i;

		aWikiLicenseDraw[wiki_idx].link_count = 0;
		memset(draw_buf, 0, sizeof(draw_buf));
		sLicenseTextSegment[0] = '\0';
		pLicenseTextSegment = sLicenseTextSegment;
		while (*pLicenseText && y < MAX_LICENSE_TEXT_PIXEL_LINES)
		{
			if (!*pLicenseTextSegment)
			{
				if (pLicenseText[0] == LICENSE_LINK_START)
				{
					bInLink = 1;
					nLinkArticleId++;
					pLicenseText++;
					if ((p = strchr(pLicenseText, LICENSE_LINK_END)))
					{
						memcpy(sLicenseTextSegment, pLicenseText, p - pLicenseText);
						sLicenseTextSegment[p - pLicenseText] = '\0';
						pLicenseText = p + 1;
					}
					else
					{
						strcpy(sLicenseTextSegment, pLicenseText);
						pLicenseText += strlen(pLicenseText);
					}
				}
				else if (pLicenseText[0] == NLS_TEXT_REPLACEMENT_START)
				{
					bInLink = 0;
					pLicenseText++;
					if ((p = strchr(pLicenseText, NLS_TEXT_REPLACEMENT_END)))
					{
						memcpy(str, pLicenseText, p - pLicenseText);
						str[p - pLicenseText] = '\0';
						pLicenseText = p + 1;
					}
					else
					{
						strcpy(str, pLicenseText);
						pLicenseText += strlen(pLicenseText);
					}
					nls_replace_text(str, sLicenseTextSegment);
				}
				else
				{
					bInLink = 0;
					p = strchr(pLicenseText, LICENSE_LINK_START);
					q = strchr(pLicenseText, NLS_TEXT_REPLACEMENT_START);
					if ((p && q && p > q) || (!p))
						p = q;
					if (p)
					{
						memcpy(sLicenseTextSegment, pLicenseText, p - pLicenseText);
						sLicenseTextSegment[p - pLicenseText] = '\0';
						pLicenseText = p;
					}
					else
					{
						strcpy(sLicenseTextSegment, pLicenseText);
						pLicenseText += strlen(pLicenseText);
					}
				}
				pLicenseTextSegment = sLicenseTextSegment;
			}

			while (*pLicenseTextSegment && y < MAX_LICENSE_TEXT_PIXEL_LINES)
			{
				if (!x)
				{
					while (*pLicenseTextSegment == ' ')
						pLicenseTextSegment++;
				}

				width = extract_str_fitting_width(&pLicenseTextSegment, str, LCD_BUF_WIDTH_PIXELS - x - LCD_LEFT_MARGIN, LICENSE_TEXT_FONT);
				if (*str) // if null string, the next segment will start from a new line
				{
					p = str;
					buf_draw_UTF8_str_in_copy_buffer(draw_buf, &p, x, LCD_BUF_WIDTH_PIXELS,
									 y, y + line_height - 1, LCD_LEFT_MARGIN, LICENSE_TEXT_FONT);
					if (bInLink && aWikiLicenseDraw[wiki_idx].link_count < MAX_LINKS_IN_LICENSE_TEXT)
					{
						start_x = x;
						start_y = y + 4; // for sync with the links in article text
						end_x = x + width;
						end_y = y + line_height + 3;
						aWikiLicenseDraw[wiki_idx].links[aWikiLicenseDraw[wiki_idx].link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
						aWikiLicenseDraw[wiki_idx].links[aWikiLicenseDraw[wiki_idx].link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
						aWikiLicenseDraw[wiki_idx].links[aWikiLicenseDraw[wiki_idx].link_count++].article_id = nLinkArticleId;
						for(i = start_x + LCD_LEFT_MARGIN; i < end_x + LCD_LEFT_MARGIN; i++)
						{
							lcd_set_pixel(draw_buf, i, end_y - 1);
						}
					}
				}
				if (*pLicenseTextSegment)
				{
					x = 0;
					y += line_height;
				}
				else
					x += width;
			}
			if ('\0' == *pLicenseText && NULL != footer) {
				pLicenseText = footer;
				footer = NULL;
			}
		}
		if (x)
			y += line_height;
		y += SPACE_AFTER_LICENSE_TEXT;
		aWikiLicenseDraw[wiki_idx].lines = y;
		aWikiLicenseDraw[wiki_idx].buf = malloc_simple(y * LCD_BUF_WIDTH_BYTES, MEM_TAG_INDEX_M1);
		memcpy(aWikiLicenseDraw[wiki_idx].buf, draw_buf, y * LCD_BUF_WIDTH_BYTES);
	}
	nCurrentWiki = nTempCurrentWiki;
	return &aWikiLicenseDraw[wiki_idx];
}

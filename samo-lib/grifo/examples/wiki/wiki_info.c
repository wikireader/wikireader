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
#include "grifo.h"
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "wiki_info.h"
#include "search.h"
#include "search_fnd.h"

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
	{10,  9, WIKI_CAT_ENCYCLOPAEDIA, "da", "dapedia", KEYBOARD_CHAR_DA, 0},
	{11, 10, WIKI_CAT_ENCYCLOPAEDIA, "no", "nopedia", KEYBOARD_CHAR, 0},
	{12, 11, WIKI_CAT_ENCYCLOPAEDIA, "hu", "hupedia", KEYBOARD_CHAR, 0},
	{13, 12, WIKI_CAT_ENCYCLOPAEDIA, "ko", "kopedia", KEYBOARD_CHAR_KO, 0},
	{14, 13, WIKI_CAT_ENCYCLOPAEDIA, "el", "elpedia", KEYBOARD_CHAR, 0},
	{15, 14, WIKI_CAT_ENCYCLOPAEDIA, "ru", "rupedia", KEYBOARD_CHAR, 0},
	{16, 15, WIKI_CAT_ENCYCLOPAEDIA, "zhs", "zhpedia", KEYBOARD_CHAR, 0},
	{17, 15, WIKI_CAT_ENCYCLOPAEDIA, "zht", "zhpedia", KEYBOARD_PHONE_STYLE_TW, 2},
	{18, 16, WIKI_CAT_ENCYCLOPAEDIA, "cy", "cypedia", KEYBOARD_CHAR, 0},
	{19, 17, WIKI_CAT_ENCYCLOPAEDIA, "pl", "plpedia", KEYBOARD_CHAR, 0},
	{20, 18, WIKI_CAT_ENCYCLOPAEDIA, "simple", "e0pedia", KEYBOARD_CHAR, 0},
	{21, 19, WIKI_CAT_QUOTE,         "en", "enquote", KEYBOARD_CHAR, 0},
	{22, 20, WIKI_CAT_BOOKS,         "en", "enbooks", KEYBOARD_CHAR, 0},
	{23, 21, WIKI_CAT_DICTIONARY,    "en", "endict", KEYBOARD_CHAR, 0},
	{24, 22, WIKI_CAT_BOOKS,         "en", "enguten", KEYBOARD_CHAR, 0},
	{25, 23, WIKI_CAT_ENCYCLOPAEDIA, "it", "itpedia", KEYBOARD_CHAR, 0},
};
#define MAX_WIKIS (sizeof(wiki_list) / sizeof(WIKI_LIST))

bool baWikiActive[MAX_WIKIS];
extern int search_interrupted;
int nWikiCount = 0;
int nCurrentWiki = -1; // index to aActiveWikis[].WikiInfoIdx
bool bWikiIsDanish = false;
bool bWikiIsJapanese = false;
bool bWikiIsKorean = false;
bool bWikiIsTC = false;
KEYBOARD_MODE default_keyboard = KEYBOARD_CHAR;
int rendered_wiki_selection_count = -1;
int current_article_wiki_id = 0;
char *pWikiIni = NULL;
unsigned long lenWikiIni = 0;
unsigned long sizeWikiIni = 0;
PACTIVE_WIKI aActiveWikis = NULL;
extern int bShowPositioner;
WIKI_LICENSE_DRAW *pWikiLicenseDraw;

char *get_nls_key_value(char *key, char *key_pairs, long key_pairs_len, int wiki_nls_idx);
int get_wiki_idx_from_serial_id(int wiki_serial_id);

void init_wiki_info(void)
{
	int i, j;
	int fd;
	char *p;
	int nWikiSerialId;

    memset(baWikiActive, 0, sizeof(baWikiActive));
	for (i = 0; i < MAX_WIKIS; i++)
	{
		if (directory_exists(wiki_list[i].wiki_folder))
		{
            baWikiActive[i] = true;
            nWikiCount++;
		}
	}
	
	if (nWikiCount > 0)
	{
		pWikiLicenseDraw = (PWIKI_LICENSE_DRAW)memory_allocate(sizeof(WIKI_LICENSE_DRAW), "wikiinfo0");
		aActiveWikis = (PACTIVE_WIKI)memory_allocate(sizeof(ACTIVE_WIKI) * nWikiCount, "wikiinfo1");
        if (!pWikiLicenseDraw || !aActiveWikis)
        {
            fatal_error("too many wikis");
        }
		pWikiLicenseDraw->lines = 0;

        j = 0;
        for (i = 0; i < MAX_WIKIS; i++)
    	{
            if (baWikiActive[i])
                aActiveWikis[j++].WikiInfoIdx = i;
    	}

		nCurrentWiki = 0;
		fd = file_open("wiki.ini", FILE_OPEN_READ);
		if (fd >= 0)
		{
			file_size("wiki.ini", &lenWikiIni);
			sizeWikiIni = lenWikiIni + 16;  // reserve space for wiki_id key pair
			pWikiIni = memory_allocate(sizeWikiIni, "wikiinfo2");
			if (pWikiIni)
			{
				memset(pWikiIni, 0, sizeWikiIni);
				file_read(fd, pWikiIni, lenWikiIni);
				file_close(fd);
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
		else
		{
            lenWikiIni = 0;
            pWikiIni = memory_allocate(20, "wikiinfo3");
			memset(pWikiIni, 0, 20);
		}

		for (i = 0; i < nWikiCount; i++)
		{
			aActiveWikis[i].WikiNlsLen = -1;
		}
		if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ja"))
			bWikiIsJapanese = true;
		else
			bWikiIsJapanese = false;
		if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "zht")) // Simplified Chinese Pinyin IME does not need special conversion
			bWikiIsTC = true;
		else
			bWikiIsTC = false;
		if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ko"))
			bWikiIsKorean = true;
		else
			bWikiIsKorean = false;
		if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "da"))
			bWikiIsDanish = true;
		else
			bWikiIsDanish = false;
		default_keyboard = wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_default_keyboard;
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
		current_wiki_cat = wiki_list[aActiveWikis[wiki_idx].WikiInfoIdx].wiki_cat;
		len = p - lang_link_str;
		for (i = 0; i < nWikiCount; i++)
		{
			if (current_wiki_cat == wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_cat &&
                    !strncmp(lang_link_str, wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_lang, len))
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

bool wiki_keyboard_conversion_needed()
{
	return bWikiIsJapanese || bWikiIsTC || bWikiIsKorean || bWikiIsDanish;
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

bool wiki_is_danish()
{
	return bWikiIsDanish;
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
		reset_search_info(nCurrentWiki);
		init_search_fnd();
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
				article_idx |= wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_id << 24;
		}
	}
	nCurrentWiki = nTempCurrentWiki;
	return article_idx;
}
char *get_wiki_file_path(int nWikiIdx, char *file_name)
{
	static char sFilePath[32];


	sprintf(sFilePath, "%s/%s", wiki_list[aActiveWikis[nWikiIdx].WikiInfoIdx].wiki_folder, file_name);
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
		if (wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_id == wiki_id)
			return i;
	}
	return -1;
}

int get_wiki_idx_from_serial_id(int wiki_serial_id)
{
	int i;

	for (i = 0; i < nWikiCount; i++)
	{
		if (wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_serial_id == wiki_serial_id)
			return i;
	}
	return -1;
}

int get_wiki_id_from_idx(int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aActiveWikis[wiki_idx].WikiInfoIdx].wiki_id;
	}
	return 0;
}

int get_wiki_serial_id_from_idx(int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aActiveWikis[wiki_idx].WikiInfoIdx].wiki_serial_id;
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
	unsigned long nSize;
	char *p;

	if (nCurrentWiki < 0)
		return "";
	else
	{
		if (aActiveWikis[nCurrentWiki].WikiNlsLen < 0)
		{
			fd = file_open(get_wiki_file_path(nCurrentWiki, "wiki.nls"), FILE_OPEN_READ);
			if (fd >= 0)
			{
				file_size(get_wiki_file_path(nCurrentWiki, "wiki.nls"), &nSize);
				aActiveWikis[nCurrentWiki].WikiNlsLen = nSize;
				aActiveWikis[nCurrentWiki].WikiNls = memory_allocate(nSize + 1, "wikiinfo4");
				if (aActiveWikis[nCurrentWiki].WikiNls)
				{
					file_read(fd, aActiveWikis[nCurrentWiki].WikiNls, nSize);
					file_close(fd);
					aActiveWikis[nCurrentWiki].WikiNls[nSize] = '\0';
					p = aActiveWikis[nCurrentWiki].WikiNls;
					while (*p)
					{
						if (*p == '\r' || *p == '\n')
							*p = '\0';
						p++;
					}
				}
				else
					aActiveWikis[nCurrentWiki].WikiNlsLen = 0;
			}
			else
			{
				aActiveWikis[nCurrentWiki].WikiNlsLen = 0;
			}
		}

		if (aActiveWikis[nCurrentWiki].WikiNlsLen == 0)
			return "";

		return get_nls_key_value(key, aActiveWikis[nCurrentWiki].WikiNls, aActiveWikis[nCurrentWiki].WikiNlsLen, wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_nls_idx);
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
	reset_search_info(nCurrentWiki);
	if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ja"))
		bWikiIsJapanese = true;
	else
		bWikiIsJapanese = false;
	if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "zht"))
		bWikiIsTC = true;
	else
		bWikiIsTC = false;
	if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ko"))
		bWikiIsKorean = true;
	else
		bWikiIsKorean = false;
	if (!strcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "da"))
		bWikiIsDanish = true;
	else
		bWikiIsDanish = false;
	default_keyboard = wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_default_keyboard;
	fd = file_open("wiki.ini", FILE_OPEN_WRITE);
	if (fd < 0)
		fd = file_create("wiki.ini", FILE_OPEN_WRITE);
	if (fd >= 0)
	{
		sprintf(sWikiId, "%d", get_wiki_serial_id_from_idx(nCurrentWiki));
		wiki_ini_insert_keypair("wiki_id", sWikiId);
		for (i = 0; i < lenWikiIni; i++)
		{
			if (pWikiIni[i] == '\0')
			{
				if (prev_c != '\0')
					file_write(fd, "\n", 1);
			}
			else
				file_write(fd, &pWikiIni[i], 1);
			prev_c = pWikiIni[i];
		}
		file_close(fd);
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
	int y = 0;
	int x = 0;
	char draw_buf[MAX_LICENSE_TEXT_PIXEL_LINES * LCD_BUF_WIDTH_BYTES];
	unsigned char *pLicenseText;
	unsigned char footer_buffer[40];
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

	if (wiki_idx >= 0)
		nCurrentWiki = wiki_idx;

	pLicenseText = get_nls_text("license_text");
	memset(footer_buffer, '\0', sizeof(footer_buffer));
	memset(footer_buffer, ' ', 5);

	int fd = file_open(get_wiki_file_path(nCurrentWiki, "wiki.ftr"), FILE_OPEN_READ);
	if (fd >= 0)
	{
		unsigned char c = ' ';
		unsigned char *p = &footer_buffer[5];
		while (' ' == c) {
			file_read(fd, &c, sizeof(c));
		}
		while (c >= ' ' && p < &footer_buffer[sizeof(footer_buffer) - 1]) {
			*p++ = c;
			file_read(fd, &c, sizeof(c));
		}
		file_close(fd);
	}

	pWikiLicenseDraw->link_count = 0;
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
				if (bInLink && pWikiLicenseDraw->link_count < MAX_LINKS_IN_LICENSE_TEXT)
				{
					start_x = x;
					start_y = y + 4; // for sync with the links in article text
					end_x = x + width;
					end_y = y + line_height + 3;
					pWikiLicenseDraw->links[pWikiLicenseDraw->link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
					pWikiLicenseDraw->links[pWikiLicenseDraw->link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
					pWikiLicenseDraw->links[pWikiLicenseDraw->link_count++].article_id = nLinkArticleId;
					for(i = start_x + LCD_LEFT_MARGIN; i < end_x + LCD_LEFT_MARGIN; i++)
					{
						lcd_buffer_set_pixel(draw_buf, i, end_y - 1);
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
	pWikiLicenseDraw->lines = y;
	pWikiLicenseDraw->buf = memory_allocate(y * LCD_BUF_WIDTH_BYTES, "wikinfo5");
	memcpy(pWikiLicenseDraw->buf, draw_buf, y * LCD_BUF_WIDTH_BYTES);
	
	nCurrentWiki = nTempCurrentWiki;
	return pWikiLicenseDraw;
}

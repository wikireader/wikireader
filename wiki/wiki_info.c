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
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include <grifo.h>

#include "ustring.h"
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "wiki_info.h"
#include "search.h"
#include "search_fnd.h"
#include "guilib.h"

WIKI_LIST wiki_list_default[] =
{
	{ 1,  1, WIKI_CAT_ENCYCLOPAEDIA, "en", "enpedia", KEYBOARD_CHAR, 0},
#if 0
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
	{24, 22, WIKI_CAT_GUTENBERG,     "en", "enguten", KEYBOARD_CHAR, 0},
	{25, 23, WIKI_CAT_ENCYCLOPAEDIA, "it", "itpedia", KEYBOARD_CHAR, 0},
#endif
};

WIKI_LIST *wiki_list;
unsigned int nWikiList = 0;
bool *baWikiActive;
extern int search_interrupted;
unsigned int nWikiCount = 0;
int nCurrentWiki = -1; // index to aActiveWikis[].WikiInfoIdx
bool bWikiIsDanish = false;
bool bWikiIsJapanese = false;
bool bWikiIsKorean = false;
bool bWikiIsTC = false;
KEYBOARD_MODE default_keyboard = KEYBOARD_CHAR;
int rendered_wiki_selection_count = -1;
int current_article_wiki_id = 0;
unsigned char *pWikiIni = NULL;
unsigned long lenWikiIni = 0;
unsigned long sizeWikiIni = 0;
PACTIVE_WIKI aActiveWikis = NULL;
extern int bShowPositioner;
WIKI_LICENSE_DRAW *pWikiLicenseDraw;

unsigned char *get_nls_key_value(const char *key, unsigned char *key_pairs, long key_pairs_len, int wiki_nls_idx);
int get_wiki_idx_from_serial_id(int wiki_serial_id);

#define MAX_LINE_SIZE 256
static ssize_t copy_line(char *buf, char *line, ssize_t nLineChars)
{
	ssize_t len_left;
	int i = 0;

	while (i < nLineChars && i < MAX_LINE_SIZE - 1 && line[i] != '\r' && line[i] != '\n')
		i++;
	if (i > 0)
		memcpy(buf, line, i);
	buf[i] = '\0';
	while (i < nLineChars && (line[i] == '\r' || line[i] == '\n'))
		i++;
	len_left = nLineChars - i;
	if (len_left > 0)
		memmove(line, &line[i], len_left);
	return len_left;
}

static void trim_leading_spaces(char *buf)
{
	int i = 0;
	while (buf[i] && strchr(" \t\r\n", buf[i]))
		i++;
	if (i > 0)
	{
		int nMoveLen = strlen(buf) - i;

		memmove(buf, &buf[i], nMoveLen + 1);
	}
}

static void trim_trailing_spaces(char *buf)
{
	int i = strlen(buf);

	while (i && strchr(" \t\r\n", buf[i]))
		i--;
	buf[i + 1] = '\0';
}

static WIKI_CAT_E get_category_info(char *word)
{
	if (!word[0] || !strcmp(word, "ENCYCLOPAEDIA"))
		return WIKI_CAT_ENCYCLOPAEDIA;
	else if (!strcmp(word, "TRAVEL"))
		return WIKI_CAT_TRAVEL;
	else if (!strcmp(word, "DICTIONARY"))
		return WIKI_CAT_DICTIONARY;
	else if (!strcmp(word, "QUOTE"))
		return WIKI_CAT_QUOTE;
	else if (!strcmp(word, "SOURCE"))
		return WIKI_CAT_SOURCE;
	else if (!strcmp(word, "BOOKS"))
		return WIKI_CAT_BOOKS;
	else if (!strcmp(word, "GUTENBERG"))
		return WIKI_CAT_GUTENBERG;
	else if (!strcmp(word, "OTHERS"))
		return WIKI_CAT_OTHERS;
	else
		return WIKI_CAT_INVALID;
}

static KEYBOARD_MODE get_keyboard_info(char *word)
{
	if (!word[0] || !strcmp(word, "ENGLISH") || !strcmp(word, "CHINESE-PINYIN"))
		return KEYBOARD_CHAR;
	else if (!strcmp(word, "JAPANESE-HIRAGANA"))
		return KEYBOARD_PHONE_STYLE_JP;
	else if (!strcmp(word, "JAPANESE-ROMAN"))
		return KEYBOARD_CHAR_JP;
	else if (!strcmp(word, "KOREAN"))
		return KEYBOARD_CHAR_KO;
	else if (!strcmp(word, "DANISH"))
		return KEYBOARD_CHAR_DA;
	else if (!strcmp(word, "CHINESE-BOPOMO"))
		return KEYBOARD_PHONE_STYLE_TW;
	else
		return KEYBOARD_NONE;
}

static int assign_wiki_id(char *word)
{
	unsigned int i;
	int max_wiki_id = 0;

	for (i = 0; i < nWikiList; i++)
	{
		if (!strcmp(word, wiki_list[i].wiki_folder))
			return wiki_list[i].wiki_id;
		if (max_wiki_id < wiki_list[i].wiki_id)
			max_wiki_id = wiki_list[i].wiki_id;
	}
	return max_wiki_id + 1;
}

static void str_to_upper(char *word)
{
	unsigned int i;
	for (i = 0; i < strlen(word); i++)
		word[i] = toupper(word[i]);
}

static void str_to_lower(char *word)
{
	unsigned int i;
	for (i = 0; i < strlen(word); i++)
		word[i] = tolower(word[i]);
}

static bool get_next_token(char *word, char *buf)
{
	static char *last = NULL;

	if (buf)
		last = buf;
	if (word && last && *last)
	{
		while (last && *last && *last != ',')
			*word++ = *last++;
		if (*last == ',')
			last++;
		*word = '\0';
		return true;
	}
	return false;
}

void init_wiki_info(void)
{
	unsigned int i, j;
	int fd;
	unsigned char *p;
	int nWikiSerialId;
	char line[MAX_LINE_SIZE], buf[MAX_LINE_SIZE], word[MAX_LINE_SIZE];
	ssize_t nLineChars;

	nWikiCount = 0;
	fd = file_open("wiki.inf", FILE_OPEN_READ);
	if (fd >= 0)
	{
		unsigned int nTempWikiList = 0;

		nLineChars = file_read(fd, line, MAX_LINE_SIZE);
		while (nLineChars > 0)
		{
			nLineChars = copy_line(buf, line, nLineChars);
			trim_leading_spaces(buf);
			if (buf[0] && buf[0] != '#')
				nTempWikiList++;
			nLineChars += file_read(fd, &line[nLineChars], MAX_LINE_SIZE - nLineChars);
		}

		if (nTempWikiList > 0)
		{
			if (nTempWikiList > sizeof(wiki_list) / sizeof(WIKI_LIST))
				wiki_list = (WIKI_LIST *)memory_allocate(sizeof(WIKI_LIST) * nTempWikiList, "wiki_list");
			else
				wiki_list = wiki_list_default;
			file_lseek(fd, 0);
			nLineChars = file_read(fd, line, MAX_LINE_SIZE);
			while (nLineChars > 0)
			{
				nLineChars = copy_line(buf, line, nLineChars);
				trim_leading_spaces(buf);
				if (buf[0] && buf[0] != '#')
				{
					int state = 0;

					memset(&wiki_list[nWikiList], 0, sizeof(WIKI_LIST));
					wiki_list[nWikiList].wiki_default_keyboard = KEYBOARD_CHAR; // default keyboard
					get_next_token(NULL, buf); // initialize get_next_token()
					while (get_next_token(word, NULL))
					{
						trim_leading_spaces(word);
						trim_trailing_spaces(word);
						switch (state)
						{
						case 0:
							str_to_upper(word);
							wiki_list[nWikiList].wiki_cat = get_category_info(word);
							if (wiki_list[nWikiList].wiki_cat != WIKI_CAT_INVALID)
								state++;
							else
								state = 99;
							break;
						case 1:
							strncpy(wiki_list[nWikiList].wiki_lang, word, sizeof(wiki_list[nWikiList].wiki_lang) - 1);
							wiki_list[nWikiList].wiki_lang[sizeof(wiki_list[nWikiList].wiki_lang) - 1] = '\0';
							if (wiki_list[nWikiList].wiki_lang[0])
								state++;
							else
								state = 99;
							break;
						case 2:
							str_to_lower(word);
							strncpy(wiki_list[nWikiList].wiki_folder, word, sizeof(wiki_list[nWikiList].wiki_folder) - 1);
							wiki_list[nWikiList].wiki_folder[sizeof(wiki_list[nWikiList].wiki_folder) - 1] = '\0';
							if (wiki_list[nWikiList].wiki_folder[0])
							{
								wiki_list[nWikiList].wiki_id = assign_wiki_id(word);
								state++;
							}
							else
								state = 99;
							break;
						case 3:
							str_to_upper(word);
							wiki_list[nWikiList].wiki_default_keyboard = get_keyboard_info(word);
							if (wiki_list[nWikiList].wiki_default_keyboard != KEYBOARD_NONE)
								state++;
							else
								state = 99;
							break;
						case 4:
							wiki_list[nWikiList].wiki_nls_idx = atoi(word);
							state++;
							break;
						default:
							break;
						}
					}
					if (wiki_list[nWikiList].wiki_cat != WIKI_CAT_INVALID &&
					    wiki_list[nWikiList].wiki_lang[0] &&
					    wiki_list[nWikiList].wiki_folder[0] &&
					    wiki_list[nWikiList].wiki_default_keyboard != KEYBOARD_NONE)
					{
						wiki_list[nWikiList].wiki_serial_id = nWikiList + 1;
						nWikiList++;
					}
				}
				nLineChars += file_read(fd, &line[nLineChars], MAX_LINE_SIZE - nLineChars);
			}
		}
	}
	if (!nWikiList)
	{
		wiki_list = wiki_list_default;
		nWikiList = sizeof(wiki_list_default) / sizeof(WIKI_LIST);
	}
	baWikiActive = (bool *)memory_allocate(sizeof(bool) * nWikiList, "baWikiActive");
	memset(baWikiActive, 0, sizeof(bool) * nWikiList);
	for (i = 0; i < nWikiList; i++)
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
		for (i = 0; i < nWikiList; i++)
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
					bShowPositioner = atoi((const char *)p);
				p = get_nls_key_value("wiki_id", pWikiIni, lenWikiIni, 0);
				if (*p)
				{
					nWikiSerialId = atoi((const char *)p);
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
		if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ja"))
			bWikiIsJapanese = true;
		else
			bWikiIsJapanese = false;
		if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "zht")) // Simplified Chinese Pinyin IME does not need special conversion
			bWikiIsTC = true;
		else
			bWikiIsTC = false;
		if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ko"))
			bWikiIsKorean = true;
		else
			bWikiIsKorean = false;
		if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "da"))
			bWikiIsDanish = true;
		else
			bWikiIsDanish = false;
		default_keyboard = wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_default_keyboard;
		keyboard_set_mode(default_keyboard);
	} else {
		fatal_error("No wiki found");
	}
}

int get_wiki_idx_by_lang_link(const unsigned char *lang_link_str)
{
	int len;
	unsigned char *p, *q;
	unsigned int i;
	int current_wiki_cat = -1;

	p = ustrchr(lang_link_str, ':');
	q = ustrchr(lang_link_str, '#');
	if (!p || (q && q < p))
		p = q;

	if (p)
	{
		int wiki_idx = get_wiki_idx_from_id(current_article_wiki_id);
		current_wiki_cat = wiki_list[aActiveWikis[wiki_idx].WikiInfoIdx].wiki_cat;
		len = p - lang_link_str;
		for (i = 0; i < nWikiCount; i++)
		{
			if (current_wiki_cat == (int)wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_cat &&
			    !ustrncmp(lang_link_str, wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_lang, len))
				return i;
		}
	}
	return -1;
}

bool wiki_lang_exist(const unsigned char *lang_link_str)
{
	return get_wiki_idx_by_lang_link(lang_link_str) >= 0;
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


uint32_t wiki_lang_link_search(const unsigned char *lang_link_str)
{
	uint32_t article_idx = 0;
	int nTempCurrentWiki = nCurrentWiki;
	unsigned char *p, *q;

	search_interrupted = 0;
	if ((nCurrentWiki = get_wiki_idx_by_lang_link(lang_link_str)) >= 0)
	{
		reset_search_info(nCurrentWiki);
		init_search_fnd();
		p = ustrchr(lang_link_str, ':');
		q = ustrchr(lang_link_str, '#');
		if (!p || (q && q < p))
			p = q;
		if (p)
		{
			if (*p == '#') // actual title is different than title for search
			{
				q = ustrchr(p + 1, CHAR_LANGUAGE_LINK_TITLE_DELIMITER); // locate the actual title
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
	unsigned int i;

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
	unsigned int i;

	for (i = 0; i < nWikiCount; i++)
	{
		if (wiki_list[aActiveWikis[i].WikiInfoIdx].wiki_serial_id == wiki_serial_id)
			return i;
	}
	return -1;
}

int get_wiki_id_from_idx(unsigned int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aActiveWikis[wiki_idx].WikiInfoIdx].wiki_id;
	}
	return 0;
}

int get_wiki_serial_id_from_idx(unsigned int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aActiveWikis[wiki_idx].WikiInfoIdx].wiki_serial_id;
	}
	return 0;
}

unsigned char *get_nls_key_value(const char *key, unsigned char *key_pairs, long key_pairs_len, int wiki_nls_idx)
{
	int i, j;
	int key_len;
	int bFound = 0;
	char local_key[64];

	if (wiki_nls_idx)
		sprintf(local_key, "%s%d", key, wiki_nls_idx); // try the key name with wiki_name_idx first
	else
		ustrcpy(local_key, key);

	key_len = ustrlen(local_key);
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
		return (unsigned char *)""; // *** VERY BAD: removes const
}

const unsigned char *get_nls_text(const char *key)
{
	int fd;
	unsigned long nSize;
	unsigned char *p;

	if (nCurrentWiki < 0)
		return (const unsigned char *)"";
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
			return (const unsigned char *)"";

		return get_nls_key_value(key, aActiveWikis[nCurrentWiki].WikiNls, aActiveWikis[nCurrentWiki].WikiNlsLen, wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_nls_idx);
	}
}

const unsigned char *get_lang_link_display_text(const unsigned char *lang_link_str)
{
	int nTempCurrentWiki = nCurrentWiki;
	static unsigned char lang_str[3];
	const unsigned char *p = NULL;

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

const unsigned char *get_wiki_name(int idx)
{
	int nTempCurrentWiki = nCurrentWiki;
	const unsigned char *pName;

	nCurrentWiki = idx;
	pName = get_nls_text("wiki_name");
	nCurrentWiki = nTempCurrentWiki;
	return pName;
}

void wiki_ini_insert_keypair(char *key, char *keyval)
{
	unsigned char *p = get_nls_key_value(key, pWikiIni, lenWikiIni, 0);

	if (*p)
	{
		if (ustrlen(p) < ustrlen(keyval))
		{
			int diff = ustrlen(keyval) - ustrlen(p);
			int move_size;

			if (lenWikiIni + diff < sizeWikiIni)
			{
				move_size = lenWikiIni - (p - pWikiIni);
				if (move_size > 0)
					memmove(p + diff, p, move_size);
			}
			lenWikiIni += diff;
		}
		if (p + ustrlen(keyval) < pWikiIni + sizeWikiIni - 1)
		{
			memset(p, 0, ustrlen(keyval) + 1);
			memcpy(p, keyval, ustrlen(keyval));
		}
	}
	else if (lenWikiIni + ustrlen(key) + ustrlen(keyval) + 2 < sizeWikiIni)
	{
		if (lenWikiIni)
			pWikiIni[lenWikiIni++] = '\0'; // make sure the new key pair start with a new line
		memcpy(&pWikiIni[lenWikiIni], key, ustrlen(key));
		lenWikiIni += ustrlen(key);
		pWikiIni[lenWikiIni++] = '=';
		memcpy(&pWikiIni[lenWikiIni], keyval, ustrlen(keyval));
		lenWikiIni += ustrlen(keyval);
		pWikiIni[lenWikiIni] = '\0';
	}
}

void set_wiki(int idx)
{
	int fd;
	char sWikiId[10];
	unsigned int i;
	char prev_c = 0;

	nCurrentWiki = idx;
	reset_search_info(nCurrentWiki);
	if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ja"))
		bWikiIsJapanese = true;
	else
		bWikiIsJapanese = false;
	if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "zht"))
		bWikiIsTC = true;
	else
		bWikiIsTC = false;
	if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "ko"))
		bWikiIsKorean = true;
	else
		bWikiIsKorean = false;
	if (!ustrcmp(wiki_list[aActiveWikis[nCurrentWiki].WikiInfoIdx].wiki_lang, "da"))
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

void nls_replace_text(const unsigned char *replace_str, unsigned char *out_str)
{
	if (!ustrcmp(replace_str, "title"))
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
		ustrcpy(out_str, replace_str);
	}
}

WIKI_LICENSE_DRAW *wiki_license_draw()
{
	int wiki_idx = get_wiki_idx_from_id(current_article_wiki_id);
	int nTempCurrentWiki = nCurrentWiki;
	int y = 0;
	int x = 0;
	unsigned char draw_buf[MAX_LICENSE_TEXT_PIXEL_LINES * LCD_BUF_WIDTH_BYTES];
	const unsigned char *pLicenseText;
	unsigned char footer_buffer[40];
	unsigned char *footer = footer_buffer;
	unsigned char sLicenseTextSegment[MAX_LICENSE_TEXT_LEN];
	const unsigned char *pLicenseTextSegment;
	int line_height = pcfFonts[LICENSE_TEXT_FONT - 1].Fmetrics.linespace;
	unsigned char str[256];  // possible buffer overflow
	const unsigned char *p, *q;
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
				if ((p = ustrchr(pLicenseText, LICENSE_LINK_END)))
				{
					memcpy(sLicenseTextSegment, pLicenseText, p - pLicenseText);
					sLicenseTextSegment[p - pLicenseText] = '\0';
					pLicenseText = p + 1;
				}
				else
				{
					ustrcpy(sLicenseTextSegment, pLicenseText);
					pLicenseText += ustrlen(pLicenseText);
				}
			}
			else if (pLicenseText[0] == NLS_TEXT_REPLACEMENT_START)
			{
				bInLink = 0;
				pLicenseText++;
				if ((p = ustrchr(pLicenseText, NLS_TEXT_REPLACEMENT_END)))
				{
					memcpy(str, pLicenseText, p - pLicenseText);
					str[p - pLicenseText] = '\0';
					pLicenseText = p + 1;
				}
				else
				{
					ustrcpy(str, pLicenseText);
					pLicenseText += ustrlen(pLicenseText);
				}
				nls_replace_text(str, sLicenseTextSegment);
			}
			else
			{
				bInLink = 0;
				p = ustrchr(pLicenseText, LICENSE_LINK_START);
				q = ustrchr(pLicenseText, NLS_TEXT_REPLACEMENT_START);
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
					ustrcpy(sLicenseTextSegment, pLicenseText);
					pLicenseText += ustrlen(pLicenseText);
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
						guilib_buffer_set_pixel(draw_buf, i, end_y - 1);
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

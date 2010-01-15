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

WIKI_LIST wiki_list[MAX_WIKIS] =
{
	{1, "enpedia"},
	{2, "espedia"},
	{3, "nlpedia"},
};

int nWikiCount = 0;
int aWikiInfoIdx[MAX_WIKIS_PER_DEVICE]; // index to wiki_info[]
char *aWikiNls[MAX_WIKIS_PER_DEVICE];
long aWikiNlsLen[MAX_WIKIS_PER_DEVICE];
int nCurrentWiki = -1; // index to aWikiInfoIdx[]
int rendered_wiki_selection_count = -1;
int current_article_wiki_id = 0;
WIKI_LICENSE_DRAW aWikiLicenseDraw[MAX_WIKIS_PER_DEVICE];

void init_wiki_info(void)
{
	int i;
	int fd;
	char sFilePath[20];
	unsigned int nSize;
	char *p;
	int nWikiId;

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
		wl_fsize(fd, &nSize);
		p = malloc_simple(nSize + 1, MEM_TAG_INDEX_M1);
		if (p)
		{
			wl_read(fd, p, nSize);
			wl_close(fd);
			p[nSize] = '\0';
			nWikiId = atoi(p);
			if (nWikiId > 0)
				nCurrentWiki = get_wiki_idx_from_id(nWikiId);
			if (nCurrentWiki < 0)
				nCurrentWiki = 0;
			free_simple(p, MEM_TAG_INDEX_M1);
		}
		for (i = 0; i < nWikiCount; i++)
		{
			aWikiNlsLen[i] = -1;
			aWikiLicenseDraw[i].lines = 0;
		}
	}
	else
		fatal_error("No wiki found");
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

int get_wiki_id_from_idx(int wiki_idx)
{
	if (wiki_idx < nWikiCount)
	{
		return wiki_list[aWikiInfoIdx[wiki_idx]].wiki_id;
	}
	return 0;
}

unsigned char *get_nls_key_value(char *key, char *key_pairs, long key_pairs_len)
{
	int i, j;
	int key_len = strlen(key);
	int bFound = 0;

	i = 0;
	while (i < key_pairs_len - key_len - 1 && !bFound)
	{
		for (j = 0; j < key_len; j++)
		{
			if (key[j] != key_pairs[i + j])
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
	else
		return "";
}

unsigned char *get_nls_text(char *key)
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

		return get_nls_key_value(key, aWikiNls[nCurrentWiki], aWikiNlsLen[nCurrentWiki]);
	}
}

void wiki_selection(void)
{
	rendered_wiki_selection_count = 0;
	render_wiki_selection_with_pcf();
}

char *get_wiki_name(int idx)
{
	int nTmpeCurrentWiki = nCurrentWiki;
	char *pName;

	nCurrentWiki = idx;
	pName = get_nls_text("wiki_name");
	nCurrentWiki = nTmpeCurrentWiki;
	return pName;
}

void set_wiki(int idx)
{
	int fd;
	char sWikiId[10];

	nCurrentWiki = idx;
	fd = wl_open("wiki.ini", WL_O_CREATE);
	if (fd >= 0)
	{
		sprintf(sWikiId, "%d", get_wiki_id_from_idx(nCurrentWiki));
		wl_write(fd, (void *)sWikiId, strlen(sWikiId));
		wl_close(fd);
	}
}

WIKI_LICENSE_DRAW *wiki_license_draw()
{
	int wiki_idx = get_wiki_idx_from_id(current_article_wiki_id);

	if (wiki_idx < 0)
		wiki_idx = nCurrentWiki;
	if (!aWikiLicenseDraw[wiki_idx].lines)
	{
		int y = 0;
		int x = 0;
		char draw_buf[MAX_LICENSE_TEXT_PIXEL_LINES * LCD_BUF_WIDTH_BYTES];
		unsigned char *pLicenseText = get_nls_text("license_text");
		char sLicenseTextSegment[MAX_LICENSE_TEXT_LEN];
		unsigned char *pLicenseTextSegment;
		int line_height = pcfFonts[LICENSE_TEXT_FONT - 1].Fmetrics.linespace;
		char str[256];
		unsigned char *p;
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
				else
				{
					bInLink = 0;
					if ((p = strchr(pLicenseText, LICENSE_LINK_START)))
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
				p = str;
				buf_draw_UTF8_str_in_copy_buffer(draw_buf, &p, x, LCD_BUF_WIDTH_PIXELS,
								 y, y + line_height - 1, LCD_LEFT_MARGIN, LICENSE_TEXT_FONT);
				if (bInLink && aWikiLicenseDraw[wiki_idx].link_count < MAX_LINKS_IN_LICENSE_TEXT)
				{
					start_x = x;
					start_y = y + 1;
					end_x = x + width;
					end_y = y + line_height;
					aWikiLicenseDraw[wiki_idx].links[aWikiLicenseDraw[wiki_idx].link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
					aWikiLicenseDraw[wiki_idx].links[aWikiLicenseDraw[wiki_idx].link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
					aWikiLicenseDraw[wiki_idx].links[aWikiLicenseDraw[wiki_idx].link_count++].article_id = nLinkArticleId;
					for(i = start_x + LCD_LEFT_MARGIN; i < end_x + LCD_LEFT_MARGIN; i++)
					{
						lcd_set_pixel(draw_buf, i, end_y + 1);
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
		}
		if (x)
			y += line_height;
		y += SPACE_AFTER_LICENSE_TEXT;
		aWikiLicenseDraw[wiki_idx].lines = y;
		aWikiLicenseDraw[wiki_idx].buf = malloc_simple(y * LCD_BUF_WIDTH_BYTES, MEM_TAG_INDEX_M1);
		memcpy(aWikiLicenseDraw[wiki_idx].buf, draw_buf, y * LCD_BUF_WIDTH_BYTES);
	}
	return &aWikiLicenseDraw[wiki_idx];
}

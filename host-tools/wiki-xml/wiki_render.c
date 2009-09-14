#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wchar.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>
#include <dirent.h>
#include "bmf.h"
#include "wiki_render.h"
#include "lcd_buf_draw.h"
#include "Alloc.h"
#include "Bra.h"
#include "LzmaEnc.h"
#include "bigram.h"
#include "search_hash.h"

#define MAX_LOCAL_TEXT_BUF 512000
#define LIST_TYPE_ORDERED 1
#define LIST_TYPE_UNORDERED 2
#define LIST_TYPE_DEFINITION 3
#define PIXELS_PER_INDENT 20
#define PIXELS_FIRST_INDENT 10
#define MAX_LIST_DEPTH 10
#define LIST_LI 1
#define LIST_DT 2
#define LIST_DD 3
struct render_buf {
	int nCurrentLineIdx;
	int nCurrentFontIdx;
	int nCurrentRenderType;
	int nCurrentLineHeight;
	long nLines;
	long current_x;
	long current_y;
	int current_indent;
	int list_depth;
	char list_type[MAX_LIST_DEPTH];
	int ol_count[MAX_LIST_DEPTH];
	int nLinks;
} render_buf;
char render_buf_sLines[64*1024][256];

#define MAX_LINKS 10240
#define MAX_LINK_STRING 256
#define MAX_CLASS_STRING 2048
#define MAX_CELL_STRING 4096
struct link {
	long start_x;
	long start_y;
	long end_x;
	long end_y;
	int bWikiLink;
	char sLink[MAX_LINK_STRING];
} links[MAX_LINKS];

char word_break_before_chars[] = "\n\r ";
char word_break_after_chars[] = "\n\r )*+,-./:;<=>?]|}";
#define WORD_BREAK_FLEX_CHAR_MASK1 0xF3000
#define WORD_BREAK_FLEX_CHAR_MASK2 0xF4000
#define WORD_BREAK_FLEX_CHAR_MASK3 0xF8000

#define RENDER_TYPE_NORMAL 0
#define RENDER_TYPE_LINK 1
#define RENDER_TYPE_TABLE_DATA 2

#define MAX_TAG_LEN 256
#define WIKI_TAG_PAIR_TEMPLATE 		0 
#define WIKI_TAG_PAIR_LINK 		1 
#define WIKI_TAG_PAIR_TABLE 		2 
#define WIKI_TAG_PAIR_TABLE_CAPTION	3 
#define WIKI_TAG_PAIR_TABLE_ROW		4 
#define WIKI_TAG_PAIR_TABLE_CELLS	5 
#define WIKI_TAG_PAIR_TABLE_HEADER	6 
#define WIKI_TAG_PAIR_EXTERNAL_LINK	7 
#define WIKI_TAG_PAIR_H6		8 
#define WIKI_TAG_PAIR_H5		9 
#define WIKI_TAG_PAIR_H4		10
#define WIKI_TAG_PAIR_H3		11
#define WIKI_TAG_PAIR_H2		12
#define WIKI_TAG_PAIR_BOLD_ITALIC	13
#define WIKI_TAG_PAIR_BOLD		14
#define WIKI_TAG_PAIR_ITALIC		15
#define WIKI_TAG_PAIR_NOWIKI		16
#define WIKI_TAG_PAIR_PRE		17
#define WIKI_TAG_PAIR_PRE_LINE		18
#define WIKI_TAG_PAIR_SEP		19
#define WIKI_TAG_PAIR_OL		20
#define WIKI_TAG_PAIR_UL		21
#define WIKI_TAG_PAIR_DT		22
#define WIKI_TAG_PAIR_DD		23
#define WIKI_TAG_PAIR_REF		24
#define WIKI_TAG_PAIR_COMMENT		25
#define WIKI_TAG_PAIR_TEXT		26
#define MAX_WIKI_TAG_PAIRS WIKI_TAG_PAIR_TEXT
struct wiki_tag_pair {
	char sTagStart[MAX_TAG_LEN];
	char sTagEnd[MAX_TAG_LEN];
	int lenTagStart;
	int lenTagEnd;
	int bGotChild;
	int bBeginOfLine;
} wiki_tag_pairs[MAX_WIKI_TAG_PAIRS] = {
	{"{{", "}}", 2, 2, 0, 0}, 			// template
	{"[[", "]", 2, 1, 1, 0}, 			// link
	{"{|", "|}", 2, 2, 1, 1}, 			// table
	{"|+", "\n", 2, 1, 1, 1},			// table caption
	{"|-", "\n", 2, 1, 1, 1},			// row
	{"|", "\n", 1, 1, 1, 1}, 				// row cells
	{"!", "\n", 1, 1, 1, 1},				// column or row header
	{"[", "]", 1, 1, 0, 0},				// external link
	{"======", "======", 6, 6, 0, 1},		// H6
	{"=====", "=====", 5, 5, 0, 1}, 		// H5
	{"====", "====", 4, 4, 0, 1},			// H4
	{"===", "===", 3, 3, 0, 1}, 			// H3
	{"==", "==", 2, 2, 0, 1},			// H2
	{"\'\'\'\'\'", "\'\'\'\'\'", 5, 5, 1, 0}, 	// bold italic
	{"\'\'\'", "\'\'\'", 3, 3, 1, 0},		// bold
	{"\'\'", "\'\'", 2, 2, 1, 0}, 			// italic
	{"&lt;nowiki&gt;", "&lt;/nowiki&gt;", 14, 15, 0, 0}, // nowiki
	{"&lt;pre&gt;", "&lt;/pre&gt;", 11, 12, 0, 0}, 	// preserve format
	{" ", "\n", 1, 1, 1, 1},			// line with preserved format
	{"#####", "\n", 5, 1, 1, 1},			// separator line
	{"#", "\n", 1, 1, 1, 1},			// ordered list
	{"*", "\n", 1, 1, 1, 1},			// unordered list
	{";", "\n", 1, 1, 1, 1},			// definition term
	{":", "\n", 1, 1, 1, 1},			// definition
	{"&lt;ref", "&lt;/ref&gt;", 7, 12, 0, 0}, 	// reference (to be filtered out)
	{"&lt;!--", "--&gt;", 7, 6, 0, 0}		// comment
};

#define MAX_WIKI_NODES 160000
#define MAX_WIKI_TAG_STACK 128

struct wiki_node { // one node for either start tag, end tag or content (string between tags)
	int idxTag;
	int bTagStart;
	char *pTag; // only meanful for content (string between tags)
	int lenTag;
} *wiki_nodes;

int nWikiNodeCount = 0;

#define TAG_PAIR_STRONG		0 
#define TAG_PAIR_SCRIPT		1 
#define TAG_PAIR_DIV		2 
#define TAG_PAIR_SPAN		3 
#define TAG_PAIR_REFERENCE      4 
#define TAG_PAIR_BR             5 
#define TAG_PAIR_BR2            6 
#define TAG_PAIR_H1		7 
#define TAG_PAIR_H2		8 
#define TAG_PAIR_H3		9 
#define TAG_PAIR_H4		10
#define TAG_PAIR_H5		11
#define TAG_PAIR_H6		12
#define TAG_PAIR_A		13
#define TAG_PAIR_BIG_BIG_BIG	14
#define TAG_PAIR_BIG_BIG	15
#define TAG_PAIR_BIG		16
#define TAG_PAIR_BI		17
#define TAG_PAIR_B		18
#define TAG_PAIR_I		19
#define TAG_PAIR_DEL		20
#define TAG_PAIR_INS		21
#define TAG_PAIR_LI		22
#define TAG_PAIR_OL		23
#define TAG_PAIR_EOL		24
#define TAG_PAIR_DL		25
#define TAG_PAIR_EDL		26
#define TAG_PAIR_DT		27
#define TAG_PAIR_DD		28
#define TAG_PAIR_P_END          29
#define TAG_PAIR_P		30
#define TAG_PAIR_REF		31
#define TAG_PAIR_SUP		32
#define TAG_PAIR_TABLE		33
#define TAG_PAIR_TD		34
#define TAG_PAIR_TH		35
#define TAG_PAIR_TR		36
#define TAG_PAIR_UL		37
#define TAG_PAIR_EUL		38
#define TAG_PAIR_HTTP		39
#define TAG_PAIR_UNKNOWN	40
#define MAX_TAG_PAIRS TAG_PAIR_UNKNOWN
struct tag_pair {
	char sTagStart[MAX_TAG_LEN];
	char sTagEnd[MAX_TAG_LEN];
	int lenTagStart;
	int lenTagEnd;
	int bGotChild;
} tag_pairs[MAX_TAG_PAIRS] = {
	{"<strong", "</strong>", 7, 9, 0},
	{"<script", "</script>", 7, 9, 0},
	{"<div", "</div>", 4, 6, 1},
	{"<span", "</span>", 5, 7, 1},
	{"<reference/", "", 11, 0, 1},
	{"<br", "",  3, 0, 0},
	{"<br/", "", 4, 0, 0},
	{"<h1", "</h1>", 3, 5, 1},		// Header 2
	{"<h2", "</h2>", 3, 5, 1},		// Header 2
	{"<h3", "</h3>", 3, 5, 1},		// Header 3
	{"<h4", "</h4>", 3, 5, 1},		// Header 4
	{"<h5", "</h5>", 3, 5, 1},		// Header 5
	{"<h6", "</h6>", 3, 5, 1},		// Header 6
	{"<a", "</a>", 2, 4, 1},		// <a> </a>
	{"<big><big><big", "</big></big></big>", 14, 18, 1},// big big big
	{"<big><big", "</big></big>", 9, 12, 1},// big big
	{"<big", "</big>", 4, 6, 1},		// big
	{"<b><i", "</i></b>", 5, 8, 1},		// bold
	{"<b", "</b>", 2, 4, 1},		// bold
	{"<i", "</i>", 2, 4, 1},		// italic
	{"<del", "</del>", 4, 6, 1},		// deleted text (striked out)
	{"<ins", "</ins>", 4, 6, 1},		// inserted text (underlined)
	{"<li", "</li>", 3, 5, 1},		// list item
	{"<ol", "", 3, 0, 0},			// order list start
	{"</ol", "", 4, 0, 0},			// order list end
	{"<dl", "", 3, 0, 0},			// definition list start
	{"</dl", "", 4, 0, 0},			// definition list end
	{"<dt", "</dt>", 3, 5, 1},		// definition term
	{"<dd", "</dd>", 3, 5, 1},		// definition
	{"<p/", "", 3, 0, 0},
	{"<p", "</p>", 2, 4, 1},		// <p>
	{"<ref", "</ref>", 4, 6, 1},		// <ref> </ref>
	{"<sup", "</sup>", 4, 6, 1},		// <sup>
	{"<table", "</table>", 6, 8, 1},	// table
	{"<td", "</td>", 3, 5, 1},		// table data
	{"<th", "</th>", 3, 5, 1},		// table header
	{"<tr", "</tr>", 3, 5, 1},		// table row
	{"<ul", "", 3, 0, 0},			// unorder list start
	{"</ul", "", 4, 0, 0},			// unorder list end
	{"<http:", "", 6, 0, 0}
};

#define MAX_ARTICLE_NODES 80000

struct article_node {
	int idxTag;
	char *pTagDesc;
	int lenTagDesc;
	char *pContent;
	int len;
	int idxChildNode;
	int idxNextNode;
} *article_nodes;

int nArticleNodeCount = 0;

//#define HEAP_ALLOC(var,size) \
//    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
//static HEAP_ALLOC(wrkmem,LZO1X_999_MEM_COMPRESS);

static void *SzAlloc(void *p, size_t size) { p = p; return malloc(size); }
static void SzFree(void *p, void *address) { p = p; free(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };
extern int nMsgLevel;
long process_wiki_tag(int *idxNode, int maxNode, char *sText, char *sBuf, long lenBuf, long maxLenBuf, int bInList);

void processing_speed(long nCount)
{
	static time_t base_t;
	time_t current_t;
	double elapsed_seconds;
	long nCountPerSecond;

	if (nCount == 0)
	{
		time(&base_t);
	}
	else
	{
		time(&current_t);
		elapsed_seconds = difftime(current_t, base_t);
		if (elapsed_seconds > 1)
		{
			nCountPerSecond = (long)(nCount / elapsed_seconds);
			showMsg(0, "%ld titles processed in %ld seconds, %ld titles per second\n", nCount, (long)elapsed_seconds, nCountPerSecond);
		}
	}
}

char *strnchr(char *s, char c, int len)
{
	int bFound = 0;

	while (!bFound && len > 0)
	{
		if (*s == c)
			bFound = 1;
		else
		{
			s++;
			len--;
		}
	}

	if (bFound)
		return s;
	else
		return NULL;
}

char *strnstr(char *s1, char *s2, int len)
{
	int bFound = 0;
	int s2_len = strlen(s2);

	while (!bFound && len >= s2_len)
	{
		if (!memcmp(s1, s2, s2_len))
			bFound = 1;
		else
		{
			s1++;
			len--;
		}
	}

	if (bFound)
		return s1;
	else
		return NULL;
}

long srting_len_fit_width(int idxFont, long *xCurrent, int nWidthLimit, char *pContent, long lenContent)
{
	int lenLastBreak;
	int lenCurrent;
	int bIsBreakBeforeChar;
	int bIsBreakAfterChar;
	int nCharBytes;
	int xCurrentLocal;
	int bNewLine = 0;

	if (*xCurrent <= render_buf.current_indent)
		bNewLine = 1;
	lenLastBreak = 0;
	lenCurrent = 0;
	xCurrentLocal = (int)*xCurrent;
	if (xCurrentLocal < 0)
		xCurrentLocal = 0;
	while (xCurrentLocal < nWidthLimit && lenContent > 0 && *pContent != '\n')
	{
		bIsBreakBeforeChar = strchr(word_break_before_chars, *pContent) ||
			(*pContent & WORD_BREAK_FLEX_CHAR_MASK1) ||
			(*pContent & WORD_BREAK_FLEX_CHAR_MASK2) ||
			(*pContent & WORD_BREAK_FLEX_CHAR_MASK3);
		bIsBreakAfterChar = strchr(word_break_after_chars, *pContent) ||
			(*pContent & WORD_BREAK_FLEX_CHAR_MASK1) ||
			(*pContent & WORD_BREAK_FLEX_CHAR_MASK2) ||
			(*pContent & WORD_BREAK_FLEX_CHAR_MASK3);
		if (bIsBreakBeforeChar)
		{
			lenLastBreak = 	lenCurrent;
			*xCurrent = xCurrentLocal;
		}
		xCurrentLocal += get_UTF8_char_width(idxFont, &pContent, &lenContent, &nCharBytes);
		if (xCurrentLocal < nWidthLimit)
		{
			lenCurrent += nCharBytes;
			if (bIsBreakAfterChar)
			{
				lenLastBreak = lenCurrent;
				*xCurrent = xCurrentLocal;
			}
		}
		else
		{
			if (!lenLastBreak)
			{
				if (!bNewLine)
					return 0;  // if not beginning of line, move to the next line
				lenLastBreak = lenCurrent;
				*xCurrent = xCurrentLocal;
				// if no last break point, just return lenCurrent
			}
			else
			{
				lenContent = lenLastBreak;
			}
		}
	}

	if (!lenContent || !lenLastBreak) // if all string is comsumed or no break point
	{
		*xCurrent = xCurrentLocal;
		return lenCurrent;
	}
	else
		return lenLastBreak;
}

void render_title(char *pTitle, long nTitleLen)
{
	long nTitleLenFitWidth;
	int nLineHeight;
	int i;

	for (i=0; i < nTitleLen; i++)
	{
		if (pTitle[i] == '~')
			pTitle[i] = ':';
		else if (pTitle[i] == '_')
			pTitle[i] = ' ';
	}

	nLineHeight = pcfFonts[TITLE_FONT_IDX - 1].Fmetrics.linespace;
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	render_buf.nCurrentLineIdx = 0;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 5;
	render_buf.current_y += 5;
	while (nTitleLen > 0)
	{
		render_buf.nLines++;
		memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
		render_buf.nCurrentLineIdx = 0;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_3_NEW_LINE_WITH_FONT;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)(TITLE_FONT_IDX | (nLineHeight << 3));
		nTitleLenFitWidth = srting_len_fit_width(TITLE_FONT_IDX, &render_buf.current_x, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, pTitle, nTitleLen);
		strncpy(&render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] , pTitle, nTitleLenFitWidth);
		render_buf.nCurrentLineIdx += nTitleLenFitWidth;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
		nTitleLen -= nTitleLenFitWidth;
		pTitle += nTitleLenFitWidth;
		render_buf.current_y += nLineHeight;
		render_buf.current_x = -1;
	}
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	render_buf.nCurrentLineIdx = 0;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 6;
//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_12_FULL_HORIZONTAL_LINE;
//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 4;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
	render_buf.current_y += 7;
	render_buf.current_x = -1;
	render_buf.nCurrentLineIdx = 0;
	render_buf.nLines++; // starting with a new line
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
}

void get_tagend_str(char *pTagBuf, char **pText, int *lenText)
{
	char c;
	int bDone = 0;
	
	if (*lenText > 0)
	{
		(*pText)++; // skip <
		(*lenText)--;
		*pTagBuf++ = '<';
		*pTagBuf++ = '/';
	}
	
	while (!bDone && *lenText > 0)
	{
		c = **pText;
		if (c == '>' || c == ' ' || c == '\r' || c == '\n' || c == '\t')
		{
			bDone = 1;
		}
		else
		{
			*pTagBuf++ = c;
			(*pText)++; 
			(*lenText)--;
		}
	}
	*pTagBuf++ = '>';
	*pTagBuf = '\0';
}

int find_next_tag(char *pText, int lenText, int *lenBeforeTag, char **pTagDesc, int *lenTagDesc,
	char **pTagContent, int *lenTagContent, char **pAfterTag, int *lenAfterTag)
{
	int i;
	int bFound;
	int idxTag;
	char *p;
	char *pTagEnd;
	char bufTagEnd[MAX_TAG_LEN];

	bFound = 0;
	idxTag = -1;
	*lenBeforeTag = 0;
	*lenTagDesc = 0;
	*lenTagContent = 0;
	*lenAfterTag = 0;
	while (lenText > 0 && !bFound)
	{
		p = strnchr(pText, '<', lenText);
		while (p && lenText > 1 && (p[1] == ' ' || p[1] == '\n'))
			p = strnchr(p + 1, '<', lenText - (p - pText) - 1);
		if (p)
		{
			*lenBeforeTag = p - pText;
			lenText -= *lenBeforeTag;
			pText = p;
			for (i=0; i < MAX_TAG_PAIRS && !bFound; i++)
			{
				if (!strncmp(pText, tag_pairs[i].sTagStart, tag_pairs[i].lenTagStart) &&
					(pText[tag_pairs[i].lenTagStart] == '>' || pText[tag_pairs[i].lenTagStart] == ' ' ||
					pText[tag_pairs[i].lenTagStart] == '\r' || pText[tag_pairs[i].lenTagStart] == '\n' ||
					pText[tag_pairs[i].lenTagStart] == '\t' || pText[tag_pairs[i].lenTagStart] == '/'))
				{
					bFound = 1;
					idxTag = i;
					pText += tag_pairs[i].lenTagStart;
					lenText -= tag_pairs[i].lenTagStart;
					if (tag_pairs[idxTag].lenTagEnd)
						pTagEnd = tag_pairs[idxTag].sTagEnd;
					else
					{
						bufTagEnd[0] = '\0';
						pTagEnd = bufTagEnd;
					}
				}
			}
			if (!bFound)
			{
				bFound = 1;
				idxTag = TAG_PAIR_UNKNOWN;
				if (*(pText + 1) == '/')
					bufTagEnd[0] = '\0';
				else
					get_tagend_str(bufTagEnd, &pText, &lenText); // build end tag and move pText to the end of start tag
				pTagEnd = bufTagEnd;
			}

			p = strnchr(pText, '>', lenText);
			if (p)
			{
				if (*(p - 1) == '/')
				{
					bufTagEnd[0] = '\0';
					pTagEnd = bufTagEnd;
				}
				*pTagDesc = pText + 1;
				*lenTagDesc = p - *pTagDesc;
				if (*pTagEnd)
				{
					*pTagContent = p + 1;
					lenText -= *pTagContent - pText;
					p = strnstr(*pTagContent, pTagEnd, lenText);
					if (p)
					{
						*lenTagContent = p - *pTagContent;
						pText = p + strlen(pTagEnd);
						lenText -= p + strnlen(pTagEnd) - *pTagContent;
					}
					else
					{
						*lenTagContent = lenText;
						lenText = 0;
					}
				}
				else
				{
					lenText -= p + 1 - pText;
					pText = p + 1;
				}
			}
			else
			{
				*lenBeforeTag += lenText;
				lenText = 0;
			}
		}
		else
		{
			*lenBeforeTag += lenText;
			lenText = 0;
		}	
	}

	*lenAfterTag = lenText;
	*pAfterTag = pText;
	return idxTag;
}

int build_child_tree(int idxPreviousNode, int idxMyTag, char *pTagDesc, int lenTagDesc, char *pText, int lenText)
{
	int idxChildTag;
	int idxMyNode;
	int idxChildNode;
	int idxPreviousChild;
	int lenBeforeTag;
	char *pTagContent;
	int lenTagContent;
	char *pAfterTag;
	int lenAfterTag;

	idxMyNode = nArticleNodeCount++;
	if (idxMyNode >= MAX_ARTICLE_NODES)
	{
		showMsg(0, "Max article nodes reached\n");
		exit(-1);
	}
	article_nodes[idxMyNode].idxTag = idxMyTag;
	article_nodes[idxMyNode].pTagDesc = pTagDesc;
	article_nodes[idxMyNode].lenTagDesc = lenTagDesc;
	article_nodes[idxMyNode].pContent = pText;
	article_nodes[idxMyNode].len = 0;
	article_nodes[idxMyNode].idxChildNode = 0;
	article_nodes[idxMyNode].idxNextNode = 0;

	if (idxPreviousNode > 0)
	{
		article_nodes[idxPreviousNode].idxNextNode = idxMyNode;
	}

	if (tag_pairs[idxMyTag].bGotChild)
	{
		idxPreviousChild = 0;
		idxChildTag = find_next_tag(pText, lenText, &lenBeforeTag, &pTagDesc, &lenTagDesc, &pTagContent, &lenTagContent, &pAfterTag, &lenAfterTag);
		article_nodes[idxMyNode].len = lenBeforeTag;

		while (idxChildTag >= 0)
		{
			idxChildNode = build_child_tree(idxPreviousChild, idxChildTag, pTagDesc, lenTagDesc, pTagContent, lenTagContent);
			if (!article_nodes[idxMyNode].idxChildNode)
			{
				article_nodes[idxMyNode].idxChildNode = idxChildNode;
			}
			idxPreviousChild = idxChildNode;
			pText = pAfterTag;
			lenText = lenAfterTag;
			idxChildTag = find_next_tag(pText, lenText, &lenBeforeTag, &pTagDesc, &lenTagDesc, &pTagContent, &lenTagContent, &pAfterTag, &lenAfterTag);
			if (lenBeforeTag > 0)
			{
				article_nodes[nArticleNodeCount].idxTag = -1;
				article_nodes[nArticleNodeCount].pTagDesc = NULL;
				article_nodes[nArticleNodeCount].lenTagDesc = 0;
				article_nodes[nArticleNodeCount].pContent = pText;
				article_nodes[nArticleNodeCount].len = lenBeforeTag;
				article_nodes[nArticleNodeCount].idxChildNode = 0;
				article_nodes[nArticleNodeCount].idxNextNode = 0;
				if (idxPreviousChild)
				{
					article_nodes[idxPreviousChild].idxNextNode = nArticleNodeCount;
				}
				else
					article_nodes[idxMyNode].idxNextNode = nArticleNodeCount;
				idxPreviousChild = nArticleNodeCount;
				nArticleNodeCount++;
			}
		}
	}
	else
	{
		article_nodes[idxMyNode].len = lenText;
	}

	return idxMyNode;
}

struct ampersand_char 
{
	char *pIn;
	char *pOut;
} ampersand_chars[] = {
	{"", ""}
};

long replace_ampersand_char_crlf(char *pDest, char *pSrc)
{
	char *pSemicolon;
	ucs4_t u;
	char sUnicode[12];
	long len = 0;
	char *p = pSrc;

	while (*pSrc)
	{
		if (!strncmp(pSrc, "&gt;", 4))
		{
			*pDest++ = '>';
			len++;
			pSrc += 4;
		}
		else if (!strncmp(pSrc, "&lt;", 4))
		{
			*pDest++ = '<';
			len++;
			pSrc += 4;
		}
		else if (!strncmp(pSrc, "&quot;", 6))
		{
			*pDest++ = '"';
			len++;
			pSrc += 6;
		}
		else if (!strncmp(pSrc, "&amp;nbsp;", 10))
		{
			*pDest++ = ' ';
			len++;
			pSrc += 10;
		}
		else if (!strncmp(pSrc, "&amp;times;", 11))
		{
			*pDest++ = ' ';
			len++;
			pSrc += 11;
		}
		else if (!strncmp(pSrc, "&amp;", 5))
		{
			*pDest++ = '&';
			len++;
			pSrc += 5;
		}
		else if (!strncmp(pSrc, "&#", 2))
		{
			pSemicolon = strchr(pSrc, ';');
			if (pSemicolon && pSemicolon - (pSrc + 2) < sizeof(sUnicode))
			{
				strncpy(sUnicode, pSrc + 2, pSemicolon - (pSrc + 2));
				sUnicode[pSemicolon - (pSrc + 2)] = '\0';
				u = atol(sUnicode);
				UCS4_to_UTF8(u, sUnicode);
				strcpy(pDest, sUnicode);
				pDest += strlen(sUnicode);
				len += strlen(sUnicode);
				pSrc = pSemicolon + 1;
			}
			else
			{
				pSrc++;
				pDest++;
				len++;
			}
		}
		else if (!memcmp(pSrc, "\n\n", 2))
		{
			strncpy(pDest, "<br>", 4);
			pDest += 4;
			len += 4;
			pSrc += 2;
		}
		else if (*pSrc == '\n')
		{
			if (len > 0 && *(pDest - 1) != ' ' && *(pSrc+1) != ' ')
			{
				*pDest++ = ' ';
				len++;
			}
			pSrc++;
		}
		else if (*pSrc == '\r')
			pSrc++;
		else
		{
			*pDest++ = *pSrc++;
			len++;
		}
	}
	*pDest = '\0';
	return len;
}

void memrcpy(void *dest, void *src, int len) // memory copy starting from the last byte
{
	char *d = (char*)dest;
	char *s = (char*)src;

	if (len >= 0)
	{
		d += len - 1;
		s += len - 1;

		while (len--)
		{
			*d = *s;
			d--;
			s--;
		}
	}
}

#define ALL_LANGUAGES "ar:als:an:ast:bn:bs:cy:zh-min-nan:be-x-old:br:bg:ca:cs:da:pdc:de:et:el:es:eo:eu:fa:fo:fr:fy:gd:gl:ko:hi:hr:id:is:it:he:jv:ka:kk:sw:la:lt:lv:hu:mk:arz:ms:mn:nl:ja:no:nn:oc:pl:pt:ro:ru:sah:sco:simple:sk:sl:sr:sh:fi:sv:tl:ta:th:tr:uk:ur:ug:vi:war:yi:zh-yue:diq:bat-smg:zh:"
#define MAX_LANGUAGE_STRING 16
int unsupported_article(char *pHtmlFile)
{
	if (strstr(pHtmlFile, " talk:") ||
		!strncmp(pHtmlFile, "Talk:", 5) || !strncmp(pHtmlFile, "User:", 5) ||
		!strncmp(pHtmlFile, "Wikipedia:", 10) || !strncmp(pHtmlFile, "Category:", 9) ||
		!strncmp(pHtmlFile, "File:", 5) || !strncmp(pHtmlFile, "Template:", 9) ||
		!strncmp(pHtmlFile, "Portal:", 7) || !strncmp(pHtmlFile, "Image:", 6) ||
		!strncmp(pHtmlFile, "MediaWiki:", 10))
	{
		return 1;
	}
	else
	{
		char s[MAX_LANGUAGE_STRING];
		char *p;
		
		p = strchr(pHtmlFile, ':');
		if (p && p - pHtmlFile < MAX_LANGUAGE_STRING - 1)
		{
			memcpy(s, pHtmlFile, p - pHtmlFile + 1);
			s[p - pHtmlFile] = '\0';
			if (strstr(ALL_LANGUAGES, s))
				return 1;
		}
	}
	return 0;
}

#define MAX_FOLDER_DEPTH 4
#define MAX_SUBFOLDERS 200000
#define MAX_FOLDER_NAME 512

int get_subfolders_and_files(char *dir, int nDepth, char *folders[MAX_FOLDER_DEPTH][MAX_SUBFOLDERS],
	int folder_types[MAX_FOLDER_DEPTH][MAX_SUBFOLDERS])
{
	int count = 0;
	struct dirent *de = NULL;
	DIR *d = NULL;

	if (nDepth >= MAX_FOLDER_DEPTH)
	{
		showMsg(0, "exceed maximum depth %s\n", dir);
		exit(-1);
	}
	d = opendir(dir);
	if(d)
	{
		while ((de = readdir(d)) != NULL)
		{
			if (de->d_name[0] != '.')
			{
				if (nDepth >= MAX_FOLDER_DEPTH - 1)
					de->d_type = DT_REG; // NFS does not return the correct type sometimes
				if (de->d_type != DT_REG || !strstr(de->d_name, ".html") || !unsupported_article(de->d_name))
				{
					if (count >= MAX_SUBFOLDERS)
					{
						showMsg(0, "too many files in folder %s\n", dir);
						exit(-1);
					}
					sprintf(folders[nDepth][count], "%s/%s", dir, de->d_name);
					folder_types[nDepth][count] = de->d_type;
					count++;
				}
			}
		}
		
		closedir(d);
	}

	return count;
}

char *folders[MAX_FOLDER_DEPTH][MAX_SUBFOLDERS];
char *folder_names;
int folder_types[MAX_FOLDER_DEPTH][MAX_SUBFOLDERS];
int folder_counts[MAX_FOLDER_DEPTH];
int folder_indexes[MAX_FOLDER_DEPTH];
int current_folder_depth = -1;

char *next_html_files(char *dir)
{
	static char file_name[MAX_FOLDER_NAME];
	int bDone = 0;
	
	file_name[0] = '\0';
	if (current_folder_depth < 0)
	{
		int i, j;

		folder_names = malloc(MAX_FOLDER_DEPTH * MAX_SUBFOLDERS * MAX_FOLDER_NAME);
		if (!folder_names)
		{
			showMsg(0, "malloc folder_names error\n");
			exit(-1);
		}
		for (i = 0; i < MAX_FOLDER_DEPTH; i++)
			for (j = 0; j < MAX_SUBFOLDERS; j++)
				folders[i][j] = folder_names + (i * MAX_SUBFOLDERS + j) * MAX_FOLDER_NAME;
		current_folder_depth++;
		folder_counts[current_folder_depth] = get_subfolders_and_files(dir, current_folder_depth, folders, folder_types);
		if (folder_counts[current_folder_depth] == 0)
			bDone = 1;
		else
		{
			folder_indexes[current_folder_depth] = 0;
		}
	}

	if (!bDone && folder_indexes[current_folder_depth] < folder_counts[current_folder_depth])
	{
		int i = folder_indexes[current_folder_depth];
		if (folder_types[current_folder_depth][i] == DT_DIR || folder_types[current_folder_depth][i] == DT_UNKNOWN)
		{
			current_folder_depth++;
			folder_counts[current_folder_depth] = get_subfolders_and_files(folders[current_folder_depth-1][i],
				current_folder_depth, folders, folder_types);
			if (folder_counts[current_folder_depth] == 0)
			{
				current_folder_depth--;
				folder_indexes[current_folder_depth]++;
				return next_html_files(folders[current_folder_depth][folder_indexes[current_folder_depth]]);
			}
			folder_indexes[current_folder_depth] = 0;
			next_html_files(folders[current_folder_depth][0]);
			if (file_name[0])
				bDone = 1;
		}
		else if (folder_types[current_folder_depth][i] == DT_REG)
		{
			strcpy(file_name, folders[current_folder_depth][i]);
			folder_indexes[current_folder_depth]++;
			bDone = 1;
		}
	}

	// if not done, go to the parent folder's next folder
	if (!bDone)
	{
		current_folder_depth--;
		if (current_folder_depth < 0)
			return file_name;
		else
		{
			folder_indexes[current_folder_depth]++;
			return next_html_files(folders[current_folder_depth][folder_indexes[current_folder_depth]]);
		}
	}
	return file_name;
}

void get_file_name_from_path(char *sFile, char *sPath)
{
	char *pLastSlash;
	char *pLastDot;
	int len;

	sFile[0] = '\0';
	len = 0;
	pLastSlash = strrchr(sPath, '/');
	if (pLastSlash)
	{
		pLastDot = strrchr(pLastSlash, '.');
		if (pLastDot)
		{
			len = pLastDot - pLastSlash - 1;
			sFile = strncpy(sFile, pLastSlash + 1, len);
			sFile[len] = '\0';
		}
		else
		{
			strcpy(sFile, pLastSlash + 1);
			len = strlen(sFile);
		}
	}

	// truncation the trailing seq number in the format of _xxxx where x is the hex number
	if (len > 5 && sFile[len-5] == '_' &&
		(('0' <= sFile[len-4] && sFile[len-4] <= '9') || ('a' <= sFile[len-4] && sFile[len-4] <= 'f')) &&
		(('0' <= sFile[len-3] && sFile[len-3] <= '9') || ('a' <= sFile[len-3] && sFile[len-3] <= 'f')) &&
		(('0' <= sFile[len-2] && sFile[len-2] <= '9') || ('a' <= sFile[len-2] && sFile[len-2] <= 'f')) &&
		(('0' <= sFile[len-1] && sFile[len-1] <= '9') || ('a' <= sFile[len-1] && sFile[len-1] <= 'f')))
		sFile[len-5] = '\0';
}

void get_title(char *pHtmlFile, char *sTitle, char *sTitleSearch, char *sFirstTwoChars, char *sSecondTwoChars)
{
	get_file_name_from_path(sTitle, pHtmlFile);
	build_title_search(sTitle, sTitleSearch, sFirstTwoChars, sSecondTwoChars);
}

void article_info(char *pHtmlFile, char *sRedirect, int *nType)
{
	FILE *fd;
	char buf[1024];
	int len;
	char *p, *p2;

	*nType = -1;
	fd = fopen(pHtmlFile, "rb");
	if (!fd)
	{
		showMsg(0, "cannot open file %s, error: %s\n", pHtmlFile, strerror(errno));
		exit(-1);
	}
	len = fread(buf, 1, 1023, fd);
	buf[len] = '\0';
	sRedirect[0] = '\0';
	p = strstr(buf, "<p>Redirecting to <a href=");
	if (p)
	{
		p += 27; // skip <p>Redirecting to <a href="
		p2 = strchr(p, '"');
		if (p2)
		{
			*p2 = '\0';
			if (strstr(p, "../articles/"))
			{
				*nType = 1;
				get_file_name_from_path(sRedirect, p);
				url_decode(sRedirect);
			}
		}
	}
	else
		*nType = 0;
	if (fd)
		fclose(fd);
}

void render_newline(int space)
{
	if (render_buf.current_x >= 0)
	{
		render_buf.current_y += render_buf.nCurrentLineHeight;
		render_buf.nCurrentLineIdx = 0;
		render_buf.nLines++;
		memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	}

	if (space)
	{
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = space;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
		render_buf.current_y += space;
	}

	render_buf.current_x = -1;
	render_buf.nCurrentLineIdx = 0;
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
//	render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace;
//	if (render_buf.nCurrentFontIdx == DEFAULT_FONT_IDX)
//	{
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
//			(char)ESC_1_NEW_LINE_DEFAULT_FONT;
//	}
//	else
//	{
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
//			(char)ESC_3_NEW_LINE_WITH_FONT;
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
//			(char)(render_buf.nCurrentFontIdx | (render_buf.nCurrentLineHeight << 3));
//	}
//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
}

#define SIZE_RENDER_STRING_BUF 32000
void render_string(char *pContentCurrent, long lenToProcess, int nFontIdx, int nType, char *link_str)
{
	int nTextLenFitWidth;
	char sBuf[SIZE_RENDER_STRING_BUF];
	char *pBuf;

	if (nFontIdx >= 0)
		render_buf.nCurrentFontIdx = nFontIdx;
	if (nType >= 0)
		render_buf.nCurrentRenderType = nType;
	if (lenToProcess >= SIZE_RENDER_STRING_BUF)
		lenToProcess = SIZE_RENDER_STRING_BUF - 1;
	strncpy(sBuf,  pContentCurrent, lenToProcess);
	sBuf[lenToProcess] = '\0';

	pBuf = sBuf;

	while (*pBuf)
	{
		showMsg(6, "[%c%c%c%c] - (%d, %d)\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], render_buf.current_x, render_buf.current_y);
		if (render_buf.nCurrentRenderType == RENDER_TYPE_LINK && link_str && render_buf.nLinks < MAX_LINKS)
		{
			if (render_buf.current_x < 0)
				links[render_buf.nLinks].start_x = 0;
			else
				links[render_buf.nLinks].start_x = render_buf.current_x;
			if (links[render_buf.nLinks].start_x < render_buf.current_indent)
				links[render_buf.nLinks].start_x = render_buf.current_indent;
			links[render_buf.nLinks].start_y = render_buf.current_y;
			links[render_buf.nLinks].bWikiLink = 0;
			strncpy(links[render_buf.nLinks].sLink, link_str, MAX_LINK_STRING - 1);
			links[render_buf.nLinks].sLink[MAX_LINK_STRING - 1] = '\0';
		}
		if (render_buf.current_x < 0) /* new line */
		{
			render_buf.current_x = 0;
			if (*pBuf == ' ')
				pBuf++;
			render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
			if (render_buf.nCurrentFontIdx == DEFAULT_FONT_IDX)
			{
				render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
					(char)ESC_1_NEW_LINE_DEFAULT_FONT;
			}
			else
			{
				render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
					(char)ESC_3_NEW_LINE_WITH_FONT;
				render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
					(char)(render_buf.nCurrentFontIdx | (render_buf.nCurrentLineHeight << 3));
			}
		}
		if (render_buf.current_x < render_buf.current_indent)
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_7_FORWARD;
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)(render_buf.current_indent - render_buf.current_x);
			render_buf.current_x = render_buf.current_indent;
		}
		nTextLenFitWidth = srting_len_fit_width(render_buf.nCurrentFontIdx, &render_buf.current_x, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, pBuf, strlen(pBuf));
		if (nTextLenFitWidth > 0)
		{
		strncpy(&render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx], pBuf, nTextLenFitWidth);
		render_buf.nCurrentLineIdx += nTextLenFitWidth;
		if (render_buf.nCurrentRenderType == RENDER_TYPE_LINK && link_str && render_buf.nLinks < MAX_LINKS)
		{
			if (render_buf.current_x - links[render_buf.nLinks].start_x > 0)
			{
				render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = ESC_10_HORIZONTAL_LINE;
				render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = render_buf.current_x - links[render_buf.nLinks].start_x;
			}
			links[render_buf.nLinks].end_x = render_buf.current_x - 1;
			links[render_buf.nLinks].end_y = render_buf.current_y + render_buf.nCurrentLineHeight - 1;
			render_buf.nLinks++;
		}
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
		pBuf += nTextLenFitWidth;
		}

		if (*pBuf)
		{
			render_buf.current_y += render_buf.nCurrentLineHeight;
			render_buf.current_x = -1;
			render_buf.nCurrentLineIdx = 0;
			render_buf.nLines++;
			memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
			if (render_buf.nCurrentRenderType == RENDER_TYPE_LINK  && link_str && render_buf.nLinks < MAX_LINKS)
			{
				if (render_buf.current_x < 0)
					links[render_buf.nLinks].start_x = 0;
				else
					links[render_buf.nLinks].start_x = render_buf.current_x;
				links[render_buf.nLinks].start_y = render_buf.current_y;
				links[render_buf.nLinks].bWikiLink = 0;
				strncpy(links[render_buf.nLinks].sLink, link_str, MAX_LINK_STRING - 1);
				links[render_buf.nLinks].sLink[MAX_LINK_STRING - 1] = '\0';
			}
//			else if (render_buf.nCurrentRenderType == RENDER_TYPE_TABLE_DATA)
//			{
//				render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = ESC_12_FULL_HORIZONTAL_LINE;
//				render_buf.current_y += 1;
//			}
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
		}
	}
}

void get_key_value(char *value, int val_len, char *key, char *str, int len)
{
	char *p, *p2;

	value[0] = '\0';
	p = strnstr(str, key, len);
	if (p)
	{
		p += strlen(key);
		if (p[0] == '=' && (p[1] == '"' || p[1] =='\''))
		{
			p += 2;
			if (p[1] == '"')
				p2 = strnchr(p, '"', len - strlen(key) - 2);
			else
				p2 = strnchr(p, '\'', len - strlen(key) - 2);
			if (p2)
			{
				int copy_len;
				if (val_len - 1 > p2 - p)
					copy_len = p2 - p;
				else
					copy_len = val_len - 1;
				strncpy(value, p, copy_len);
				value[copy_len] = '\0';
			}
		}

	}
//	url_decode(value);
}

void render_sub_title_node(int idxNode)
{
	if (render_buf.current_x >= 0)
	{
		render_buf.current_y += render_buf.nCurrentLineHeight;
		render_buf.current_x = -1;
		render_buf.nCurrentLineIdx = 0;
		render_buf.nLines++;
		memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	}

	switch (article_nodes[idxNode].idxTag)
	{
		case TAG_PAIR_H2:
			render_text_node(idxNode, H2_FONT_IDX, RENDER_TYPE_NORMAL);
//			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_12_FULL_HORIZONTAL_LINE;
//			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
//			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 2;
//			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
//			render_buf.current_y += 3;
			break;
		case TAG_PAIR_H3:
			render_text_node(idxNode, H3_FONT_IDX, RENDER_TYPE_NORMAL);
			break;
		case TAG_PAIR_H4:
			render_text_node(idxNode, H4_FONT_IDX, RENDER_TYPE_NORMAL);
			break;
		case TAG_PAIR_H5:
			render_text_node(idxNode, H5_FONT_IDX, RENDER_TYPE_NORMAL);
			break;
		default:
			render_text_node(idxNode, DEFAULT_FONT_IDX, RENDER_TYPE_NORMAL);
			break;
	}

	render_buf.current_y += render_buf.nCurrentLineHeight;
	render_buf.current_x = -1;
	render_buf.nCurrentLineIdx = 0;
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
}

void dump_wiki_nodes()
{
	int i;
	int j;
	int level = 0;

	for (i = 0; i < nWikiNodeCount; i++)
	{
		printf("|node idx[%d] tag[%d] %d tag[",
			i, wiki_nodes[i].idxTag, wiki_nodes[i].bTagStart);
		for (j = 0; j < wiki_nodes[i].lenTag; j++)
			printf("%c", wiki_nodes[i].pTag[j]);
		printf("] len[%d]\n", wiki_nodes[i].lenTag);
	}
}

void dump_article_node(int idxNode, int level)
{
	int i;

	for (i = 0; i < level; i++)
		printf("-");
	printf("|node idx[%d] child[%d] next[%d] tag[",
		idxNode, article_nodes[idxNode].idxChildNode, article_nodes[idxNode].idxNextNode);
	if (0 <= article_nodes[idxNode].idxTag && article_nodes[idxNode].idxTag < MAX_TAG_PAIRS)
		printf("%s] desc[", tag_pairs[article_nodes[idxNode].idxTag].sTagStart);
	else
		printf("%d] desc[", article_nodes[idxNode].idxTag);
	for (i=0; i < article_nodes[idxNode].lenTagDesc; i++)
		printf("%c", article_nodes[idxNode].pTagDesc[i]);
	printf("] len[%d]\n", article_nodes[idxNode].len);
	for (i = 0; i < level; i++)
		printf(" ");
	printf("|{");
	for (i=0; i< article_nodes[idxNode].len; i++)
		printf("%c", article_nodes[idxNode].pContent[i]);
	printf("}\n");
}

void dump_article_node_and_children(int idxNode, int level)
{
	int childNode;
	int nextNode;

	dump_article_node(idxNode, level);
	if (article_nodes[idxNode].idxChildNode > 0)
		dump_article_node_and_children(article_nodes[idxNode].idxChildNode, level + 1);
	if (article_nodes[idxNode].idxNextNode > 0)
		dump_article_node_and_children(article_nodes[idxNode].idxNextNode, level);
}

void render_text_node(int idxNode, int nFontIdx, int nRenderType)
{
	int nOrigFontIdx;
	int nOrigRenderType;
	int idxNextChildNode;

	nOrigFontIdx = render_buf.nCurrentFontIdx;
	nOrigRenderType = render_buf.nCurrentRenderType;
	if (nFontIdx >= 0)
		render_buf.nCurrentFontIdx = nFontIdx;
	if (nRenderType >= 0)
		render_buf.nCurrentRenderType = nRenderType;
	if (article_nodes[idxNode].len > 0 && !all_blanks(article_nodes[idxNode].pContent, article_nodes[idxNode].len))
		render_string(article_nodes[idxNode].pContent, article_nodes[idxNode].len,
			render_buf.nCurrentFontIdx, render_buf.nCurrentRenderType, NULL);
	idxNextChildNode = article_nodes[idxNode].idxChildNode;

	if (idxNextChildNode)
	{
		render_article_node(idxNextChildNode);
//		idxNextChildNode = article_nodes[idxNextChildNode].idxNextNode;
	}
	render_buf.nCurrentFontIdx = nOrigFontIdx;
	render_buf.nCurrentRenderType = nOrigRenderType;
}

void render_LI(int idxNode)
{
	int indent;
	char *pForwardPixels;
	char li_str[10];
	int nDepth = 0;
	int i;

	if (render_buf.list_depth > 0)
		indent = (render_buf.list_depth - 1) * PIXELS_PER_INDENT + PIXELS_FIRST_INDENT;
	else
		indent = 0;
	if (indent > LCD_BUF_WIDTH_PIXELS / 3 * 2)
		indent = LCD_BUF_WIDTH_PIXELS / 3 * 2;

	if (render_buf.list_depth > 0 && render_buf.list_type[render_buf.list_depth - 1] == 'O')
	{
		render_buf.ol_count[render_buf.list_depth - 1]++;
		for (i = 0; i < render_buf.list_depth; i++)
		{
			if (render_buf.list_type[i] == 'O')
				nDepth++;
		}
		
		switch (nDepth)
		{
			case 1:
				sprintf(li_str, "%d. ", render_buf.ol_count[render_buf.list_depth - 1]);
				break;
			case 2:
				if (render_buf.ol_count[render_buf.list_depth - 1] > (26 * 27))
					sprintf(li_str, "%c%c%c. ", 'A' + (render_buf.ol_count[render_buf.list_depth - 1] - 26 * 26 - 1) / (26 * 26), 
						'A' + ((render_buf.ol_count[render_buf.list_depth - 1] - 27) % (26 * 26)) / 26,
						'A' + ((render_buf.ol_count[render_buf.list_depth - 1] - 1) % 26));
				else if (render_buf.ol_count[render_buf.list_depth - 1] > 26)
					sprintf(li_str, "%c%c. ", 'A' + (render_buf.ol_count[render_buf.list_depth - 1] - 27) / 26, 
						'A' + ((render_buf.ol_count[render_buf.list_depth - 1] - 1)% 26));
				else
					sprintf(li_str, "%c. ", 'A' + render_buf.ol_count[render_buf.list_depth - 1] - 1);
				break;
			default:
				if (render_buf.ol_count[render_buf.list_depth - 1] > 26 * 27)
					sprintf(li_str, "%c%c%c. ", 'a' + (render_buf.ol_count[render_buf.list_depth - 1] - 26 * 26 - 1) / (26 * 26), 
						'a' + ((render_buf.ol_count[render_buf.list_depth - 1] - 27) % 26 * 26) / 26,
						'a' + ((render_buf.ol_count[render_buf.list_depth - 1] - 1) % 26));
				else if (render_buf.ol_count[render_buf.list_depth - 1] > 26)
					sprintf(li_str, "%c%c. ", 'a' + (render_buf.ol_count[render_buf.list_depth - 1] - 27) / 26, 
						'a' + ((render_buf.ol_count[render_buf.list_depth - 1] - 1) % 26));
				else
					sprintf(li_str, "%c. ", 'a' + render_buf.ol_count[render_buf.list_depth - 1] - 1);
				break;
		}
	}
	else
	{
		for (i = 0; i < render_buf.list_depth; i++)
			if (render_buf.list_type[render_buf.list_depth - 1] == 'U')
				nDepth++;
		switch (nDepth)
		{
			case 1:
				li_str[0] = 0xE2;
				li_str[1] = 0x96;
				li_str[2] = 0xAA;
				li_str[3] = ' ';
				li_str[4] = '\0';
				break;
			case 2:
				li_str[0] = 0xE2;
				li_str[1] = 0x80;
				li_str[2] = 0xA2;
				li_str[3] = ' ';
				li_str[4] = '\0';
				break;
			default:
				li_str[0] = 0xE2;
				li_str[1] = 0x97;
				li_str[2] = 0xA6;
				li_str[3] = ' ';
				li_str[4] = '\0';
				break;
		}
	}

	if (render_buf.current_x < 0) /* new line */
	{
		render_buf.current_x = 0;
		render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
		if (render_buf.nCurrentFontIdx == DEFAULT_FONT_IDX)
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)ESC_1_NEW_LINE_DEFAULT_FONT;
		}
		else
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)ESC_3_NEW_LINE_WITH_FONT;
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)(render_buf.nCurrentFontIdx | (render_buf.nCurrentLineHeight << 3));
		}
	}

	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_7_FORWARD;
	pForwardPixels = &(render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx]);
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 0;
	render_buf.current_x = 0;
	render_buf.current_indent = 0;
	render_string(li_str, strlen(li_str), -1, -1, NULL);
	if (render_buf.current_x < indent)
	{
		*pForwardPixels = (char)(indent - render_buf.current_x);
		render_buf.current_x += indent - render_buf.current_x;
	}
	render_buf.current_indent = indent;
	render_text_node(idxNode, -1, -1);
	render_newline(0);
}

void render_DT(int idxNode)
{
	int indent;
	char *pForwardPixels;

	if (render_buf.list_depth > 1)
		indent = (render_buf.list_depth - 2) * PIXELS_PER_INDENT + PIXELS_FIRST_INDENT;
	else
		indent = 0;
	if (indent > LCD_BUF_WIDTH_PIXELS / 3 * 2)
		indent = LCD_BUF_WIDTH_PIXELS / 3 * 2;

	if (render_buf.current_x < 0) /* new line */
	{
		render_buf.current_x = 0;
		render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
		if (render_buf.nCurrentFontIdx == DEFAULT_FONT_IDX)
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)ESC_1_NEW_LINE_DEFAULT_FONT;
		}
		else
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)ESC_3_NEW_LINE_WITH_FONT;
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)(render_buf.nCurrentFontIdx | (render_buf.nCurrentLineHeight << 3));
		}
	}

	render_buf.current_indent = indent;
	render_text_node(idxNode, -1, -1);
	render_newline(0);
}

void render_DD(int idxNode)
{
	int indent;
	char *pForwardPixels;

	if (render_buf.list_depth > 0)
		indent = (render_buf.list_depth - 1) * PIXELS_PER_INDENT + PIXELS_FIRST_INDENT;
	else
		indent = 0;
	if (indent > LCD_BUF_WIDTH_PIXELS / 3 * 2)
		indent = LCD_BUF_WIDTH_PIXELS / 3 * 2;

	if (render_buf.current_x < 0) /* new line */
	{
		render_buf.current_x = 0;
		render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
		if (render_buf.nCurrentFontIdx == DEFAULT_FONT_IDX)
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)ESC_1_NEW_LINE_DEFAULT_FONT;
		}
		else
		{
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)ESC_3_NEW_LINE_WITH_FONT;
			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
				(char)(render_buf.nCurrentFontIdx | (render_buf.nCurrentLineHeight << 3));
		}
	}

	render_buf.current_indent = indent;
	render_text_node(idxNode, -1, -1);
	render_newline(0);
}

void render_node_with_font(int idxNode, int idxFont)
{
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
		(char)ESC_4_CHANGE_FONT;
	render_buf.nCurrentFontIdx = idxFont;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
		(char)(render_buf.nCurrentFontIdx);
	render_text_node(idxNode, idxFont, -1);
	render_buf.nCurrentFontIdx = DEFAULT_FONT_IDX;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] =
		(char)ESC_5_RESET_TO_DEFAULT_FONT;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
}

void render_table(int idxNode)
{
	int idxNextChildNode;

	if (render_buf.current_x >= 0)
	{
		render_buf.current_y += render_buf.nCurrentLineHeight;
		render_buf.current_x = -1;
		render_buf.nCurrentLineIdx = 0;
		render_buf.nLines++;
		memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	}

	if (article_nodes[idxNode].idxTag == TAG_PAIR_TABLE)
	{
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 8;
		render_buf.current_y += 8;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
	}
//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_12_FULL_HORIZONTAL_LINE;
//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
//	render_buf.current_y += 1;

	if (article_nodes[idxNode].idxTag == TAG_PAIR_TR)
	{
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_1_NEW_LINE_DEFAULT_FONT;
		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
		render_buf.nCurrentFontIdx = DEFAULT_FONT_IDX;
		render_buf.current_x = 0;
	}
	else
		render_buf.current_x = -1;
	render_buf.nCurrentLineIdx = 0;
	render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);

	idxNextChildNode = article_nodes[idxNode].idxChildNode;

	if (idxNextChildNode)
	{
		render_article_node(idxNextChildNode);
	}

	if (render_buf.current_x >= 0)
	{
		render_buf.current_y += render_buf.nCurrentLineHeight;
		render_buf.current_x = -1;
		render_buf.nCurrentLineIdx = 0;
		render_buf.nLines++;
		memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	}

//	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_12_FULL_HORIZONTAL_LINE;
//	render_buf.current_y += 1;
//	if (article_nodes[idxNode].idxTag == TAG_PAIR_TABLE)
//	{
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 8;
//		render_buf.current_y += 8;
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
//	}

	render_buf.current_x = -1;
	render_buf.nCurrentLineIdx = 0;
	render_buf.nCurrentLineHeight = pcfFonts[render_buf.nCurrentFontIdx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
}

void render_TR(int idxNode)
{
	render_table(idxNode);
}

void render_TD(int idxNode)
{
//	if (render_buf.current_x > 0 && render_buf.current_x % TABLE_CELL_WIDTH) // cell not aligned to the cell width
//	{
//		int nForwardPixels = TABLE_CELL_WIDTH - (render_buf.current_x % TABLE_CELL_WIDTH);
//		if (render_buf.current_x + nForwardPixels >= LCD_BUF_WIDTH_PIXELS)
//		{
//			render_buf.current_y += render_buf.nCurrentLineHeight;
//			render_buf.current_x = -1;
//			render_buf.nCurrentLineIdx = 0;
//			render_buf.nLines++;
//			render_buf_sLines[render_buf.nLines-1][0] = '\0';
//		}
//		else
//		{
//			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_7_FORWARD;
//			render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)(nForwardPixels);
//			render_buf_sLines[render_buf.nLines-1][0] = '\0';
//			render_buf.current_x += nForwardPixels;
//		}
//
//	}
	render_text_node(idxNode, DEFAULT_FONT_IDX, RENDER_TYPE_TABLE_DATA);
	if (article_nodes[idxNode].len)
		render_string("  ", 2, DEFAULT_FONT_IDX, RENDER_TYPE_NORMAL, NULL);
//	if (render_buf.current_x < LCD_BUF_WIDTH_PIXELS - 2)
//	{
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_13_FULL_VERTICAL_LINE;
//		render_buf.current_x += 4;
//	}
//	else
//	{
//		render_buf.current_x = 4;
//		render_buf.nCurrentLineIdx = 0;
//		render_buf.nLines++;
//		render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_13_FULL_VERTICAL_LINE;
//	}
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
}

void render_ref(int idxNode)
{
}

void render_line_break(void)
{
	if (render_buf.current_x >= 0)
	{
		render_buf.current_y += render_buf.nCurrentLineHeight;
		render_buf.current_x = -1;
		render_buf.nCurrentLineIdx = 0;
		render_buf.nLines++;
		memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
	}
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = (char)ESC_0_SPACE_LINE;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx++] = 12;
	render_buf_sLines[render_buf.nLines-1][render_buf.nCurrentLineIdx] = '\0';
	render_buf.current_y += 12;
	render_buf.current_x = -1;
	render_buf.nCurrentLineIdx = 0;
	render_buf.nLines++;
	memset(render_buf_sLines[render_buf.nLines-1], 0, 256);
}

int all_blanks(char *p, int len)
{
	int rc = 1;

	while (rc && len > 0)
	{
		if (*p == ' ' || *p == 0x0A || *p == 0x0D || *p == '\t')
		{
			p++;
			len--;
		}
		else
			rc = 0;
	}
	return rc;
}

extern MYSQL *g_conn;

void get_redirect_title(char *sTitle)
{
	char sSQL[MAX_SQL_STR];
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc;

	sprintf(sSQL, "select redirect_title from entries where title='%s' and entry_type=1", sTitle);
	sTitle[0] = '\0';
	if (!(rc = mysql_query(g_conn, sSQL)))
	{
		res = mysql_use_result(g_conn);
		if ((row = mysql_fetch_row(res)) != NULL)
		{
			strcpy(sTitle, row[0]);
		}
		mysql_free_result(res);
	}
	mysql_commit(g_conn);
}

void render_link(int idxNode)
{
	char sClass[MAX_CLASS_STRING];
	char sRef[MAX_FOLDER_NAME];
	char sLink[MAX_LINK_STRING];
	int i;

	sLink[0] = '\0';
	get_key_value(sRef, MAX_FOLDER_NAME, "href", article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
	get_key_value(sClass, MAX_CLASS_STRING, "class", article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
	get_key_value(sLink, MAX_LINK_STRING, "title",article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
//	for (i=0; i < strlen(sLink); i++)
//	{
//		if (sLink[i] == ':')
//			sLink[i] = '~';
//		else if (sLink[i] == ' ')
//			sLink[i] = '_';
//	}
	if (!strcmp(sClass, "mw-redirect"))
	{
		get_redirect_title(sLink);
	}
	else if (!sLink[0] && !strncmp(sRef, "../../../../articles/", 21))
	{
		get_file_name_from_path(sLink, sRef);
	}
	else if (!strcmp(sClass, "external autonumber") || !strncmp(sRef, "#cite_note", 10) || !strncmp(sRef, "internal", 8))
		return; // skip external reference or citation


	if (article_nodes[idxNode].len > 0 && strncmp(article_nodes[idxNode].pContent, "File:", 5) && strncmp(article_nodes[idxNode].pContent, "http:", 5))
	{
		if (!strncmp(sLink, "http:", 5) || unsupported_article(sLink))
			sLink[0] = '\0';
		if (sLink[0])
		{
			render_string(article_nodes[idxNode].pContent, article_nodes[idxNode].len, -1, RENDER_TYPE_LINK, sLink);
		}
//		else
//		{
//			render_string(article_nodes[idxNode].pContent, article_nodes[idxNode].len, -1, RENDER_TYPE_NORMAL, NULL);
//		}
	}
}

void render_article_node(int idxNode)
{
	char keyval[256];

	showMsg(3, "render idxNode %d, idxTag %d, len %d, current_y %ld\n", idxNode, article_nodes[idxNode].idxTag, article_nodes[idxNode].len, render_buf.current_y);
	switch (article_nodes[idxNode].idxTag)
	{
		case TAG_PAIR_P:
			get_key_value(keyval, sizeof(keyval), "class", article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
			// skip if for error message
			if (strcmp(keyval, "error"))
			{
				//render_line_break(idxNode);
				render_newline(12);
				render_text_node(idxNode, DEFAULT_FONT_IDX, RENDER_TYPE_NORMAL);
			}
			break;
		case TAG_PAIR_BR:
		case TAG_PAIR_BR2:
		case TAG_PAIR_P_END:
			render_line_break();
			break;
		case TAG_PAIR_SPAN:
			get_key_value(keyval, sizeof(keyval), "class", article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
			// skip if for [edit]
			if (strcmp(keyval, "editsection"))
			{
				render_text_node(idxNode, -1, -1);
			}
			break;
		case TAG_PAIR_H1:
		case TAG_PAIR_H2:
		case TAG_PAIR_H3:
		case TAG_PAIR_H4:
		case TAG_PAIR_H5:
		case TAG_PAIR_H6:
			render_sub_title_node(idxNode);
			break;
		case TAG_PAIR_A:
			render_link(idxNode);
			break;
		case TAG_PAIR_BIG_BIG:
		case TAG_PAIR_BIG:
			render_node_with_font(idxNode, BOLD_FONT_IDX);
			break;
		case TAG_PAIR_BI:
			render_node_with_font(idxNode, BOLD_ITALIC_FONT_IDX);
			break;
		case TAG_PAIR_B:
			render_node_with_font(idxNode, BOLD_FONT_IDX);
			break;
		case TAG_PAIR_I:
			render_node_with_font(idxNode, ITALIC_FONT_IDX);
			break;
		case TAG_PAIR_TABLE:
			get_key_value(keyval, sizeof(keyval), "class", article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
			// skip if for table of contents
			if (strcmp(keyval, "toc"))
			{
				render_table(idxNode);
			}
			break;
		case TAG_PAIR_TR:
			render_TR(idxNode);
			break;
		case TAG_PAIR_TH:
		case TAG_PAIR_TD:
			render_TD(idxNode);
			break;
		case TAG_PAIR_OL:
			render_newline(2);
			if (render_buf.list_depth < MAX_LIST_DEPTH - 1)
			{
				render_buf.list_type[render_buf.list_depth] = 'O';
				render_buf.ol_count[render_buf.list_depth] = 0;
				render_buf.list_depth++;
			}
			break;
		case TAG_PAIR_UL:
			render_newline(2);
			if (render_buf.list_depth < MAX_LIST_DEPTH - 1)
			{
				render_buf.list_type[render_buf.list_depth] = 'U';
				render_buf.ol_count[render_buf.list_depth] = 0;
				render_buf.list_depth++;
			}
			break;
		case TAG_PAIR_DL:
			render_newline(2);
			if (render_buf.list_depth < MAX_LIST_DEPTH - 1)
			{
				render_buf.list_type[render_buf.list_depth] = 'D';
				render_buf.ol_count[render_buf.list_depth] = 0;
				render_buf.list_depth++;
			}
			break;
		case TAG_PAIR_EOL:
		case TAG_PAIR_EUL:
		case TAG_PAIR_EDL:
			render_newline(2);
			if (render_buf.list_depth > 0)
			{
				render_buf.list_depth--;
				render_buf.current_indent = 0;
			}
			break;
		case TAG_PAIR_LI:
			get_key_value(keyval, sizeof(keyval), "class", article_nodes[idxNode].pTagDesc, article_nodes[idxNode].lenTagDesc);
			// skip if for for contents
			if (strncmp(keyval, "toclevel-", 9))
			{
				render_LI(idxNode);
			}
			break;
		case TAG_PAIR_DT:
			render_DT(idxNode);
			break;
		case TAG_PAIR_DD:
			render_DD(idxNode);
			break;
		case TAG_PAIR_SCRIPT:
		case TAG_PAIR_REF:
			break;
		default:
			render_text_node(idxNode, DEFAULT_FONT_IDX, RENDER_TYPE_NORMAL);
			break;
	}
	if (article_nodes[idxNode].idxNextNode)
		render_article_node(article_nodes[idxNode].idxNextNode);
}

void render_wiki_text(char *pText, long lenText)
{
	int idxChildTag;
	int idxMyNode;
	int idxChildNode;
	int idxPreviousChild;
	int lenBeforeTag;
	char *pTagDesc;
	int lenTagDesc;
	char *pTagContent;
	int lenTagContent;
	char *pAfterTag;
	int lenAfterTag;

	idxMyNode = nArticleNodeCount++;
	article_nodes[idxMyNode].idxTag = -1;
	article_nodes[idxMyNode].pTagDesc = NULL;
	article_nodes[idxMyNode].lenTagDesc = 0;
	article_nodes[idxMyNode].pContent = pText;
	article_nodes[idxMyNode].len = 0;
	article_nodes[idxMyNode].idxChildNode = 0;
	article_nodes[idxMyNode].idxNextNode = 0;

	idxPreviousChild = 0;
	idxChildTag = find_next_tag(pText, lenText, &lenBeforeTag, &pTagDesc, &lenTagDesc, &pTagContent, &lenTagContent, &pAfterTag, &lenAfterTag);
	article_nodes[idxMyNode].len = lenBeforeTag;

	while (idxChildTag >= 0)
	{
		idxChildNode = build_child_tree(idxPreviousChild, idxChildTag, pTagDesc, lenTagDesc, pTagContent, lenTagContent);
		if (!article_nodes[idxMyNode].idxChildNode)
		{
			article_nodes[idxMyNode].idxChildNode = idxChildNode;
		}
		idxPreviousChild = idxChildNode;
		pText = pAfterTag;
		lenText = lenAfterTag;
		idxChildTag = find_next_tag(pText, lenText, &lenBeforeTag, &pTagDesc, &lenTagDesc, &pTagContent, &lenTagContent, &pAfterTag, &lenAfterTag);
		if (lenBeforeTag > 0)
		{
			article_nodes[nArticleNodeCount].idxTag = -1;
			article_nodes[nArticleNodeCount].pTagDesc = NULL;
			article_nodes[nArticleNodeCount].lenTagDesc = 0;
			article_nodes[nArticleNodeCount].pContent = pText;
			article_nodes[nArticleNodeCount].len = lenBeforeTag;
			article_nodes[nArticleNodeCount].idxChildNode = 0;
			article_nodes[nArticleNodeCount].idxNextNode = 0;
			if (idxPreviousChild > 0)
			{
				article_nodes[idxPreviousChild].idxNextNode = nArticleNodeCount;
			}
			else
				article_nodes[idxMyNode].idxNextNode = nArticleNodeCount;
			idxPreviousChild = nArticleNodeCount;
			nArticleNodeCount++;
		}
	}

	if (msgLevel() >= 4)
	{
//		int i;
//		for (i=0; i< nArticleNodeCount; i++)
//			dump_article_node(i, 0);

		dump_article_node_and_children(0, 0);
	}
	render_article_node(0);
}

void url_decode(char *src)
{
	char dst[MAX_TITLE_LEN];
	int dst_len = 0;

	if (!*src)
		return;
	
	int i = 0;
	int length = strlen(src);
	while ( i < length && dst_len < MAX_TITLE_LEN - 1)
	{
		unsigned int c = src[i++];
		if ( c=='%' && (i+1<length) )
		{
			int number = 0;
			
			unsigned char digit = (unsigned char) src[i++];
			digit = toupper(digit);
			if ( digit<='9' )
				digit -= 48;
			else
				digit -= 55;
			number = digit;
			
			digit = (unsigned char) src[i++];
			digit = toupper(digit);
			if ( digit<='9' )
				digit -= 48;
			else
				digit -= 55;
			
			number = number*16 + digit;
			
			dst[dst_len++] = (char)number;
		}
		else
			dst[dst_len++] = c;
	}

	dst[dst_len] = '\0';
	strcpy(src, dst);
	return;
}

// size is the max size of input buffer (for expansion), not the length of the input string
void pad_backslash(char *s, int size)
{
	int i;

	i = 0;
	while (i < size - 2 && s[i])
	{
		if (s[i] == '\'' || s[i] == '\\')
		{
			memrcpy(&s[i+1], &s[i], size - i - 2);
			s[i] = '\\';
			i++;
		}
		i++;
	}
	s[size - 1] = '\0';
}

long GetArticleIdxByTitle(MYSQL *conn, char *sTitle)
{
	char sPaddedTitle[MAX_TITLE_LEN];
	char sSQL[MAX_SQL_STR];
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc;
	long idxArticle = -1;

	strcpy(sPaddedTitle, sTitle);
	pad_backslash(sPaddedTitle, MAX_TITLE_LEN);
	sprintf(sSQL, "select idx from entries where title='%s' and entry_type=0", sPaddedTitle);
	if (!(rc = mysql_query(conn, sSQL)))
	{
		res = mysql_use_result(conn);
		if ((row = mysql_fetch_row(res)) != NULL)
		{
			idxArticle = atol(row[0]);
		}
		mysql_free_result(res);
	}
	return idxArticle;
}

void trim_body_text(char **pText, int *len)
{
	char *p;

	p = strstr(*pText, "<!-- start content -->");
	if (p)
	{
		p += 22;
		*len -= p - *pText;
		*pText = p;
	}
	p = strstr(*pText, "<div class=\"printfooter\">");
	if (p)
		*len = p - *pText;

}

void trim_blanks(char *pSrc, int lenSrc)
{
	int lenLeadingBlanks = 0;
	int lenTrailingBlanks = 0;
	
	while (lenLeadingBlanks < lenSrc && pSrc[lenLeadingBlanks] == ' ')
		lenLeadingBlanks++;
	if (lenLeadingBlanks < lenSrc)
		while (lenTrailingBlanks < lenSrc && pSrc[lenSrc - lenTrailingBlanks - 1] == ' ')
			lenTrailingBlanks++;
	if (lenLeadingBlanks > 0)
		memcpy(pSrc, &pSrc[lenLeadingBlanks], lenSrc - lenLeadingBlanks - lenTrailingBlanks);
	if (lenLeadingBlanks > 0 || lenTrailingBlanks > 0)
		pSrc[lenSrc - lenLeadingBlanks - lenTrailingBlanks] = '\0';
}

long cpy_wiki_buf(char *sBuf, long lenBuf, long maxLenBuf, char *pTagContent, int lenTagContent)
{
	int copy_len;
	
	if (lenBuf + lenTagContent > maxLenBuf - 1)
	{
		showMsg(0, "cpy_wiki_buf %d > %d\n", lenBuf + lenTagContent, maxLenBuf);
		copy_len = maxLenBuf - 1 - lenBuf;
	}
	else
		copy_len = lenTagContent;
	memcpy(&sBuf[lenBuf], pTagContent, copy_len);
	return lenBuf + copy_len;
}

static char sListStack[MAX_LIST_DEPTH];
static int nListStack;
static char sListStackPrev[MAX_LIST_DEPTH];
static int nListStackPrev;

long process_list_stack(char *sListStack, int nListStack, char *sListStackPrev, int nListStackPrev, char *sBuf, long lenBuf, long maxLenBuf)
{
	int i;
	int nListSame = 0;
	
	i = 0;
	while (i < nListStack && i < nListStackPrev && sListStack[i] == sListStackPrev[i])
	{
		nListSame++;
		i++;
	}
	
	if (nListStackPrev > nListSame)
	{
		for  (i = nListStackPrev - 1; i >= nListSame; i--)
		{
			switch (sListStackPrev[i])
			{
				case 'O':
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</ol>", 5);
					break;
				case 'U':
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</ul>", 5);
					break;
				case 'D':
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</dl>", 5);
					break;
				default:
					break;
			}
		}
	}

	if (nListStack > nListSame)
	{
		for  (i = nListSame; i < nListStack; i++)
		{
			switch (sListStack[i])
			{
				case 'O':
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<ol>", 4);
					break;
				case 'U':
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<ul>", 4);
					break;
				case 'D':
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<dl>", 4);
					break;
				default:
					break;
			}
		}
	}
	return lenBuf;
	
}

long process_wiki_table(int idxStart, int idxEnd, char *sText, char *sBuf, long lenBuf, long maxLenBuf)
{
	int bIdxIncreased = 0;
	int idxEndLocal;
	int idxEndRow;
	char sCellText[MAX_CELL_STRING];
	char *pRow;
	int lenRow;
	int bBrBeforeRow = 0;
	int bBlankBeforeCell = 0;

	lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<br>", 4);
	idxStart++;
	if (idxStart <= idxEnd && wiki_nodes[idxStart].idxTag == WIKI_TAG_PAIR_TEXT)
		idxStart++; // skip the table attribute
	while (idxStart <= idxEnd)
	{
		bIdxIncreased = 0;
		switch(wiki_nodes[idxStart].idxTag)
		{
			case WIKI_TAG_PAIR_TABLE_CAPTION:
				if (wiki_nodes[idxStart].bTagStart == 1 && idxStart < idxEnd && wiki_nodes[idxStart + 1].idxTag == WIKI_TAG_PAIR_TEXT)
				{
					pRow = wiki_nodes[idxStart + 1].pTag;
					lenRow = wiki_nodes[idxStart + 1].lenTag;
					get_next_cell(&pRow, &lenRow, NULL, sCellText);
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, sCellText, strlen(sCellText));
					idxStart += 2;
					bIdxIncreased = 1;
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<br>", 4);
				}
				break;
			case WIKI_TAG_PAIR_TABLE_ROW:
				if (bBrBeforeRow) {
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<br>", 4);
					bBrBeforeRow = 0;
				}
				idxStart = find_wiki_tag_end(idxStart, idxEnd);
				bBlankBeforeCell = 0;
				break;
			case WIKI_TAG_PAIR_TABLE_CELLS: 
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					bBrBeforeRow = 1;
					bBlankBeforeCell = 0;
					idxEndRow = find_wiki_tag_end(idxStart, idxEnd); 
					idxStart++;
					while (idxStart < idxEndRow)
					{
						if (wiki_nodes[idxStart].idxTag == WIKI_TAG_PAIR_TEXT)
						{
							pRow = wiki_nodes[idxStart].pTag;
							lenRow = wiki_nodes[idxStart].lenTag;
							while (get_next_cell(&pRow, &lenRow, "||", sCellText))
							{
								if (bBlankBeforeCell)
								{
									lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, " ", 1);
								}
								lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, sCellText, strlen(sCellText));
								bBlankBeforeCell = 1;
							}
						}
						else
						{
							idxEndLocal = find_wiki_tag_end(idxStart, idxEndRow);
							lenBuf = process_wiki_tag(&idxStart, idxEndLocal, sText, sBuf, lenBuf, maxLenBuf, 0);
							idxStart = idxEndLocal;
						}
						idxStart++;
					}
					bIdxIncreased = 1;
				}
				break;
			case WIKI_TAG_PAIR_TABLE_HEADER:
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					bBlankBeforeCell = 0;
					idxEndRow = find_wiki_tag_end(idxStart, idxEnd); 
					idxStart++;
					while (idxStart < idxEndRow)
					{
						if (wiki_nodes[idxStart].idxTag == WIKI_TAG_PAIR_TEXT)
						{
							pRow = wiki_nodes[idxStart].pTag;
							lenRow = wiki_nodes[idxStart].lenTag;
							while (get_next_cell(&pRow, &lenRow, "!!", sCellText))
							{
								if (bBlankBeforeCell)
								{
									lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, " ", 1);
								}
								lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<b>", 3);
								lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, sCellText, strlen(sCellText));
								lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</b>", 4);
								bBlankBeforeCell = 1;
							}
						}
						else
						{
							idxEndLocal = find_wiki_tag_end(idxStart, idxEndRow);
							lenBuf = process_wiki_tag(&idxStart, idxEndLocal, sText, sBuf, lenBuf, maxLenBuf, 0);
							idxStart = idxEndLocal;
						}
						idxStart++;
					}
					bIdxIncreased = 1;
				}
				break;
			default:
				idxEndLocal = find_wiki_tag_end(idxStart, idxEnd);
				lenBuf = process_wiki_tag(&idxStart, idxEndLocal, sText, sBuf, lenBuf, maxLenBuf, 0);
				idxStart = idxEndLocal + 1;
				bIdxIncreased = 1;
				break;
		}
		if (!bIdxIncreased)
			idxStart++;
	}
	lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</table>", 8);
	return lenBuf;
}

int unsupported_section(char *pTagContent, int lenTagContent)
{
	while (*pTagContent == ' ') // trim leading blanks
	{
		pTagContent++;
		lenTagContent--;
	}
	
	while (lenTagContent > 0 && pTagContent[lenTagContent - 1] == ' ') // trim trailing blanks
	{
		lenTagContent--;
	}
	
	if (!memcmp(pTagContent, "External links", lenTagContent) ||
		!memcmp(pTagContent, "References", lenTagContent) ||
		!memcmp(pTagContent, "See also", lenTagContent) ||
		!memcmp(pTagContent, "Further reading", lenTagContent) ||
		!memcmp(pTagContent, "Footnotes and References", lenTagContent) ||
		!memcmp(pTagContent, "Notes", lenTagContent) ||
		!memcmp(pTagContent, "Gallery", lenTagContent) ||
		!memcmp(pTagContent, "Notes and references", lenTagContent))
		return 1;
	else
		return 0;
}

long process_wiki_h2(int *idxNode, int maxNode, char *sBuf, long lenBuf, long maxLenBuf)
{
	int idxNodeContent = *idxNode + 1;
	
	if (idxNodeContent < maxNode)
	{
		if (unsupported_section(wiki_nodes[idxNodeContent].pTag, wiki_nodes[idxNodeContent].lenTag))
		{
			while (idxNodeContent < maxNode && (wiki_nodes[idxNodeContent].idxTag != WIKI_TAG_PAIR_H2 || 
				wiki_nodes[idxNodeContent].bTagStart== 0))
				idxNodeContent++;
		}
		else
		{
			if (lenBuf >= 4 && !memcmp(sBuf - 4, "<br>", 4))
			{
				if (lenBuf >= 8 && !memcmp(sBuf - 8, "<br>", 4))
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<h2>", 4);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<br><h2>", 8);
			}
			else
			{
				lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<br><br><h2>", 12);
			}	
		}
		*idxNode = idxNodeContent;
	}
	else
		(*idxNode)++;
	return lenBuf;
}

int parse_link_string(char *pTagContent, int lenTagContent, char *sLink, char *sLinkDisplay)
{
	char *p;
	int len;
	
	if (unsupported_article(pTagContent))
	{
		return -1;
	}
	else
	{
		p = strnchr(pTagContent, '|', lenTagContent);
		if (p)
		{
			len = (p - pTagContent);
			if (len < 0)
				len = 0;
			else if (len >= MAX_LINK_STRING)
				len = MAX_LINK_STRING - 1;
			memcpy(sLink, pTagContent, len);
			sLink[len] = '\0';
			
			len = lenTagContent - len - 1;
			if (len <= 0)
			{
				strcpy(sLinkDisplay, sLink);
			}
			else
			{
				if (len >= MAX_LINK_STRING)
					len = MAX_LINK_STRING - 1;
				memcpy(sLinkDisplay, p + 1, len);
				sLinkDisplay[len] = '\0';
			}
		}
		else
		{
			if (lenTagContent >= MAX_LINK_STRING)
				lenTagContent = MAX_LINK_STRING - 1;
			strncpy(sLink, pTagContent, lenTagContent);
			sLink[lenTagContent] = '\0';
			strcpy(sLinkDisplay, sLink);
		}
		return 0;
	}
}

void parse_external_link_string(char *pTagContent, int lenTagContent, char *sLinkDisplay)
{
	char *p;
	int len;
	
	p = strnchr(pTagContent, ' ', lenTagContent);
	if (p)
	{
		len = lenTagContent - (p - pTagContent + 1);
		if (len < 0)
			len = 0;
		else if (len >= MAX_LINK_STRING)
			len = MAX_LINK_STRING - 1;
		memcpy(sLinkDisplay, p + 1, len);
		sLinkDisplay[len] = '\0';
	}
	else
	{
		if (lenTagContent >= MAX_LINK_STRING)
			lenTagContent = MAX_LINK_STRING - 1;
		memcpy(sLinkDisplay, pTagContent, lenTagContent);
		sLinkDisplay[lenTagContent] = '\0';
	}
	if (!strncmp(sLinkDisplay, "http:", 5))
		sLinkDisplay[0] = '\0';
}

long process_wiki_list(int idxStart, int idxEnd, char *sText, char *sBuf, long lenBuf, long maxLenBuf)
{
	int lenBufBase;
	char lastListType;
	int i;
	
	nListStack = wiki_nodes[idxStart].lenTag;
	if (nListStack > MAX_LIST_DEPTH - 1)
		nListStack = MAX_LIST_DEPTH - 1;
	for (i = 0; i < nListStack; i++)
	{
		if (wiki_nodes[idxStart].pTag[i] == '#')
			sListStack[i] = 'O';
		else if (wiki_nodes[idxStart].pTag[i] == '*')
			sListStack[i] = 'U';
		else
			sListStack[i] = 'D';
	}
	lenBuf = process_list_stack(sListStack, nListStack, sListStackPrev, nListStackPrev, sBuf, lenBuf, maxLenBuf);
	if (idxStart <= idxEnd - 2)
	{
		lastListType = wiki_nodes[idxStart].pTag[wiki_nodes[idxStart].lenTag - 1];
		if (lastListType == '*' || lastListType == '#')
		{
			lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<li>", 4);
		}
		else if (lastListType == ';')
		{
			lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<dt>", 4);
		}
		else if (lastListType == ':')
		{
			lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<dd>", 4);
		}
		lenBufBase = lenBuf;
		idxStart++;
		idxEnd--;
		lenBuf = process_wiki_tag(&idxStart, idxEnd, sText, sBuf, lenBuf, maxLenBuf, 1);
		if (lenBuf == lenBufBase) // empty list
			lenBuf -= 4;
		else
		{
			if (lastListType == '*' || lastListType == '#')
			{
				lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</li>", 5);
			}
			else if (lastListType == ';')
			{
				lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</dt>", 5);
			}
			else if (lastListType == ':')
			{
				lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</dd>", 5);
			}
		}
	}
	memcpy(sListStackPrev, sListStack, nListStack);
	nListStackPrev = nListStack;
	return lenBuf;
}

long process_wiki_link(int idxStart, int idxEnd, char *sText, char *sBuf, long lenBuf, long maxLenBuf)
{
	char sLink[MAX_LINK_STRING];
	char sLinkDisplay[MAX_LINK_STRING];

	idxStart++;
	if (wiki_nodes[idxStart].idxTag == WIKI_TAG_PAIR_TEXT && 
		!parse_link_string(wiki_nodes[idxStart].pTag, wiki_nodes[idxStart].lenTag, sLink, sLinkDisplay))
	{
		lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<a title=\"", 10);
		lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, sLink, strlen(sLink));
		lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "\">", 2);
		lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, sLinkDisplay, strlen(sLinkDisplay));
		lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</a>", 4);
	}
	return lenBuf;
}

long process_wiki_external_link(int idxStart, int idxEnd, char *sText, char *sBuf, long lenBuf, long maxLenBuf)
{
	char sLink[MAX_LINK_STRING];
	char sLinkDisplay[MAX_LINK_STRING];

	idxStart++;
	if (wiki_nodes[idxStart].idxTag == WIKI_TAG_PAIR_TEXT)
	{
		parse_external_link_string(wiki_nodes[idxStart].pTag, wiki_nodes[idxStart].lenTag, sLinkDisplay);
		lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, sLinkDisplay, strlen(sLinkDisplay));
	}
	return lenBuf;
}

int find_wiki_tag_end(int idxStartNode, int maxIdxTag)
{
	int wiki_tag_stack[MAX_WIKI_TAG_STACK];
	int nWikiTagStack = 0;
	int idxEndNode;

	if (wiki_nodes[idxStartNode].idxTag == WIKI_TAG_PAIR_TEXT || 
		wiki_tag_pairs[wiki_nodes[idxStartNode].idxTag].lenTagEnd <= 0 ||
		!wiki_nodes[idxStartNode].bTagStart)
		idxEndNode = idxStartNode;
	else
	{
		idxEndNode = idxStartNode + 1;
		while (idxEndNode < maxIdxTag && 
			(wiki_nodes[idxEndNode].idxTag != wiki_nodes[idxStartNode].idxTag || wiki_nodes[idxEndNode].bTagStart == 1 ||
			nWikiTagStack > 0))
		{
			if (idxEndNode >= 0 && wiki_nodes[idxEndNode].idxTag != WIKI_TAG_PAIR_TEXT &&
				wiki_tag_pairs[wiki_nodes[idxEndNode].idxTag].lenTagEnd > 0)
			{
				if (wiki_nodes[idxEndNode].bTagStart == 1)
				{
					if (nWikiTagStack < MAX_WIKI_TAG_STACK)
						wiki_tag_stack[nWikiTagStack++] = wiki_nodes[idxEndNode].idxTag;
				}
				else
				{
					if (wiki_nodes[idxEndNode].idxTag == wiki_tag_stack[nWikiTagStack - 1])
						nWikiTagStack--;
				}
			}
			idxEndNode++;
		}
		if (idxEndNode > maxIdxTag)
			idxEndNode = maxIdxTag;
	}

	return idxEndNode;
}

long process_wiki_tag(int *idxNode, int maxNode, char *sText, char *sBuf, long lenBuf, long maxLenBuf, int bInList)
{
	int lenLocalBuf = 0;
	int idxStart = *idxNode;
	int idxEnd;
	int bIdxIncreased;

	if (idxStart >=0)
		idxEnd = find_wiki_tag_end(idxStart, maxNode);
	else
		idxEnd = idxStart;
		
	while (idxStart <= idxEnd)
	{
		showMsg(5, "process_wiki_tag idxStart %d, idxEnd %d, tag %d\n", idxStart, idxEnd, wiki_nodes[idxStart].idxTag);
		bIdxIncreased = 0;
		if (!bInList)
			switch (wiki_nodes[idxStart].idxTag)
			{
				case WIKI_TAG_PAIR_OL:
				case WIKI_TAG_PAIR_UL:
				case WIKI_TAG_PAIR_DT:
				case WIKI_TAG_PAIR_DD:
					break;
				default:
					nListStack = 0;
					if (nListStackPrev > 0)
					{
						lenBuf = process_list_stack(sListStack, nListStack, sListStackPrev, nListStackPrev, sBuf, lenBuf, maxLenBuf);
						nListStackPrev = 0;
					}
					break;
			}
		
		switch (wiki_nodes[idxStart].idxTag)
		{
			case WIKI_TAG_PAIR_SEP:
			case WIKI_TAG_PAIR_COMMENT:
			case WIKI_TAG_PAIR_TEMPLATE:
			case WIKI_TAG_PAIR_REF:
				idxStart = idxEnd + 1;
				bIdxIncreased = 1;
				break;
			case WIKI_TAG_PAIR_TABLE:
				if (wiki_nodes[idxStart].bTagStart)
				{
					lenBuf = process_wiki_table(idxStart, idxEnd, sText, sBuf, lenBuf, maxLenBuf);
					idxStart = idxEnd + 1;
					bIdxIncreased = 1;
				}
				break;
			case WIKI_TAG_PAIR_LINK:
				if (wiki_nodes[idxStart].bTagStart)
				{
					lenBuf = process_wiki_link(idxStart, idxEnd, sText, sBuf, lenBuf, maxLenBuf);
					idxStart = idxEnd + 1;
					bIdxIncreased = 1;
				}
				break;
			case WIKI_TAG_PAIR_EXTERNAL_LINK:
				if (wiki_nodes[idxStart].bTagStart)
				{
					lenBuf = process_wiki_external_link(idxStart, idxEnd, sText, sBuf, lenBuf, maxLenBuf);
					idxStart = idxEnd + 1;
					bIdxIncreased = 1;
				}
				break;
			case WIKI_TAG_PAIR_H6:
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<h6>", 4);
				}
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</h6>", 5);
				break;
			case WIKI_TAG_PAIR_H5:
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<h5>", 4);
				}
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</h5>", 5);
				break;
			case WIKI_TAG_PAIR_H4:
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<h4>", 4);
				}
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</h4>", 5);
				break;
			case WIKI_TAG_PAIR_H3:
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					if (lenBuf >= 8 && !memcmp(sBuf - 4, "<br>", 4))
						lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<h3>", 4);
					else
						lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<br><h3>", 8);
				}
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</h3><br><br>", 5);
				break;
			case WIKI_TAG_PAIR_H2:
				if (wiki_nodes[idxStart].bTagStart == 1)
				{
					lenBuf = process_wiki_h2(&idxStart, maxNode, sBuf, lenBuf, MAX_LOCAL_TEXT_BUF);
					bIdxIncreased = 1;
				}
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</h2><br><br>", 5);
				break;
			case WIKI_TAG_PAIR_BOLD_ITALIC:
				if (wiki_nodes[idxStart].bTagStart == 1)
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<b><i>", 6);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</i></b>", 8);
				break;
			case WIKI_TAG_PAIR_BOLD:
				if (wiki_nodes[idxStart].bTagStart == 1)
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<b>", 3);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</b>", 4);
				break;
			case WIKI_TAG_PAIR_ITALIC:
				if (wiki_nodes[idxStart].bTagStart == 1)
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<i>", 3);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</i>", 4);
				break;
			case WIKI_TAG_PAIR_NOWIKI:
				if (wiki_nodes[idxStart].bTagStart == 1)
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<nowiki>", 8);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</nowiki>", 9);
				break;
			case WIKI_TAG_PAIR_PRE:	
				if (wiki_nodes[idxStart].bTagStart == 1)
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<pre>", 5);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</pre><br>", 10);
					break;
			case WIKI_TAG_PAIR_PRE_LINE:
				if (wiki_nodes[idxStart].bTagStart == 1)
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "<pre>", 5);
				else
					lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, "</pre><br>", 10);
				break;
			case WIKI_TAG_PAIR_OL:
			case WIKI_TAG_PAIR_UL:
			case WIKI_TAG_PAIR_DT:
			case WIKI_TAG_PAIR_DD:
				if (wiki_nodes[idxStart].bTagStart)
				{
					lenBuf = process_wiki_list(idxStart, idxEnd, sText, sBuf, lenBuf, maxLenBuf);
					idxStart = idxEnd + 1;
					bIdxIncreased = 1;
				}
				break;
			case WIKI_TAG_PAIR_TEXT:
				lenBuf = cpy_wiki_buf(sBuf, lenBuf, maxLenBuf, wiki_nodes[idxStart].pTag, wiki_nodes[idxStart].lenTag);
				break;
			default:
				break;
		}
	
		if (!bIdxIncreased)
		{
			idxStart++;
		}
	}
	
	*idxNode = idxStart;
	sBuf[lenBuf] = '\0';
	return lenBuf;
}

int in_wiki_table_cells(int *wiki_tag_stack, int nWikiTagStack)
{
	int i;
	
	for (i = 0; i < nWikiTagStack; i++)
		if (wiki_tag_stack[i] == WIKI_TAG_PAIR_TABLE_CELLS)
			return 1;
	return 0;
}

void locate_wiki_tags(char *sText, long nTextLen)
{
	int wiki_tag_stack[MAX_WIKI_TAG_STACK];
	int nWikiTagStack = 0;
	char *pText = sText;
	int idxCurrentTag;
	char *pContentBeforeTag = NULL;
	long lenContentBeforeTag = 0;
	int bFound;
	int i;
	int bBeginOfLine = 1;
	char *pTemplateStart;
	char *pTemplateEnd;
	char *pCellDelimeter;
	char *pEndOfLine;

	nWikiNodeCount = 0;
	idxCurrentTag = -1;

	while (nTextLen > 0)
	{
		bFound = 0;
		if (idxCurrentTag == WIKI_TAG_PAIR_TEMPLATE) // special processing for filtering templates
		{
			while (idxCurrentTag == WIKI_TAG_PAIR_TEMPLATE && nTextLen > 0)
			{
				pTemplateStart = strnstr(pText, "{{", nTextLen);
				pTemplateEnd = strnstr(pText, "}}", nTextLen);
				if (in_wiki_table_cells(wiki_tag_stack, nWikiTagStack)) // special processing for missing }} before ||
				{
					pEndOfLine = strnstr(pText, "\n", nTextLen);
					pCellDelimeter = strnstr(pText, "||", nTextLen);
					if (pEndOfLine && pCellDelimeter && pEndOfLine < pCellDelimeter)
						pCellDelimeter = NULL;
				}
				else
					pCellDelimeter = NULL;

				if (pCellDelimeter)
				{
					if (pTemplateStart && pTemplateStart > pCellDelimeter) // no counting {{ or }} after ||
						pTemplateStart = NULL;
					if (pTemplateEnd && pTemplateEnd > pCellDelimeter) // no counting {{ or }} after ||
						pTemplateEnd = NULL;
					if (!pTemplateStart && !pTemplateEnd) // set missing }}
					{
						while (nWikiTagStack > 0 && wiki_tag_stack[nWikiTagStack - 1] == WIKI_TAG_PAIR_TEMPLATE)
						{
							nWikiTagStack--;
							wiki_nodes[nWikiNodeCount].idxTag = WIKI_TAG_PAIR_TEMPLATE;
							wiki_nodes[nWikiNodeCount].bTagStart = 0;
							wiki_nodes[nWikiNodeCount].pTag = pText - 2;
							wiki_nodes[nWikiNodeCount++].lenTag = 2;
							lenContentBeforeTag = 0;
							pContentBeforeTag = NULL;
						}
						if (nWikiTagStack > 0)
							idxCurrentTag = wiki_tag_stack[nWikiTagStack - 1];
						else
							idxCurrentTag = -1;
						continue;
					}
				}
				if (!pTemplateEnd) // no end of template found
				{
					nTextLen = 0;
					continue;
				}
				else if (pTemplateStart && pTemplateEnd && pTemplateStart < pTemplateEnd)
				{
					if (nWikiTagStack < MAX_WIKI_TAG_STACK)
						wiki_tag_stack[nWikiTagStack++] = WIKI_TAG_PAIR_TEMPLATE;
					nTextLen -= pTemplateStart - pText + 2;
					pText = pTemplateStart + 2;
				}
				else
				{
					nWikiTagStack--;
					if (nWikiTagStack > 0)
						idxCurrentTag = wiki_tag_stack[nWikiTagStack - 1];
					else
						idxCurrentTag = -1;
					if (idxCurrentTag != WIKI_TAG_PAIR_TEMPLATE)
					{
						wiki_nodes[nWikiNodeCount].idxTag = WIKI_TAG_PAIR_TEMPLATE;
						wiki_nodes[nWikiNodeCount].bTagStart = 0;
						wiki_nodes[nWikiNodeCount].pTag = pTemplateEnd;
						wiki_nodes[nWikiNodeCount++].lenTag = 2;
						lenContentBeforeTag = 0;
						pContentBeforeTag = NULL;
					}
					nTextLen -= pTemplateEnd - pText + 2;
					pText = pTemplateEnd + 2;
				}
			}
			continue;
		}
		
		if (0 <= idxCurrentTag && idxCurrentTag < WIKI_TAG_PAIR_TEXT && wiki_tag_pairs[idxCurrentTag].lenTagEnd > 0 &&
			!memcmp(pText, wiki_tag_pairs[idxCurrentTag].sTagEnd, wiki_tag_pairs[idxCurrentTag].lenTagEnd))
		{
			if (lenContentBeforeTag > 0)
			{
				wiki_nodes[nWikiNodeCount].idxTag = WIKI_TAG_PAIR_TEXT;
				wiki_nodes[nWikiNodeCount].bTagStart = 1;
				wiki_nodes[nWikiNodeCount].pTag = pContentBeforeTag;
				wiki_nodes[nWikiNodeCount++].lenTag = lenContentBeforeTag;
				lenContentBeforeTag = 0;
				pContentBeforeTag = NULL;
			}
			bFound = 1;
			wiki_nodes[nWikiNodeCount].idxTag = idxCurrentTag;
			wiki_nodes[nWikiNodeCount].bTagStart = 0;
			wiki_nodes[nWikiNodeCount].pTag = pText;
			wiki_nodes[nWikiNodeCount].lenTag = wiki_tag_pairs[idxCurrentTag].lenTagEnd;
			pText += wiki_tag_pairs[idxCurrentTag].lenTagEnd;
			nTextLen -= wiki_tag_pairs[idxCurrentTag].lenTagEnd;
			switch (idxCurrentTag)
			{
				case WIKI_TAG_PAIR_LINK:
					if (nTextLen > 0 && *pText == ']') // fix the "[[xxx]" problem
					{
						pText++;
						nTextLen--;
						wiki_nodes[nWikiNodeCount].lenTag++;
					}
					break;
				default:
					break;
			}
			nWikiNodeCount++;
			if (nWikiTagStack > 0)
			{
			 	if (idxCurrentTag == wiki_tag_stack[nWikiTagStack - 1])
				{
					nWikiTagStack--;
				}
			}
			if (nWikiTagStack > 0)
				idxCurrentTag = wiki_tag_stack[nWikiTagStack - 1];
			else
				idxCurrentTag = -1;
		}

		if (!bFound && (idxCurrentTag < 0 || idxCurrentTag >= WIKI_TAG_PAIR_TEXT || wiki_tag_pairs[idxCurrentTag].bGotChild))
		{
			for (i=0; i < MAX_WIKI_TAG_PAIRS && !bFound; i++)
			{
				if (nTextLen >= wiki_tag_pairs[i].lenTagStart && 
					!memcmp(pText, wiki_tag_pairs[i].sTagStart, wiki_tag_pairs[i].lenTagStart) &&
					(!wiki_tag_pairs[i].bBeginOfLine || bBeginOfLine))
				{
					if (lenContentBeforeTag > 0)
					{
						wiki_nodes[nWikiNodeCount].idxTag = WIKI_TAG_PAIR_TEXT;
						wiki_nodes[nWikiNodeCount].bTagStart = 1;
						wiki_nodes[nWikiNodeCount].pTag = pContentBeforeTag;
						wiki_nodes[nWikiNodeCount++].lenTag = lenContentBeforeTag;
						lenContentBeforeTag = 0;
						pContentBeforeTag = NULL;
					}
					bFound = 1;
					if (wiki_tag_pairs[i].lenTagEnd > 0)
					{
						idxCurrentTag = i;
						if (nWikiTagStack < MAX_WIKI_TAG_STACK)
							wiki_tag_stack[nWikiTagStack++] = idxCurrentTag;
					}
					else
						idxCurrentTag = -1;
					wiki_nodes[nWikiNodeCount].idxTag = i;
					wiki_nodes[nWikiNodeCount].bTagStart = 1;
					wiki_nodes[nWikiNodeCount].pTag = pText;
					wiki_nodes[nWikiNodeCount].lenTag = wiki_tag_pairs[idxCurrentTag].lenTagStart;
					pText += wiki_tag_pairs[i].lenTagStart;
					nTextLen -= wiki_tag_pairs[i].lenTagStart;
					switch (idxCurrentTag)
					{
						case WIKI_TAG_PAIR_OL:
						case WIKI_TAG_PAIR_UL:
						case WIKI_TAG_PAIR_DT:
						case WIKI_TAG_PAIR_DD:
							while (nTextLen > 0 && (*pText == '#' || *pText == '*' || *pText == ';' || *pText == ':'))
							{
								pText++;
								nTextLen--;
								wiki_nodes[nWikiNodeCount].lenTag++;
							}
							if (*pText == ' ')
							{
								pText++;
								nTextLen--;
							}
							break;
						default:
							break;
					}
					nWikiNodeCount++;
				}
			}
	
			for (i=0; i < MAX_WIKI_TAG_PAIRS && !bFound; i++)
			{
				if (wiki_tag_pairs[i].lenTagEnd && nTextLen >= wiki_tag_pairs[i].lenTagEnd &&
					strcmp(wiki_tag_pairs[i].sTagEnd, "\n") && // not special end tag
					!memcmp(pText, wiki_tag_pairs[i].sTagEnd, wiki_tag_pairs[i].lenTagEnd))
				{
					if (lenContentBeforeTag > 0)
					{
						wiki_nodes[nWikiNodeCount].idxTag = WIKI_TAG_PAIR_TEXT;
						wiki_nodes[nWikiNodeCount].bTagStart = 1;
						wiki_nodes[nWikiNodeCount].pTag = pContentBeforeTag;
						wiki_nodes[nWikiNodeCount++].lenTag = lenContentBeforeTag;
						lenContentBeforeTag = 0;
						pContentBeforeTag = NULL;
					}
					bFound = 1;
					idxCurrentTag = i;
					wiki_nodes[nWikiNodeCount].idxTag = idxCurrentTag;
					wiki_nodes[nWikiNodeCount].bTagStart = 0;
					wiki_nodes[nWikiNodeCount].pTag = pText;
					wiki_nodes[nWikiNodeCount++].lenTag = wiki_tag_pairs[idxCurrentTag].lenTagEnd;
					pText += wiki_tag_pairs[i].lenTagStart;
					nTextLen -= wiki_tag_pairs[i].lenTagStart;
					if (nTextLen > 0 && *pText == ']') // fix the "[[xxx]" problem
					{
						pText++;
						nTextLen--;
					}
					if (nWikiTagStack > 0)
					{
					 	if (idxCurrentTag == wiki_tag_stack[nWikiTagStack - 1])
						{
							nWikiTagStack--;
						}
					}
					if (nWikiTagStack > 0)
						idxCurrentTag = wiki_tag_stack[nWikiTagStack - 1];
					else
						idxCurrentTag = -1;
						}
					}
		}
		
		if (!bFound)
		{
			if (!pContentBeforeTag)
				pContentBeforeTag = pText;
			if (nTextLen > 0)
				lenContentBeforeTag++;
			pText++;
			nTextLen--;
		}
		if (*(pText - 1) == '\n')
			bBeginOfLine = 1;
		else
			bBeginOfLine = 0;
	}
	
	if (lenContentBeforeTag > 0)
	{
		wiki_nodes[nWikiNodeCount].idxTag = WIKI_TAG_PAIR_TEXT;
		wiki_nodes[nWikiNodeCount].bTagStart = 1;
		wiki_nodes[nWikiNodeCount].pTag = pContentBeforeTag;
		wiki_nodes[nWikiNodeCount++].lenTag = lenContentBeforeTag;
	}
}		

long parse_wiki_tags(char *sText, long nTextLen, char *sBuf, long maxLenBuf)
{
	int lenBuf = 0;
	int idxWikiNode = 0;

	locate_wiki_tags(sText, nTextLen);
	if (msgLevel() >= 5)
		dump_wiki_nodes();
	while (idxWikiNode < nWikiNodeCount)
	{
		if (wiki_nodes[idxWikiNode].bTagStart)
		{
			lenBuf = process_wiki_tag(&idxWikiNode, nWikiNodeCount - 1, sText, sBuf, lenBuf, maxLenBuf, 0);
		}
		else
			idxWikiNode++;
	}
			
	sBuf[lenBuf] = '\0';
	lenBuf = replace_ampersand_char_crlf(sText, sBuf);
	return lenBuf;
}		

int get_next_cell(char **pRow, int *lenRow, char *sDelimiter, char *sCellText)
{
	char *p, *p2, *p3;
	char *pNextCell;
	int lenCell;
	int rc = 0;
	char sLocalBuf[MAX_CELL_STRING];
	
	sCellText[0] = '\0';
	if (*lenRow > 0)
	{
		if (sDelimiter && (p = strnstr(*pRow, sDelimiter, *lenRow)) != NULL)
		{
			lenCell = p - (*pRow);
			(*lenRow) -= lenCell + strlen(sDelimiter);
			pNextCell = p + strlen(sDelimiter);
		}
		else
		{
			lenCell = *lenRow;
			(*lenRow) = 0;
			pNextCell = (*pRow) + lenCell;
		}
		p = strnchr(*pRow, '|', lenCell);
		p2 = strnstr(*pRow, "{{", lenCell);
		p3 = strnstr(*pRow, "[[", lenCell);
		if (p && ((p2 && p2 < p) || (p3 && p3 < p))) // resolve the confusion of '|' in {{ }} or in [[ ]]
			p = NULL;
		if (p)
		{
			lenCell -= p - (*pRow) + 1;
			if (lenCell >= MAX_CELL_STRING)
				lenCell = MAX_CELL_STRING - 1;
			memcpy(sCellText, p + 1, lenCell);
		}
		else
		{
			if (lenCell >= MAX_CELL_STRING)
				lenCell = MAX_CELL_STRING - 1;
			memcpy(sCellText, (*pRow), lenCell);
		}
		sCellText[lenCell] = '\0';
//		trim_blanks(sCellText, lenCell);
//		parse_wiki_tags(sCellText, lenCell, sLocalBuf, MAX_CELL_STRING);
		rc = 1;
		*pRow = pNextCell;
	}
	return rc;
}

int parse_list_stack(char *sListStack, int *nListStack, char **pTagContent, int *lenTagContent)
{
	int bDone = 0;
	int rc = 0;
	
	while (!bDone && *lenTagContent > 0 && *nListStack < MAX_LIST_DEPTH - 1)
	{
		switch (**pTagContent)
		{
			case '#':
				sListStack[(*nListStack)++] = 'O';
				rc = LIST_LI;
				break;
			case '*':
				sListStack[(*nListStack)++] = 'U';
				rc = LIST_LI;
				break;
			case ';':
				sListStack[(*nListStack)++] = 'D';
				rc = LIST_DT;
				break;
			case ':':
				sListStack[(*nListStack)++] = 'D';
				rc = LIST_DD;
				break;
			default:
				bDone = 1;
		}
		if (!bDone)
		{
			(*pTagContent)++;
			(*lenTagContent)--;
		}
	}
	return rc;
}

long wikiRender(MYSQL *conn, char *sRendered, long *nRenderedLines, int *nArticleLinks, int *nExternalLinks,
	char *sTitle, int nTitleLen, char *sText, long nTextLen)
{
	int i;
	ARTICLE_HEADER article_header;
	ARTICLE_LINK article_links[MAX_LINKS];
	EXTERNAL_LINK external_links[MAX_LINKS];
	char sExternalLinkStrings[MAX_LINK_STRING * MAX_LINKS];
//	int len;
	long lenExternalLinkStrings;
	long idxLinkedArticle;
	long lenRendered = 0;
	char *sBuf;

	sBuf = (char *)malloc(MAX_TEXT_BUF);
	if (!sBuf)
	{
		showMsg(0, "wikiRender malloc error\n");
		exit(-1);
	}
	render_buf.nLines = 0;
	render_buf.current_x = -1;
	render_buf.current_y = 0;
	render_buf.current_indent = 0;
	render_buf.list_depth = 0;
	render_buf.nLinks = 0;
	render_buf.nCurrentFontIdx = DEFAULT_FONT_IDX;
	render_buf.nCurrentRenderType = RENDER_TYPE_NORMAL;
	nArticleNodeCount = 0;
	nTextLen = parse_wiki_tags(sText, nTextLen, sBuf, MAX_TEXT_BUF);
	free(sBuf);
	showMsg(3, "after parse_wiki_tags\n==========\n[%s]\n==========\n", sText);
	nArticleNodeCount = 0;
	render_title(sTitle, (long)nTitleLen);
	render_wiki_text(sText, nTextLen);

	article_header.article_link_count = 0;
	article_header.external_link_count = 0;
	lenExternalLinkStrings = 0;
	for (i=0; i < render_buf.nLinks; i++)
	{
		if ((idxLinkedArticle = GetArticleIdxByTitle(conn, links[i].sLink)) >= 0)
		{
			article_links[article_header.article_link_count].start_xy = (unsigned  long)(links[i].start_x | (links[i].start_y << 8));
			article_links[article_header.article_link_count].end_xy = (unsigned  long)(links[i].end_x | (links[i].end_y << 8));
			article_links[article_header.article_link_count].article_id = idxLinkedArticle;
			article_header.article_link_count++;
		}
//		else
//		{
//			external_links[article_header.external_link_count].start_xy = (unsigned  long)(links[i].start_x | (links[i].start_y << 8));
//			external_links[article_header.external_link_count].end_xy = (unsigned  long)(links[i].end_x | (links[i].end_y << 8));
//			external_links[article_header.external_link_count].offset_link_str = lenExternalLinkStrings;
//			len = strlen(links[i].sLink);
//			external_links[article_header.external_link_count].link_str_len = len;
//			strncpy(&sExternalLinkStrings[lenExternalLinkStrings], links[i].sLink, len);
//			lenExternalLinkStrings += len;
//			article_header.external_link_count++;
//		}
	}

	article_header.offset_article = sizeof(article_header) +
		article_header.article_link_count * sizeof(ARTICLE_LINK) +
		article_header.external_link_count * sizeof(EXTERNAL_LINK) +
		lenExternalLinkStrings;
	memcpy(sRendered, &article_header, sizeof(article_header));
	lenRendered += sizeof(article_header);
	if (article_header.article_link_count)
	{
		memcpy(&sRendered[lenRendered], &article_links[0], sizeof(ARTICLE_LINK) * article_header.article_link_count);
		lenRendered += sizeof(ARTICLE_LINK) * article_header.article_link_count;
	}
	if (article_header.external_link_count)
	{
		memcpy(&sRendered[lenRendered], &external_links[0], sizeof(EXTERNAL_LINK) * article_header.external_link_count);
		lenRendered += sizeof(EXTERNAL_LINK) * article_header.external_link_count;
	}
	if (lenExternalLinkStrings)
	{
		memcpy(&sRendered[lenRendered], sExternalLinkStrings, lenExternalLinkStrings);
		lenRendered += lenExternalLinkStrings;
	}

	for (i=0; i < render_buf.nLines && lenRendered <= MAX_RENDERED - LCD_BUF_WIDTH_BYTES; i++)
	{
		memcpy(&sRendered[lenRendered], render_buf_sLines[i], strlen(render_buf_sLines[i]));
		lenRendered += strlen(render_buf_sLines[i]);
	}
	*nRenderedLines = render_buf.current_y;
	*nArticleLinks = article_header.article_link_count;
	*nExternalLinks = article_header.external_link_count;
	return lenRendered;
}

int strnChrOffset(char *s, char c, int len)
{
	int i;

	for (i=0; i < len && s[i]; i++)
		if (s[i] == c)
			return i;
	return -1;
}

void build_title_search(char *sTitle, char *sTitleSearch, char *sFirstTwoChars, char *sSecondTwoChars)
{
	int i;
	char c;

	memset (sFirstTwoChars, 0, 3);
	memset (sSecondTwoChars, 0, 3);
	i = 0;
	while ((c=*sTitle++) != 0)
	{
		if (c == '_')
			c = ' ';
		else if (c == '~')
			c = ':';
		if (is_supported_search_char(c))
		{
			switch (i)
			{
				case 0:
					sFirstTwoChars[0] = c;
					break;
				case 1:
					sFirstTwoChars[1] = c;
					break;
				case 2:
					sSecondTwoChars[0] = c;
					break;
				case 3:
					sSecondTwoChars[1] = c;
					break;
				default:
					break;
			}
			sTitleSearch[i++] = c;
		}
	}
	sTitleSearch[i] = '\0';
}

int is_redirect(char *s)
{
	char redirect[9];
	int len = 0;

	while (len < 9 && *s)
	{
		if (*s == ' ' || *s == '\t')
			s++;
		else
		{
			if ('a' <= *s && *s <= 'z')
				redirect[len] = *s - 'a' + 'A';
			else
				redirect[len] = *s;
			s++;
			len++;
		}
	}

	if (len >= 9 && !strncmp("#REDIRECT", redirect, 9))
		return 1;
	else
		return 0;
}

off64_t next_article(FILE *fd, off64_t file_offset_for_pass_1, char *sTitle, char *sRedirect,
		long *nArticleId, off64_t *nArticleOffset, long *nArticleLen, int *nType)
{
	char buf[4096];
	char *pBufS;
	char *pBufE;
	int lenBuf = 0;
	int posBuf = 0;
	char sArticleId[16];
	char sText[MAX_TITLE_LEN];
	char sTag[32];
	int nTagContentStartPos;
	int nTagContentEndPos;
	int bDone = 0;
	int bEof = 0;
	int nOffsetS;
	int nOffsetE;
	int bInTitle = 0;
	int bInArticleId = 0;
	int bInText = 0;
	int nCopyLen;
	int nTagDepth = 0;

	showMsg(3, "entering next_articlex file_offset_for_pass_1 - %ld\n", file_offset_for_pass_1);
	sTitle[0] = '\0';
	sRedirect[0] = '\0';
	sArticleId[0] = '\0';
	sText[0] = '\0';
	sTag[0] = '\0';
	*nArticleId = 0;
	*nArticleOffset = 0;
	*nArticleLen = 0;
	*nType = 0;

	fseeko64(fd, file_offset_for_pass_1, SEEK_SET);
	while (!bDone && (posBuf < lenBuf ||!bEof))
	{
		if (posBuf >= lenBuf)
		{
			lenBuf = fread(buf, 1, sizeof(buf), fd);
			bEof = feof(fd);
			if (lenBuf < 0)
				lenBuf = 0;
			posBuf = 0;
			file_offset_for_pass_1 += lenBuf;
		}
		nOffsetS = strnChrOffset(&buf[posBuf], '<', lenBuf - posBuf);
		if (nOffsetS >= 0)
			nOffsetS += posBuf;
		if (nOffsetS >=0)
		{
			nOffsetE = strnChrOffset(&buf[nOffsetS], '>', lenBuf - nOffsetS);
			if (lenBuf > nOffsetS && nOffsetE >= 0)
			{
				nOffsetE += nOffsetS;
				posBuf = nOffsetE + 1;
				if (buf[nOffsetE - 1] != '/')
				{
					nTagContentEndPos = nOffsetS - 1;
					nCopyLen = nOffsetE - nOffsetS - 1;
					if (nCopyLen > sizeof(sTag) - 1)
						nCopyLen = sizeof(sTag) - 1;
					memcpy(sTag, &buf[nOffsetS +1], nCopyLen);
					sTag[nCopyLen] = '\0';
					if (!strcmp(sTag, "page"))
						nTagDepth = 0;
					if ((nOffsetS = strnChrOffset(sTag, ' ', strlen(sTag))) >= 0)
						sTag[nOffsetS] = '\0';
					if (sTag[0] == '/' && nTagDepth > 0)
					{
						nTagDepth--;
						if (!nTagDepth)
						{
							bDone = 1;
							file_offset_for_pass_1 = file_offset_for_pass_1 - (lenBuf - nOffsetE - 1);
							fseeko64(fd, file_offset_for_pass_1, SEEK_SET);
						}
						else if (!strcmp(sTag, "/title") && nTagDepth == 1)
						{
							nCopyLen = nTagContentEndPos - nTagContentStartPos + 1;
							if (nCopyLen > MAX_TITLE_LEN - strlen(sTitle) - 1)
								nCopyLen = MAX_TITLE_LEN - strlen(sTitle) - 1;
							strncat(sTitle, &buf[nTagContentStartPos], nCopyLen);
							bInTitle = 0;
						}
						else if (!strcmp(sTag, "/id") && nTagDepth == 1)
						{
							nCopyLen = nTagContentEndPos - nTagContentStartPos + 1;
							if (nCopyLen > sizeof(sArticleId) - 1)
								nCopyLen = sizeof(sArticleId) - 1;
							strncat(sArticleId, &buf[nTagContentStartPos], nCopyLen);
							*nArticleId = atol(sArticleId);
							bInArticleId = 0;
						}
						else if (!strcmp(sTag, "/text") && nTagDepth == 2)
						{
							nCopyLen = nTagContentEndPos - nTagContentStartPos + 1;
							if (nCopyLen > MAX_TITLE_LEN - strlen(sText) - 1)
								nCopyLen = MAX_TITLE_LEN - strlen(sText) - 1;
							strncat(sText, &buf[nTagContentStartPos], nCopyLen);
							*nArticleLen = file_offset_for_pass_1 - (lenBuf - nTagContentEndPos) - *nArticleOffset + 1;
							if (is_redirect(sText))
							{
								*nType = 1;
								pBufS = strstr(sText, "[[");
								pBufE = strstr(sText, "]]");
								if (pBufS && pBufE)
								{
									nOffsetS = pBufS - sText;
									nOffsetE = pBufE - sText;
									strncat(sRedirect, &sText[nOffsetS+2], nOffsetE - nOffsetS - 2);
								}
							}
							bInText = 0;
						}
					}
					else if (nTagDepth || !strcmp(sTag, "page"))
					{
						nTagDepth++;
						if (!strcmp(sTag, "text") && nTagDepth == 3)
						{
							*nArticleOffset = file_offset_for_pass_1 - (lenBuf - nOffsetE - 1);
							bInText = 1;
						}
						else if (!strcmp(sTag, "id") && nTagDepth == 2)
							bInArticleId = 1;
						if (!strcmp(sTag, "title") && nTagDepth == 2)
							bInTitle = 1;
						nTagContentStartPos = nOffsetE + 1;
					}
				}
			}
			else
			{
				if (nOffsetS > 0)
					file_offset_for_pass_1 = file_offset_for_pass_1 - (lenBuf - nOffsetS + 1);
				fseeko64(fd, file_offset_for_pass_1, SEEK_SET);
				posBuf = lenBuf;
			}
		}
		else
		{
			if (bInTitle)
			{
				nCopyLen = lenBuf - posBuf;
				if (nCopyLen > MAX_TITLE_LEN - strlen(sTitle) - 1)
					nCopyLen = MAX_TITLE_LEN - strlen(sTitle) - 1;
				strncat(sTitle, &buf[posBuf], nCopyLen);
			}
			else if (bInArticleId)
			{
				nCopyLen = lenBuf - posBuf;
				if (nCopyLen > sizeof(sArticleId) - strlen(sArticleId) - 1)
					nCopyLen = sizeof(sArticleId) - strlen(sArticleId) - 1;
				strncat(sArticleId, &buf[posBuf], nCopyLen);
			}
			else if (bInText)
			{
				nCopyLen = lenBuf - posBuf;
				if (nCopyLen > MAX_TITLE_LEN - strlen(sText) - 1)
					nCopyLen = MAX_TITLE_LEN - strlen(sText) - 1;
				strncat(sText, &buf[posBuf], nCopyLen);
			}
			posBuf = lenBuf;
		}
	}

	if (!strncmp(sTitle, "Template:", 9))
	{
		*nType = 2;
		memcpy(sTitle, &sTitle[9], strlen(sTitle) - 8);
	}
	else if (unsupported_article(sTitle))
	{
		*nType = 3;
	}

	showMsg(3, "exiting next_article title[%s], file_offset_for_pass_1 - %Ld\n", sTitle, file_offset_for_pass_1);
	return file_offset_for_pass_1;
}

void increase_bigram(MYSQL *conn, char *sBigramChars)
{
	char sSQL[MAX_SQL_STR];

	sprintf(sSQL, "update bigram set occurrences=occurrences+1 where bigram_chars = '%s'", sBigramChars);
	mysql_query(conn, sSQL);
}

void process_pass_1(MYSQL *conn, char *sFileName, int msgLevel, long titlesToProcess,
	off64_t file_offset_for_pass_1, long max_article_idx)
{
	FILE *fd;
	char sTitle[MAX_TITLE_LEN];
	char sTitleSearch[MAX_TITLE_LEN];
	char sFirstTwoChars[3];
	char sSecondTwoChars[3];
	char sRedirect[MAX_TITLE_LEN];
	long nArticleId;
	off64_t nArticleOffset;
	long nArticleLen;
	off64_t nFileSize;
	int rc;
	int nType;
	char sSQL[MAX_SQL_STR];
	long titlesProcessed = 0;
	int bDone = 0;

	fd = fopen64(sFileName, "rb");
	if (!fd)
	{
		showMsg(0, "cannot open file %s, error: %s\n", sFileName, strerror(errno));
		exit(-1);
	}
	fseeko64(fd, 0, SEEK_END);
	nFileSize = ftello64(fd);
	processing_speed(0);
	while (file_offset_for_pass_1 < nFileSize && titlesProcessed < titlesToProcess)
	{
		file_offset_for_pass_1 = next_article(fd, file_offset_for_pass_1, sTitle, sRedirect,
			&nArticleId, &nArticleOffset, &nArticleLen, &nType);
		showMsg(3, "title[%s], redirect[%s], id[%ld], offset[%ld], len[%ld], template[%d]\n",
			sTitle, sRedirect, nArticleId, nArticleOffset, nArticleLen, nType);
		if (sTitle[0])
		{
			titlesProcessed++;
			if (file_offset_for_pass_1 >= nFileSize)
			{
				showMsg(0, "Pass 1 completed\n");
			}
			else if (nType == 0 || nType == 1)
			{
				build_title_search(sTitle, sTitleSearch, sFirstTwoChars, sSecondTwoChars);
				if (sTitleSearch[0] == '\0')
					continue;
				if (!(titlesProcessed % 100)) // sampling bigram out of every 100 entries
				{
					increase_bigram(conn, sFirstTwoChars);
					increase_bigram(conn, sSecondTwoChars);
					mysql_commit(conn); // commit periodically
				}
				if (!(titlesProcessed % 10000))
					processing_speed(titlesProcessed);
			}

			pad_backslash(sTitle, sizeof(sTitle));
			pad_backslash(sTitleSearch, sizeof(sTitleSearch));
			if (nType == 0)
			{
				max_article_idx++;
				sprintf(sSQL, "insert into entries (title, title_search, idx, "
					"entry_type, text_start_offset, text_len) values "
					"('%s', '%s', %ld, %d, %Ld, %ld)",
					sTitle, sTitleSearch, max_article_idx,
					nType, nArticleOffset, nArticleLen);
			}
			else if (nType == 1)
			{
				pad_backslash(sRedirect, sizeof(sRedirect));
				sprintf(sSQL, "insert into entries (title, title_search, "
					"entry_type, redirect_title) values"
					"('%s', '%s', %d, '%s')",
					sTitle, sTitleSearch,
					nType, sRedirect);
			}
			else
				sprintf(sSQL, "insert into entries (title, entry_type, text_start_offset, text_len) values"
					"('%s', %d, %Ld, %ld)", sTitle, nType, nArticleOffset, nArticleLen);
			rc = mysql_query(conn, sSQL);
			if (rc)
				showMsg(0, "rc=%d, SQL - %s\n", rc, sSQL);
		}
	}
	processing_speed(titlesProcessed);
	mysql_commit(conn);
}

// drop the section reference in the redirect
void drop_pound_sign(char *s)
{
	while (*s)
	{
		if (*s == '#')
			*s = '\0'; // truncate starting from #
		else
			s++;
	}
}

void create_folders_for_idx_range(long idxStart, long idxEnd)
{
	long i;
	char sPath[128];


	mkdir("./dat", 0777);
	for (i = idxStart / 1000000; i <= idxEnd / 1000000; i++)
	{
		sprintf(sPath, "./dat/%ld", i);
		mkdir(sPath, 0777);
	}
	for (i = idxStart / 10000; i <= idxEnd / 10000; i++)
	{
		sprintf(sPath, "./dat/%ld/%ld", (i / 100) % 100, i % 100);
		mkdir(sPath, 0777);
	}
	for (i = idxStart / 100; i <= idxEnd / 100; i++)
	{
		sprintf(sPath, "./dat/%ld/%ld/%ld", (i / 10000) % 100, (i / 100) % 100, i % 100);
		mkdir(sPath, 0777);
	}
}

void process_pass_2(MYSQL *conn, MYSQL *conn2, char *sFileName, int msgLevel, long titlesToProcess, int batch,
	long idxStart, long idxEnd)
{
	FILE *fdXml, *fdArticle, *fdIdx, *fdDat;
	char sRenderFile[128]; /* The file name (including path) of the rendered file for single article */
	char sTitle[MAX_TITLE_LEN];
	long idxArticle;
	off64_t nArticleOffset;
	unsigned int nRenderedArticleLen;
	unsigned int maxRenderedArticleLen = 0;
	CLzmaEncProps props;
	SizeT propsSize;
	unsigned int nCompressedArticleLen;
	unsigned int maxCompressedArticleLen = 0;
	char *sRenderedCompressed;
	long maxRenderedLines = 0;
	int maxArticleLinks = 0;
	long nRenderedLines;
	int nArticleLinks;
	int nExternalLinks;
	int rc;
	char sSQL[MAX_SQL_STR];
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *sTextBuf;
	long nTextLen;
	char *sRendered;
	int bDone;
	long idxCurEnd;
	ARTICLE_PTR *articlePtrs;
	long nIdxCount;
	long maxIdx = 0;
	long idxBase;
	long titlesProcessed = 0;

	processing_speed(0);
	init_lcd_draw_buf();
	create_folders_for_idx_range(idxStart, idxEnd);
	sTextBuf = (char *)malloc(MAX_TEXT_BUF);
	sRendered = (char *)malloc(MAX_RENDERED);
	if (!sTextBuf || !sRendered)
	{
		showMsg(0, "process_pass_2 malloc error\n");
		exit(-1);
	}
	if (!(wiki_nodes = (struct wiki_node *)malloc(sizeof(struct wiki_node) * MAX_WIKI_NODES)))
	{
		showMsg(0, "wiki_node allocation error\n");
		exit(-1);
	}
	if (!(article_nodes = (struct article_node *)malloc(sizeof(struct article_node) * MAX_ARTICLE_NODES)))
	{
		showMsg(0, "article_nodes allocation error\n");
		exit(-1);
	}
	fdXml = fopen64(sFileName, "rb");
	if (!fdXml)
	{
		showMsg(0, "cannot open file %s, error: %s\n", sFileName, strerror(errno));
		exit(-1);
	}
	if (batch >= 0)
	{
		char name[13];

		sprintf(name, "pedia%d.idx", batch);
		if (!(fdIdx = fopen(name, "wb")))
		{
			showMsg(0, "cannot open file %s, error: %s\n", name, strerror(errno));
			exit(-1);
		}
		sprintf(name, "pedia%d.dat", batch);
		if (!(fdDat = fopen(name, "wb")))
		{
			showMsg(0, "cannot open file %s, error: %s\n", name, strerror(errno));
			exit(-1);
		}
		nIdxCount = idxEnd - idxStart + 1;
		articlePtrs = (ARTICLE_PTR *)malloc(sizeof(ARTICLE_PTR) * nIdxCount);
		if (!articlePtrs)
		{
			showMsg(0, "malloc articlePtrs error\n");
			exit(1);
		}
		memset(articlePtrs, 0, sizeof(ARTICLE_PTR) * nIdxCount);
		sRenderedCompressed = (char *)malloc(MAX_RENDERED);
		if (!sRenderedCompressed)
		{
			showMsg(0, "malloc sRenderedCompressed error\n");
			exit(1);
		}
	}

	idxBase = idxStart;
	bDone = 0;
	while (!bDone)
	{
		idxCurEnd = idxStart + RESULT_SET_LIMIT - 1;
		if (idxCurEnd >= idxEnd)
		{
			idxCurEnd = idxEnd;
			bDone = 1;
		}
		sprintf(sSQL, "select title, text_start_offset, text_len, idx from entries "
			"where %ld <= idx and idx <= %ld", idxStart, idxCurEnd);
		rc = mysql_query(conn, sSQL);
		if (rc)
		{
			showMsg(0, "query entries error - %d (%s)\n", rc, mysql_error(conn));
			exit(1);
		}

		res = mysql_use_result(conn);
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			titlesProcessed++;
			if (titlesProcessed % 10000 == 0)
				processing_speed(titlesProcessed);
			strcpy(sTitle, row[0]);
			showMsg(2, "processing entry title[%s]\n", sTitle);
			sscanf(row[1], "%Ld", &nArticleOffset);
			nTextLen = atol(row[2]);
			idxArticle = atol(row[3]);
			if (idxArticle > maxIdx)
				maxIdx = idxArticle;
			sprintf(sRenderFile, "./dat/%ld/%ld/%ld/%s", (idxArticle / 1000000), (idxArticle / 10000) % 100, (idxArticle / 100) % 100, row[3]);
			fdArticle = fopen(sRenderFile, "rb");
			if (fdArticle)
			{
				nRenderedArticleLen = fread(sRendered, 1, MAX_RENDERED - 1, fdArticle);
				sRendered[nRenderedArticleLen - 1] = '\0';
				nRenderedLines = 0; // nRenderedLines is for information only. No needs to calculate it.
				nArticleLinks = 0; // nArticleLinks is for information only. No needs to calculate it.
				nExternalLinks = 0; // nExternalLinks is for information only. No needs to calculate it.
				fclose(fdArticle);
			}
			else
			{
				fseeko64(fdXml, nArticleOffset, SEEK_SET);
				nTextLen = fread(sTextBuf, 1, nTextLen, fdXml);
				sTextBuf[nTextLen] = '\0';
				showMsg(2, "before wikiRender\n");
				nRenderedArticleLen = wikiRender(conn2, sRendered, &nRenderedLines, &nArticleLinks, &nExternalLinks,
					sTitle, strlen(sTitle), sTextBuf, nTextLen);
				showMsg(2, "after wikiRender\n");
				if (nRenderedArticleLen > maxRenderedArticleLen)
					maxRenderedArticleLen = nRenderedArticleLen;
				if (nRenderedLines > maxRenderedLines)
					maxRenderedLines = nRenderedLines;
				if (nArticleLinks > maxArticleLinks)
					maxArticleLinks = nArticleLinks;
				fdArticle = fopen(sRenderFile, "wb");
				if (!fdArticle)
				{
					showMsg(0, "cannot open file %s, error: %s\n", sRenderFile, strerror(errno));
					exit(-1);
				}
				fwrite(sRendered, 1, nRenderedArticleLen, fdArticle);
				fclose(fdArticle);
			}

			if (batch >= 0)
			{
				sRendered[nRenderedArticleLen] = '\0';
				articlePtrs[idxArticle - idxBase].offset_dat = ftell(fdDat);
				LzmaEncProps_Init(&props);
				propsSize = LZMA_PROPS_SIZE;
				nCompressedArticleLen = MAX_RENDERED;
				rc = (int)LzmaEncode((Byte *)sRenderedCompressed + LZMA_PROPS_SIZE + 1, (SizeT *)&nCompressedArticleLen,
					(const Byte *)sRendered, (SizeT)nRenderedArticleLen, &props, (Byte *)sRenderedCompressed + 1, &propsSize, 0, NULL,
					&g_Alloc, &g_Alloc);
				if (rc != SZ_OK)
				{
					showMsg(0, "LzmaEncode failed - %d\n", rc);
					exit(-1);
				}
				sRenderedCompressed[0] = (char)propsSize;
				nCompressedArticleLen += LZMA_PROPS_SIZE + 1;
				if (nCompressedArticleLen > maxCompressedArticleLen)
					maxCompressedArticleLen = nCompressedArticleLen;
				articlePtrs[idxArticle - idxBase].file_id_compressed_len = batch;
				articlePtrs[idxArticle - idxBase].file_id_compressed_len = (articlePtrs[idxArticle - idxBase].file_id_compressed_len << 24) |
					(nCompressedArticleLen & 0x00FFFFFF);
				articlePtrs[idxArticle - idxBase].file_id_compressed_len |= 0x80000000; // using lzma compression
				fwrite(sRenderedCompressed, 1, nCompressedArticleLen, fdDat);
			}
			showMsg(2, "End processing entry title[%s]\n", sTitle);
		}
		mysql_free_result(res);
		idxStart += RESULT_SET_LIMIT;
	}

	processing_speed(titlesProcessed);
	mysql_commit(conn);
	free(sTextBuf);
	free(sRendered);
	free(article_nodes);
	if (batch >= 0)
	{
		nIdxCount = maxIdx - idxBase + 1;
		fwrite((void*)&nIdxCount, 1, sizeof(nIdxCount), fdIdx);
		fwrite((void*)articlePtrs, sizeof(ARTICLE_PTR), nIdxCount, fdIdx);
		fclose(fdDat);
		fclose(fdIdx);
		free(articlePtrs);
		free(sRenderedCompressed);
		showMsg(0, "maxRenderedArticleLen: %d\nmaxRenderedLines: %ld\nmaxArticleLinks: %d\nmaxCompressedArticleLen: %d\n",
			maxRenderedArticleLen, maxRenderedLines, maxArticleLinks, maxCompressedArticleLen);
	}
	else
		showMsg(0, "maxRenderedArticleLen: %d\nmaxRenderedLines: %ld\nmaxArticleLinks: %d\n",
			maxRenderedArticleLen, maxRenderedLines, maxArticleLinks);
}

extern char aBigram[128][2];

void get_bigram(MYSQL *conn)
{
	char sSQL[MAX_SQL_STR];
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc;
	int i;

	sprintf(sSQL, "select seq from bigram order by occurrences desc limit 128");
	rc = mysql_query(conn, sSQL);
	if (rc)
	{
		showMsg(0, "query entries error - %d (%s)\n", rc, mysql_error(conn));
		exit(1);
	}

	strcpy(sSQL, "select bigram_chars from bigram where seq in (0");
	res = mysql_use_result(conn);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		strcat(sSQL, ",");
		strcat(sSQL, row[0]);
	}
	strcat(sSQL, ") order by bigram_chars");
	mysql_free_result(res);

	rc = mysql_query(conn, sSQL);
	if (rc)
	{
		showMsg(0, "query entries error - %d (%s)\n", rc, mysql_error(conn));
		exit(1);
	}

	res = mysql_use_result(conn);
	i = 0;
	while (i < 128)
	{
		if ((row = mysql_fetch_row(res)) != NULL)
			memcpy(aBigram[i++], row[0], 2);
		else
			memset(aBigram[i++], 0, 2);
	}
	mysql_free_result(res);
	mysql_commit(conn);
}

void generate_pedia_idx(MYSQL *conn, long *nIdxCount, ARTICLE_PTR **articlePtrs)
{
	FILE *fdIdx[MAX_DAT_FILES];
	char idxFileName[13];
	int i;
	ARTICLE_PTR *p;
	long nCount[MAX_DAT_FILES];

	memset(nCount, 0, sizeof(nCount));
	*nIdxCount = 0;
	for (i = 0; i < MAX_DAT_FILES; i++)
	{
		sprintf(idxFileName, "pedia%d.idx", i);
		if ((fdIdx[i] = fopen(idxFileName, "rb")))
		{
			fread(&nCount[i], 1, sizeof(long), fdIdx[i]);
			*nIdxCount = *nIdxCount + nCount[i];
		}
	}

	p = (ARTICLE_PTR *)malloc(sizeof(ARTICLE_PTR) * *nIdxCount);
	if (!p)
	{
		showMsg(0, "malloc articlePtrs error\n");
		exit(1);
	}

	*articlePtrs = p;
	for (i = 0; i < MAX_DAT_FILES; i++)
	{
		if (fdIdx[i])
		{
			fread(p, sizeof(ARTICLE_PTR), nCount[i], fdIdx[i]);
			p += nCount[i];
		}
	}

	for (i = 0; i < MAX_DAT_FILES; i++)
		if (fdIdx[i])
			fclose(fdIdx[i]);
}

// pedia.idx format:
//	The first 4 bytes contain the article count.
// 	Each article got a ARTICLE_PTR structure entry in pedia.idx.
// 	The first ARTICLE_PTR entry is for article idx 1.
//
// pedia.pfx format:
// 	first three character indexing table - 54 * 54 * 54 entries * 4 bytes (long int - file offset of pedia.fnd)
//		54 characters - null + 0~9 + a~z + ...
//
// pedia.fnd format:
// 	bigram table - 128 entries * 2 bytes
//	All titles for search are sequentially concatnated into pedia.fnd in search order.  Each entry consists of (see TITLE_SEARCH_REMAINDER):
//		idx of article (pointing to pedia.idx)
//		variable length and null terminated remainder (starting from the 3rd character)
#define SIZE_FIRST_THREE_CHAR_INDEXING SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(long)
#define SEQUENTIAL_SEARCH_COUNT_THRESHOLD 16
void generate_pedia_files(MYSQL *conn, int bSplitted)
{
	FILE *fdPfx, *fdFnd, *fdIdx;
	long offset_fnd;
	int rc;
	char sSQL[MAX_SQL_STR];
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sLastTitleSearch[MAX_TITLE_LEN];
	long *firstThreeCharIndexing;
	TITLE_SEARCH titleSearch;
	int idxFirstThreeCharIndexing;
	int lastIdxFirstThreeCharIndexing = -1;
	int nEntryType;
	char c1, c2, c3;
	long lastIdxArticle;
	ARTICLE_PTR *articlePtrs;
	long nIdxCount;
	char t1, t2, t3, sStart[4], sEnd[4];
	long nTitlesProcessed = 0;
	int nSequentialSearchCounts[MAX_SEARCH_STRING_HASHED_LEN];
	long offsetSequentialSearch[MAX_SEARCH_STRING_HASHED_LEN];
	long hashSequentialSearch[MAX_SEARCH_STRING_HASHED_LEN];
	int i, j;
	int bMatched;
	int lenMatched;

	fdIdx = fopen("pedia.idx", "wb");
	if (!fdIdx)
	{
		showMsg(0, "cannot open file pedia.idx, error: %s\n", strerror(errno));
		exit(-1);
	}
	fdPfx = fopen("pedia.pfx", "wb");
	if (!fdPfx)
	{
		showMsg(0, "cannot open file pedia.pfx, error: %s\n", strerror(errno));
		exit(-1);
	}
	fdFnd = fopen("pedia.fnd", "wb");
	if (!fdFnd)
	{
		showMsg(0, "cannot open file pedia.fnd, error: %s\n", strerror(errno));
		exit(-1);
	}
	init_search_hash();

	rc = mysql_query(conn, "select idx "
		"from entries where idx is not null order by idx desc limit 1");
	if (rc)
	{
		showMsg(0, "query entries idx error - %d (%s)\n", rc, mysql_error(conn));
		exit(1);
	}
	res = mysql_use_result(conn);
	if ((row = mysql_fetch_row(res)) != NULL)
		nIdxCount = atol(row[0]);
	else
	{
		showMsg(0, "no idx found\n");
		exit(1);
	}
	mysql_free_result(res);

	generate_pedia_idx(conn, &nIdxCount, &articlePtrs);

	firstThreeCharIndexing = (long *)malloc(SIZE_FIRST_THREE_CHAR_INDEXING);
	fwrite(&aBigram[0][0], 1, SIZE_BIGRAM_BUF, fdFnd);

	sLastTitleSearch[0] = '\0';
	lastIdxArticle = 0;
	memset((void*)firstThreeCharIndexing, 0, SIZE_FIRST_THREE_CHAR_INDEXING);
	for (i=0; i < MAX_SEARCH_STRING_HASHED_LEN; i++)
	{
		nSequentialSearchCounts[i] = 0;
		offsetSequentialSearch[i] = 0;
		hashSequentialSearch[i] = 0;
	}

	sStart[3] = '\0';
	sEnd[3] = '\0';

	for (t1 = '0'; t1 <= 'Z'; t1++)
		for (t2 = '0'; t2 <= 'Z'; t2++)
			for (t3 = '0'; t3 <= 'Z'; t3++)
	{
		if (bSplitted)
		{
			if (t1 == '0' && t2 == '0' && t3 == '0')
			{
				sEnd[0] = t1;
				sEnd[1] = t2;
				sEnd[2] = t3;
				sprintf(sSQL,
					"select e1.idx, e1.entry_type, e1.title_search, e2.idx "
					"from entries e1 "
					"left outer join entries e2 on (e1.entry_type = 1 && e2.title = e1.redirect_title) "
					"where (e1.entry_type=0 or e1.entry_type=1) and "
					"	e1.title_search < '000'"
					"order by e1.title_search, e1.entry_type, e1.idx, e2.idx");
			}
			else if (t1 == 'Z' && t2 == 'Z' && t3 == 'Z')
			{
				strcpy(sEnd, sStart);
				sprintf(sSQL,
					"select e1.idx, e1.entry_type, e1.title_search, e2.idx "
					"from entries e1 "
					"left outer join entries e2 on (e1.entry_type = 1 && e2.title = e1.redirect_title) "
					"where (e1.entry_type=0 or e1.entry_type=1) and "
					"	'ZZZ' <= e1.title_search "
					"order by e1.title_search, e1.entry_type, e1.idx, e2.idx");
			}
			else
			{
				strcpy(sStart, sEnd);
				sEnd[0] = t1;
				sEnd[1] = t2;
				sEnd[2] = t3;
				sprintf(sSQL,
					"select e1.idx, e1.entry_type, e1.title_search, e2.idx "
					"from entries e1 "
					"left outer join entries e2 on (e1.entry_type = 1 && e2.title = e1.redirect_title) "
					"where (e1.entry_type=0 or e1.entry_type=1) and "
					"	'%s' <= e1.title_search and e1.title_search < '%s'"
					"order by e1.title_search, e1.entry_type, e1.idx, e2.idx", sStart, sEnd);
			}
		}
		else
		{
			if (t1 == '0' && t2 == '0' && t3 == '0')
			{
				sprintf(sSQL,
					"select e1.idx, e1.entry_type, e1.title_search, e2.idx "
					"from entries e1 "
					"left outer join entries e2 on (e1.entry_type = 1 && e2.title = e1.redirect_title) "
					"where (e1.entry_type=0 or e1.entry_type=1)"
					"order by e1.title_search, e1.entry_type, e1.idx, e2.idx");
			}
			else
				sSQL[0] = '\0';
		}

		if (sSQL[0])
		{
			rc = mysql_query(conn, sSQL);
			if (rc)
			{
				showMsg(0, "query entries idx error - %d (%s)\n", rc, mysql_error(conn));
				exit(1);
			}

			res = mysql_use_result(conn);
			while ((row = mysql_fetch_row(res)) != NULL)
			{
				nTitlesProcessed++;
				showMsg(3, "%ld [%s][%s][%s][%s]\n", nTitlesProcessed, row[0], row[1], row[2], row[3]);
				nEntryType = atoi(row[1]); // entry_type
				if (nEntryType == 1) // Redirected article
				{
					if (row[3])
						titleSearch.idxArticle = atol(row[3]);
					else
						titleSearch.idxArticle = 0;
				}
				else
				{
					if (row[0])
						titleSearch.idxArticle = atol(row[0]);  // idx
					else
						titleSearch.idxArticle = 0;
				}
				if (!titleSearch.idxArticle ||
					(strlen(sLastTitleSearch) == strlen(row[2]) &&
					!search_string_cmp(row[2], sLastTitleSearch, strlen(sLastTitleSearch)) && 
					titleSearch.idxArticle == lastIdxArticle))
				{
					continue; // skipping redundant title for search
				}

				lastIdxArticle = titleSearch.idxArticle;
				switch (strlen(row[2])) // title_search
				{
					case 0:
						c1 = '\0';
						c2 = '\0';
						c3 = '\0';
						break;
					case 1:
						c1 = row[2][0];
						c2 = '\0';
						c3 = '\0';
						break;
					case 2:
						c1 = row[2][0];
						c2 = row[2][1];
						c3 = '\0';
						break;
					default:
						c1 = row[2][0];
						c2 = row[2][1];
						c3 = row[2][2];
						break;
				}
				bigram_encode(titleSearch.sTitleSearch, row[2]);
				idxFirstThreeCharIndexing = bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT + 
					bigram_char_idx(c2) * SEARCH_CHR_COUNT + bigram_char_idx(c3);
				offset_fnd = ftell(fdFnd);

				lenMatched = 0;
				if (strlen(sLastTitleSearch) > 3)
				{
					int lenLastTitleSearch = strlen(sLastTitleSearch);
					int lenTitleSearch = strlen(row[2]);
					int len;
					char sSearchString[MAX_SEARCH_STRING_HASHED_LEN];
					
					// only care the length up to MAX_SEARCH_STRING_HASHED_LEN
					if (lenLastTitleSearch > MAX_SEARCH_STRING_HASHED_LEN)
						lenLastTitleSearch = MAX_SEARCH_STRING_HASHED_LEN;
					if (lenTitleSearch > MAX_SEARCH_STRING_HASHED_LEN)
						lenTitleSearch = MAX_SEARCH_STRING_HASHED_LEN;
					memcpy(sSearchString, sLastTitleSearch, lenLastTitleSearch);
					
					bMatched = 1;
					for (len = 4; len <= lenLastTitleSearch || len <= lenTitleSearch; len++)
					{
						if (bMatched)
						{
							if (len > lenTitleSearch || search_string_cmp(row[2], sLastTitleSearch, len))
								bMatched = 0;
							else
							{
								lenMatched = len;
								if (!hashSequentialSearch[len - 1])
									nSequentialSearchCounts[len - 1] += 1;
							}
						}

						if (len <= lenLastTitleSearch && nSequentialSearchCounts[len - 1] > SEQUENTIAL_SEARCH_COUNT_THRESHOLD)
						{
							hashSequentialSearch[len - 1] = add_search_hash(sSearchString, len, offsetSequentialSearch[len - 1]);
							// reset sequential counts 
							for (j = len; j <= lenLastTitleSearch; j++)
							{
								nSequentialSearchCounts[j - 1] = 0;
							}
						}
					}
				}

				i = lenMatched;
				while (i < MAX_TITLE_LEN && row[2][i])
				{
					if ('A' <= row[2][i] && row[2][i] <= 'Z')
						sLastTitleSearch[i] = row[2][i] + 32;
					else
						sLastTitleSearch[i] = row[2][i];
					if (3 <= i && i < MAX_SEARCH_STRING_HASHED_LEN)
					{
						offsetSequentialSearch[i] = offset_fnd;
						nSequentialSearchCounts[i] = 0;
						hashSequentialSearch[i] = 0;
					}
					i++;
				}
				sLastTitleSearch[i] = '\0';

				if (idxFirstThreeCharIndexing != lastIdxFirstThreeCharIndexing)
				{
					if (c2 != '\0' && firstThreeCharIndexing[bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT] == 0)
					{
						firstThreeCharIndexing[bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT] = offset_fnd;
					}
					if (c3 != '\0' && 
						firstThreeCharIndexing[bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT + 
						bigram_char_idx(c2) * SEARCH_CHR_COUNT] == 0)
					{
						firstThreeCharIndexing[bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT + 
							bigram_char_idx(c2) * SEARCH_CHR_COUNT] = offset_fnd;
					}
					firstThreeCharIndexing[idxFirstThreeCharIndexing] = offset_fnd;
					lastIdxFirstThreeCharIndexing = idxFirstThreeCharIndexing; 
				}

				if (nEntryType == 0 && titleSearch.idxArticle)
					articlePtrs[titleSearch.idxArticle - 1].offset_fnd = offset_fnd;
				titleSearch.cZero = '\0';
				fwrite(&titleSearch, 1, sizeof(titleSearch.idxArticle) + sizeof(titleSearch.cZero) +
					strlen(titleSearch.sTitleSearch) + 1, fdFnd);
			}
			mysql_free_result(res);
		}
	}
	showMsg(0, "Titles processed: %ld\n", nTitlesProcessed);
	fseek(fdPfx, 0, SEEK_SET);
	fwrite((void*)firstThreeCharIndexing, 1, SIZE_FIRST_THREE_CHAR_INDEXING, fdPfx);
	fwrite((void*)&nIdxCount, 1, sizeof(nIdxCount), fdIdx);
	fwrite((void*)articlePtrs, sizeof(ARTICLE_PTR),  nIdxCount, fdIdx);
	save_search_hash();
	free(firstThreeCharIndexing);
	free(articlePtrs);
	fclose(fdPfx);
	fclose(fdFnd);
	fclose(fdIdx);
}

void process_pass_3(MYSQL *conn, MYSQL *conn2, int bSplitted)
{
	init_char_idx();
	get_bigram(conn);
	generate_pedia_files(conn, bSplitted);
}

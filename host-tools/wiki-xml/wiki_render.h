
#ifndef _WIKI_RENDERED_H
#define _WIKI_RENDERED_H
#include <mysql/mysql.h>

#define MAX_TITLE_LEN 512
#define MAX_SQL_STR 2048
#define MAX_TEXT_BUF 5*1024*1024
#define MAX_RENDERED 10*1024*1024
#define MAX_TITLE_SEARCH 64
#define MAX_DAT_FILES 64
#define MAX_ARTICLES_PER_DAT 250000
#define RESULT_SET_LIMIT 200
#define TABLE_CELL_WIDTH (LCD_BUF_WIDTH_PIXELS / 4)

typedef struct _ARTICLE_PTR {
	long offset_dat;	/* offset to pedia?.dat for the article content */
	long offset_fnd;	/* offset to pedia.fnd for the title (for search) */
	long file_id_compressed_len; 	/* byte 0: bit0~1 - compress method (00 - lzo, 01 - bzlib, 10 - 7z), bit 2~7 pedia file id */
					/* byte 1~3: compressed article length */
} ARTICLE_PTR;

typedef struct _TITLE_SEARCH { /* used to mask the porinter to the remainder of the title for search */
	long idxArticle;
	char cZero; /* null character for backward search */
	char sTitleSearch[MAX_TITLE_SEARCH]; /* null terminated title for search */
	char sTitleActual[MAX_TITLE_SEARCH]; /* null terminated actual title. */
						/* in the fnd file, it will be concatnated immediately after the null terminator of sTitleSearch */
} TITLE_SEARCH;

void render_article_node(int idxNode);
void process_pass_1(MYSQL *conn, char *sFileName, int msgLevel, long titlesToProcess,
	off64_t file_offset_for_pass_1, long max_article_idx);
void process_pass_2(MYSQL *conn, MYSQL *conn2, char *sFileName, int msgLevel, long titlesToProcess, int batch,
	long idxStart, long idxEnd);
void process_pass_3(MYSQL *conn, MYSQL *conn2, int bSplitted);
void memrcpy(void *dest, void *src, int len); // memory copy starting from the last byte
void replace_ampersand_char(char *);
void pad_backslash(char *s, int size);
void generate_pedia_hsh(void);
void reorg_pedia(void);
extern void showMsg(int currentLevel, char *format, ...);
extern int msgLevel(void);

#endif /* _WIKI_RENDERED_H */

/*
 * (C) Copyright 2009 OpenMoko, Inc.
 * Author: Christopher Hall <hsw@openmoko.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <getopt.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>
#include "wiki_render.h"

char *framebuffer;
unsigned char *article_buf_pointer;

void fb_refresh(void) {}

static void help(void)
{
	printf("Usage: wiki-xml-parser [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -f --file\t\t\tXML file name to process (default \"enwiki-latest-pages-articles.xml\")\n"
		"  -p --pass\t\t\tPass 0, 1, 2, 3, 4 or 5 (default 0)\n"
		"\t\t\t0 - Print article content\n"
		"\t\t\t1 - Scan HTML files and log in database\n"
		"\t\t\t2 - Render articles and generate both single article files and dat files\n"
		"\t\t\t3 - Generate ouptput files\n"
		"\t\t\t4 - Count the rendered article files (generated in pass 3)\n"
		"  -n --new\t\t\tStart from scratch (for pass 1).  If not specified, default to continue from the last processed.\n"
		"  -b --batch\t\t\tBatch (for pass 3, 0~7, 0 - 1~249999, 1 - 250000~499999, 2 - 500000~999999, etc)\n"
		"  -g --begin\t\t\tBeginning idx to process (for pass 3)\n"
		"  -e --end\t\t\tEnding idx to process (for pass 3)\n"
		"  -m --messages\t\t\tMessage level (default 0, for no messages)\n"
		"  -u --user\t\t\tDatabase user id (default root)\n"
		"  -w --password\t\t\tDatabase password (default NULL)\n"
		"  -s --server\t\t\tDatabase server (default localhost)\n"
		"  -d --database\t\t\tDatabase name (default wikixml)\n"
		"  -i --idx\t\t\tIdx of the entry to retrieve (for pass 0, default 1)\n"
		"  -l --title\t\t\tTitle to retrieve (for pass 0)\n"
		"  -q --seq\t\t\tSeq of the entry to retrieve (for pass 0)\n"
		);
}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "file", 1, 0, 'f' },
	{ "pass", 1, 0, 'p' },
	{ "new", 0, 0, 'n' },
	{ "batch", 0, 0, 'b' },
	{ "begin", 1, 0, 'g' },
	{ "end", 1, 0, 'e' },
	{ "titles", 1, 0, 't' },
	{ "messages", 1, 0, 'm' },
	{ "user", 1, 0, 'u' },
	{ "password", 1, 0, 'w' },
	{ "server", 1, 0, 's' },
	{ "database", 1, 0, 'd' },
	{ "idx", 1, 0, 'i' },
	{ "title", 1, 0, 'l' },
	{ "seq", 1, 0, 'q' },
	{ NULL, 0, NULL, 0 }
};

int nMsgLevel;

int msgLevel()
{
	return nMsgLevel;
}

void showMsg(int currentLevel, char *format, ...)
{
	va_list ap;

	if (currentLevel > nMsgLevel)
		return;

//printf("[%ld]", clock());
        va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}

void init_bigram_table(MYSQL *conn)
{
	char sSQL[MAX_SQL_STR];
	int rc;
	char c1, c2;
	
	mysql_query(conn, "delete from bigram");
	mysql_query(conn, "alter table bigram AUTO_INCREMENT=1");
	mysql_commit(conn);
	for (c1 = '0'; c1 <= '9'; c1++)
	{
		for (c2 = '0';  c2 <= '9'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
		for (c2 = 'A';  c2 <= 'Z'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
		for (c2 = 'a';  c2 <= 'z'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
	}
	for (c1 = 'A'; c1 <= 'Z'; c1++)
	{
		for (c2 = '0';  c2 <= '9'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
		for (c2 = 'A';  c2 <= 'Z'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
		for (c2 = 'a';  c2 <= 'z'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
	}
	for (c1 = 'a'; c1 <= 'z'; c1++)
	{
		for (c2 = '0';  c2 <= '9'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
		for (c2 = 'A';  c2 <= 'Z'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
		for (c2 = 'a';  c2 <= 'z'; c2++)
		{
			sprintf(sSQL, "insert bigram (bigram_chars, occurrences) values ('%c%c', 0)",
				c1, c2);
			rc = mysql_query(conn, sSQL);
		}
	}
	mysql_commit(conn);
}

#define MAX_IDX_RANGE 1000

void add_idx_to_range(long idx, long *idx_range_count, long idx_range[MAX_IDX_RANGE][2])
{
	int nMatch = -1;
	int i;
	
	for (i = 0; i < *idx_range_count && nMatch < 0; i++)
	{
		if (idx < idx_range[i][0])
			nMatch = i;
	}
	if (nMatch >= 0)
	{
		if (idx == idx_range[nMatch][0] - 1)
			idx_range[nMatch][0] = idx;
		else
		{
			if (*idx_range_count < MAX_IDX_RANGE)
				*idx_range_count = *idx_range_count + 1;
			memrcpy(&idx_range[nMatch + 1][0], &idx_range[nMatch][0], sizeof(long) * 2 * (*idx_range_count - nMatch - 1));
			idx_range[nMatch][0] = idx;
			idx_range[nMatch][1] = idx;
		}
	}
	else
	{
		if (*idx_range_count == 0)
		{
			idx_range[0][0] = idx;
			idx_range[0][1] = idx;
			*idx_range_count = *idx_range_count + 1;
		}
		else if (idx == idx_range[*idx_range_count - 1][1] + 1)
		{
			idx_range[*idx_range_count - 1][1] = idx;
		}
		else if (*idx_range_count < MAX_IDX_RANGE)
		{
			idx_range[*idx_range_count][0] = idx;
			idx_range[*idx_range_count][1] = idx;
			*idx_range_count = *idx_range_count + 1;
		}
	}	
}

#define MAX_FILE_PER_FOLDER 100
void add_file_name(long name, char type, long file_names[MAX_FILE_PER_FOLDER], char file_types[MAX_FILE_PER_FOLDER], int *file_count)
{
	int i;
	int nMatch = -1;
	
	for (i = 0; i < *file_count && nMatch < 0; i++)
		if (name < file_names[i])
			nMatch = i;
	if (nMatch >= 0)
	{
		if (*file_count < MAX_FILE_PER_FOLDER)
			*file_count = *file_count + 1;
		memrcpy(&file_names[nMatch + 1], &file_names[nMatch], sizeof(long) * (*file_count - nMatch - 1));
		memrcpy(&file_types[nMatch + 1], &file_types[nMatch], sizeof(char) * (*file_count - nMatch - 1));
		file_names[nMatch] = name;
		file_types[nMatch] = type;
	}
	else
	{
		if (*file_count < MAX_FILE_PER_FOLDER)
		{
			file_names[*file_count] = name;
			file_types[*file_count] = type;
			*file_count = *file_count + 1;
		}
	}	
}		

void count_files(char *dir, long *idx_range_count, long idx_range[MAX_IDX_RANGE][2], long *max_file_size)
{
	struct dirent *de = NULL;
	DIR *d = NULL;
	char path[256];
	long file_names[MAX_FILE_PER_FOLDER];
	char file_types[MAX_FILE_PER_FOLDER];
	int file_count = 0;
	int i;
	struct stat file_stat;
	
	d = opendir(dir);
	if(d == NULL)
		return;
		
	// Loop while not NULL
	while ((de = readdir(d)) != NULL)
	{
		if (de->d_name[0] != '.')
		{
			add_file_name(atol(de->d_name), de->d_type, file_names, file_types, &file_count);
		}
	}
	
	for (i = 0; i < file_count; i++)
	{
		if (file_types[i] == DT_DIR)
		{
			sprintf(path, "%s/%ld", dir, file_names[i]);
			count_files(path, idx_range_count, idx_range, max_file_size);
		}
		else if (file_types[i] == DT_REG)
		{
			add_idx_to_range(file_names[i], idx_range_count, idx_range);
			sprintf(path, "%s/%ld", dir, file_names[i]);
			stat(path, &file_stat);
			if (file_stat.st_size > *max_file_size)
				*max_file_size = file_stat.st_size;
		}
	}

	closedir(d);
}

MYSQL *g_conn;

int main(int argc, char **argv)
{
	int pass = 0;
	int bNew = 0;
	long batch = -1;
	long idxStart = 0;
	long idxEnd = 0;
	long idx = 1;
	long seq = 0;
	long titlesToProcess = 0;
	char sFileName[1024];
	char sServer[256];
	char sDB[256];
	char sId[256];
	char sPassword[256];
	char sTitle[256];
	off64_t file_offset_for_pass_1;
	long max_article_idx;
	MYSQL *conn;
	MYSQL *conn2;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sSQL[MAX_SQL_STR];
	int rc;
	char c;
	time_t t;

	nMsgLevel = 0;
	printf("wiki-xml-parser - (C) 2009 by OpenMoko Inc.\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n\n");

	if (argc <2)
	{
		help();
		exit(2);
	}

	sFileName[0] = '\0';
	sServer[0] = '\0';
	sDB[0] = '\0';
	sId[0] = '\0';
	sPassword[0] = '\0';
	sTitle[0] = '\0';
	while (1) {
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hf:p:nb:g:e:m:t:u:w:s:d:i:l:q:", opts,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			help();
			exit(0);
			break;
		case 'f':
			strncpy(sFileName, optarg, sizeof(sFileName) - 1);
			sFileName[sizeof(sFileName) - 1] = '\0';
			break;
		case 'p':
			pass = atoi(optarg);
			break;
		case 'b':
			batch = atoi(optarg);
			break;
		case 'g':
			idxStart = atol(optarg);
			break;
		case 'e':
			idxEnd = atol(optarg);
			break;
		case 'n':
			bNew = 1;
			break;
		case 'm':
			nMsgLevel = atoi(optarg);
			break;
		case 't':
			titlesToProcess = atol(optarg);
			break;
		case 'u':
			strncpy(sId, optarg, sizeof(sId) - 1);
			sId[sizeof(sId) - 1] = '\0';
			break;
		case 'w':
			strncpy(sPassword, optarg, sizeof(sPassword) - 1);
			sPassword[sizeof(sPassword) - 1] = '\0';
			break;
		case 's':
			strncpy(sServer, optarg, sizeof(sServer) - 1);
			sServer[sizeof(sServer) - 1] = '\0';
			break;
		case 'd':
			strncpy(sDB, optarg, sizeof(sDB) - 1);
			sDB[sizeof(sDB) - 1] = '\0';
			break;
		case 'i':
			idx = atol(optarg);
			break;
		case 'q':
			seq = atol(optarg);
			break;
		case 'l':
			strncpy(sTitle, optarg, sizeof(sTitle) - 1);
			sServer[sizeof(sTitle) - 1] = '\0';
			break;
		default:
			help();
			exit(2);
		}
	}

	if (!titlesToProcess)
		titlesToProcess = 1024;
	if (!sFileName[0])
		strcpy(sFileName, "enwiki-latest-pages-articles.xml");
	if (!sId[0])
		strcpy(sId, "root");
	if (!sServer[0])
		strcpy(sServer, "localhost");
	if (!sDB[0])
		strcpy(sDB, "wikixml");
	if (batch >= 0)
	{
		idxStart = MAX_ARTICLES_PER_DAT * batch;
		idxEnd = idxStart + MAX_ARTICLES_PER_DAT - 1;
		if (!idxStart)
			idxStart = 1;
	}
	else if (idxEnd < idxStart)
		idxEnd = idxStart;

	my_init();
	g_conn = mysql_init(NULL);
	if (!mysql_real_connect(g_conn, sServer, sId, sPassword, sDB, 0, NULL, 0))
	{
		showMsg(0, "Error connecting DB %s/%s using %s/%s\n", sServer, sDB, sId, sPassword);
		exit(1);
	}
	conn = mysql_init(NULL);
	if (!mysql_real_connect(conn, sServer, sId, sPassword, sDB, 0, NULL, 0))
	{
		showMsg(0, "Error connecting DB %s/%s using %s/%s\n", sServer, sDB, sId, sPassword);
		exit(1);
	}
	
	if (pass==0)
	{
		if (sTitle[0])
			sprintf(sSQL, "select title, text_start_offset, text_len, redirect_title, entry_type from entries where title='%s'", sTitle);
		else if (seq)
			sprintf(sSQL, "select title, text_start_offset, text_len, redirect_title, entry_type from entries where seq=%ld", seq);
		else
			sprintf(sSQL, "select title, text_start_offset, text_len, redirect_title, entry_type from entries where idx=%ld", idx);
		printf("SQL: %s\n", sSQL);
		rc = mysql_query(conn, sSQL);
		if (rc)
		{
			showMsg(0, "query entries error - %d (%s)\n", rc, mysql_error(conn));
			exit(1);
		}
		
		res = mysql_use_result(conn);
		if ((row = mysql_fetch_row(res)) != NULL)
		{
			FILE *fd;
			off64_t nArticleOffset;
			long nTextLen;
			char buf[1025];
			int len;
			
			if (!row[1])
			{
				if (row[4])
					printf("title: [%s], redirect: [%s]\n", row[0], row[3]);
				else
					printf("title: [%s], entry type: %s\n", row[0], row[4]);
				exit(0);
			}
			sscanf(row[1], "%Ld", &nArticleOffset);
			nTextLen = atol(row[2]);
			printf("offset: %Lx, len: %lx\n\n", nArticleOffset, nTextLen);
			fd = fopen64(sFileName, "rb");
			fseeko64(fd, nArticleOffset, SEEK_SET);
			printf("<mediawiki>\n<page>\n<title>%s</title>\n <revision>\n<text>", row[0]);
			while (nTextLen > 0)
			{
				if (nTextLen > 1024)
					len = fread(buf, 1, 1024, fd);
				else
					len = fread(buf, 1, nTextLen, fd);
				nTextLen -= 1024;
				buf[len] = '\0';
				printf(buf);
			}
			printf("\n</text>\n</revision>\n</page>\n</mediawiki>");
		}
		else
			printf("not found\n");
		exit(0);
	}
	else if (pass == 4)
	{
		long idx_range[MAX_IDX_RANGE][2];
		long idx_range_count = 0;
		long count = 0;
		int i;
		long max_file_size = 0;
		
		count_files("./dat", &idx_range_count, idx_range, &max_file_size);
		for (i = 0; i < idx_range_count; i++)
		{
			showMsg(0, "%8ld ~ %8ld\n", idx_range[i][0], idx_range[i][1]);
			count += idx_range[i][1] - idx_range[i][0] + 1;
		}
		showMsg(0, "Total file count: %ld, Max file size: %ld\n", count, max_file_size);
		exit(0);
	}
	
	rc = mysql_query(conn, "select idx from entries order by idx desc limit 1");
	res = mysql_use_result(conn);
	if ((row = mysql_fetch_row(res)) != NULL && row[0])
	{
		showMsg(0, "max article idx:\t\t%ld\n", (max_article_idx=atol(row[0])));
		mysql_free_result(res);
		mysql_commit(conn);
		rc = mysql_query(conn, "select count(*) from entries");
		res = mysql_use_result(conn);
		if ((row = mysql_fetch_row(res)) != NULL && row[0])
			showMsg(0, "pass1 processed entries:\t%ld\n", atol(row[0]));
//		mysql_free_result(res);
//		mysql_commit(conn);
//		rc = mysql_query(conn, "select count(*) from entries where entry_type=0 and pass_2_processed=1");
//		res = mysql_use_result(conn);
//		if ((row = mysql_fetch_row(res)) != NULL && row[0])
//			showMsg(0, "pass2 processed articles:\t%ld\n", atol(row[0]));
//		mysql_free_result(res);
//		mysql_commit(conn);
//		rc = mysql_query(conn, "select count(*) from entries where entry_type=0 and pass_2_processed=0");
//		res = mysql_use_result(conn);
//		if ((row = mysql_fetch_row(res)) != NULL && row[0])
//			showMsg(0, "pass2 to be processed articles:\t%ld\n", atol(row[0]));
	}
	else
	{
		showMsg(0, "no process status\n");
		file_offset_for_pass_1 = 0;
		max_article_idx = 0;
		init_bigram_table(conn);
	}
	mysql_free_result(res);

	if (pass == 1)
	{
//		if (nBatch <= 0)
//			nBatch = 8;
//		else if (nBatch > MAX_DAT_FILES)
//			nBatch = MAX_DAT_FILES;
		showMsg(0, "\nProcessing %s for %ld titles.\n", sFileName, titlesToProcess);
	}
	else if (pass == 2)
		showMsg(0, "\nProcessing idx %ld ~ %ld.\n", idxStart, idxEnd);
	showMsg(3, "Server-%s, DB-%s, user-%s, password-%s.\n\n", sServer, sDB, sId, sPassword);

	if (bNew && pass == 1)
		printf("\nAll previously processed records will be deleted.  Continue processing? (y to continue)\n");
	else
		printf("\nStart processing? (y to continue)\n");
	c = getchar();
	if (c == 'y')
	{
		if (bNew)
		{
			if (pass == 1)
			{
				mysql_query(conn, "delete from entries");
				rc = mysql_commit(conn);
				mysql_query(conn, "alter table entries AUTO_INCREMENT=1");
				rc = mysql_commit(conn);
				showMsg(3, "commit update process_status rc=%d\n", rc);
				file_offset_for_pass_1 = 0;
				max_article_idx = 0;
				init_bigram_table(conn);
			}
		}
		
		time(&t);	
		showMsg(0, "start pass %d - %s\n", pass, ctime(&t));
		if (pass == 1)
		{
			process_pass_1(conn, sFileName, nMsgLevel, titlesToProcess,
				file_offset_for_pass_1, max_article_idx);
		}
		else if (pass == 2)
		{
			conn2 = mysql_init(NULL);
			if (!mysql_real_connect(conn2, sServer, sId, sPassword, sDB, 0, NULL, 0))
			{
				showMsg(0, "Error connecting DB %s/%s using %s/%s\n", sServer, sDB, sId, sPassword);
				exit(1);
			}
			process_pass_2(conn, conn2, sFileName, nMsgLevel, titlesToProcess, batch, idxStart, idxEnd);
			mysql_close(conn2);
		}
		else if (pass == 3)
		{
			conn2 = mysql_init(NULL);
			if (!mysql_real_connect(conn2, sServer, sId, sPassword, sDB, 0, NULL, 0))
			{
				showMsg(0, "Error connecting DB %s/%s using %s/%s\n", sServer, sDB, sId, sPassword);
				exit(1);
			}
			unlink("pedia.idx");
			unlink("pedia.fnd");
			unlink("pedia.pfx");
			if (max_article_idx > 2000)
				process_pass_3(conn, conn2, 1);
			else
				process_pass_3(conn, conn2, 0);
			mysql_close(conn2);
		}
		time(&t);
		showMsg(0, "end pass %d - %s\n", pass, ctime(&t));
	}
	/* close connection */
	mysql_close(conn);
	mysql_close(g_conn);
	
	exit(0);
}

int retrieve_article(long idx_article)
{
	// dummy function for linking lcd_buf_draw.c
	return 0;
}

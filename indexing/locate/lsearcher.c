/*
 * GPL from wikipedia-iphonw
 * Patrick Collison <patrick@collison.ie>
 * Josef Weidendorfer <Josef.Weidendorfer@gmx.de>
 */
/*
 * Copyright (c) 1995 Wolfram Schneider <wosch@FreeBSD.org>. Berlin.
 * Copyright (c) 1989, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * James A. Woods.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/usr.bin/locate/locate/fastfind.c,v 1.12.2.1 2001/02/22 13:28:52 wosch Exp $
 * $DragonFly: src/usr.bin/locate/locate/fastfind.c,v 1.3 2005/08/04 17:31:23 drhodus Exp $
 */

#include <ctype.h>
#include <string.h>
#include "lsearcher.h"

int kill_switch;
ucaddr_t addr = 0;

void kill_search() {
    kill_switch = 1;
}

int check_bigram_char(int ch) {
    /* legal bigram: 0, ASCII_MIN ... ASCII_MAX */
    if (ch == 0 || (ch >= ASCII_MIN && ch <= ASCII_MAX))
        return(ch);

    fatal("locate database header corrupt, bigram char outside 0, %d-%d: %d",  
            ASCII_MIN, ASCII_MAX, ch);
    exit(1);
}

char *tolower_word(char *word) {
    register char *p;

    for(p = word; *p != '\0'; p++)
        *p = TOLOWER(*p);

    return(word);
}

int matches = 0;

bool handle_match(uchar_t *s) {
    printf("%s\n", s);
    return ++matches != 10;
}

/*
 * read a complete block and serve getc from this block...
 */
static int blocks_read = 0;

#define BLOCK_ALIGNMENT 0xff
static uchar_t block[256];
static int bytes_available = 0;
static int eof = 0;

static void read_block(int fd)
{
    ++blocks_read;
    bytes_available = read(fd, &block, sizeof(block));
    eof = bytes_available != sizeof(block);
}

static int l_getc(int fd)
{
    if (bytes_available == 0 && !eof)
        read_block(fd);

    if (bytes_available <= 0)
        return EOF;
    return block[sizeof(block) - bytes_available--];
}

static int l_getw(int fd)
{
    int result = 0;
    result |= l_getc(fd) <<  0;
    result |= l_getc(fd) <<  8;
    result |= l_getc(fd) << 16;
    result |= l_getc(fd) << 24;

    return result;
}

static void l_lseek(int fd, off_t offset, int whence)
{
    lseek(fd, offset & ~BLOCK_ALIGNMENT, whence);

    /*
     * now read from this block and update bytes_available
     * as we don't want to directly start there...
     */
    read_block(fd);

    /* how much do we have left */
    if (bytes_available < (offset & BLOCK_ALIGNMENT))
        bytes_available = 0;
    else
        bytes_available -= offset & BLOCK_ALIGNMENT;
}

static off_t l_tell(int fd)
{
    int offset = lseek(fd, 0, SEEK_CUR);
    return offset - bytes_available;
}

void init_index(lindex *l, int db_file, int prefix_file) {
    uchar_t *p, *s;
    int c;

    for (c = 0, p = l->bigram1, s = l->bigram2; c < NBG; c++) {
        p[c] = check_bigram_char(l_getc(db_file));
        s[c] = check_bigram_char(l_getc(db_file));
    }

    /* database */
    l->db_file = db_file;
    l->db_start = l_tell(l->db_file);

    /* prefix table */
    if (prefix_file != -1)
        read(prefix_file, &l->prefixdb, sizeof(l->prefixdb));
}

void load_index(lindex *l, char *path, char *ppath) {
    int db, offset_file = -1;

    debug("load_index(0x%p, %s, %s)", l, path, ppath);

    db = open(path, O_RDONLY);
    if(ppath) offset_file = open(ppath, O_RDONLY);

    init_index(l, db, offset_file);

    if (offset_file != -1)
        close(offset_file);
}

/*
 * extract the start positions of 0-9 and A-Z from the index
 * the switch happens when the count is 0, meaning nothing
 * from the previous string should be taken...
 */
void scan(lindex *l, char *scan_file) {
    int c;
    int count = 0;
    off_t file_offset = 0;
    uchar_t path[MAXSTR];
    uchar_t *p;

    /* don't bother supporting >2gb... an index file that large
     * would have search times so high as to be useless 
     */

    debug("scanning through plenty of chars...");

    file_offset = 1;
    c = l_getc(l->db_file);
    for (; c != EOF; ) {
        off_t this_offset = file_offset - 1;

        /* go forward or backward */
        if (c == SWITCH) { /* big step, an integer */
            count +=  l_getw(l->db_file) - OFFSET;
            file_offset += sizeof(int);
        } else {	   /* slow step, =< 14 chars */
            count += c - OFFSET;
        }

        /* overlay old path */
        p = path + count;


        /* nothing got reused -> first char is different */
        if (count == 0) {
            for (;;) {
                c = l_getc(l->db_file);
                ++file_offset;
                /*
                 * == UMLAUT: 8 bit char followed
                 * <= SWITCH: offset
                 * >= PARITY: bigram
                 * rest:      single ascii char
                 *
                 * offset < SWITCH < UMLAUT < ascii < PARITY < bigram
                 */
                if (c < PARITY) {
                    if (c <= UMLAUT) {
                        if (c == UMLAUT) {
                            c = l_getc(l->db_file);
                            ++file_offset;
                        } else
                            break; /* SWITCH */
                    }
                    *p++ = c;
                } else {		
                    /* bigrams are parity-marked */
                    TO7BIT(c);
                    *p++ = l->bigram1[c];
                    *p++ = l->bigram2[c];
                }
            }

            *p-- = '\0';
            if(isalnum(path[0])) {
                /* ignore unicode chars for now */
                l->prefixdb[path[0]] = this_offset;
                debug("%c starts at 0x%x", path[0], (int)this_offset);
            }
        } else {
            /* skip stuff... until the next switch... */
            for (;;) {
                c = l_getc(l->db_file);
                ++file_offset;
                if (c < PARITY && c <= UMLAUT) {
                    if (c == UMLAUT) {
                        c = l_getc(l->db_file);
                        ++file_offset;
                    } else
                        break; /* SWITCH */
                }
            }
        }
    }

    /* write it out */
    FILE *fp = fopen(scan_file, "w");
    if (!fp)
        return;

    fwrite(l->prefixdb, sizeof(uint32_t), CHAR_MAX, fp);
    fclose(fp);
}


/*
 * Include almost the same function as search_fast and search_slow
 *  - One cheats and is fast
 *  - One is slow and doesn't cheat...
 *  - We don't mix the two paths for performance reasons...
 */
#define LOOKUP_FAST
#include "lookup.c"
#undef LOOKUP_FAST

#define LOOKUP_SLOW
#include "lookup.c"
#undef lOOKUP_SLOW

#ifdef INCLUDE_MAIN
void usage(char *prog) {
    fatal("%s -f <indexFile> [-c <scanFile>] [-s <search>]", prog);
}

int main(int argc, char **argv) {
    extern char *optarg;
    char scanFile[MAXSTR], indexFile[MAXSTR], needle[MAXSTR];
    unsigned char ch; 
    bool doScan = false, doSearch = false, haveScanFile = false, icase = true, twoRuns = false;
    lindex l;
    memset(&l, 0, sizeof(l));

    debug = false;

    while ((ch = getopt(argc, argv, "dac:s:f:hnp")) != 255) {
        switch (ch) {
        case 'c':
            haveScanFile = true;
            strncpy(scanFile, optarg, MAXSTR);
            break;
        case 'n':
            doScan = true;
            break;
        case 'd':
            debug = true;
            break;
        case 'f':
            strncpy(indexFile, optarg, MAXSTR);
            break;
        case 's':
            doSearch = true;
            strncpy(needle, optarg, MAXSTR);
            break;
        case 'a':
            icase = false;
            break;
        case 'p':
            twoRuns = true;
            break;
        case 'h':
        default:
            usage(argv[0]); 
        }
    }

    if(indexFile) 
        load_index(&l, indexFile, (haveScanFile && doSearch) ? scanFile : NULL);
    else {
        debug("no index file");
        usage(argv[0]);
    }

    if(doScan)
        scan(&l, scanFile);
    else if(doSearch) {
        search_fast(&l, needle, handle_match, NULL, icase);
        debug("During the search %d blocks were read", blocks_read);
        if (twoRuns) {
            blocks_read = 0;
            search_slow(&l, needle, handle_match, NULL, icase);
            debug("During the slow search %d blocks were read", blocks_read);
        }
    } else {
        debug("no action");
        usage(argv[0]);
    }

    return 0;
}
#endif

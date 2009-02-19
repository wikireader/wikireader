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
#include <limits.h>
#include <string.h>
#include <file-io.h>
#include "lsearcher.h"

#ifdef INCLUDE_MAIN
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#endif

static int blocks_read = 0;

#define BLOCK_ALIGNMENT 0x1ff
static uchar_t block[512];
static int bytes_available = 0;
static unsigned int _l_offset = 0;
static int eof = 0;


int check_bigram_char(int ch) {
    /* legal bigram: 0, ASCII_MIN ... ASCII_MAX */
    if (ch == 0 || (ch >= ASCII_MIN && ch <= ASCII_MAX))
        return(ch);

#ifdef INCLUDE_MAIN
    fatal("locate database header corrupt, bigram char outside 0, %d-%d: %d",  
            ASCII_MIN, ASCII_MAX, ch);
    exit(1);
#else
    return -1;
#endif
}

/*
 * read a complete block and serve getc from this block...
 */
unsigned int lsesrch_consume_block_stat()
{
    unsigned int blocks = blocks_read;
    blocks_read = 0;
    return blocks;
}

static void read_block(int fd)
{
    ++blocks_read;
    bytes_available = wl_read(fd, &block, sizeof(block));
    eof = bytes_available != sizeof(block) || bytes_available < 0;
    _l_offset += bytes_available;
}

int l_getc(int fd)
{
    if (bytes_available == 0 && !eof)
        read_block(fd);

    if (bytes_available <= 0)
        return EOF;
    return block[sizeof(block) - bytes_available--];
}

int l_getw(int fd)
{
    int result = 0;
    result |= l_getc(fd) <<  0;
    result |= l_getc(fd) <<  8;
    result |= l_getc(fd) << 16;
    result |= l_getc(fd) << 24;

    return result;
}

void l_lseek(int fd, unsigned int offset)
{
    _l_offset = offset & ~BLOCK_ALIGNMENT;
    wl_seek(fd, _l_offset);

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

unsigned int l_offset(int fd)
{
    return _l_offset - bytes_available;
}

#define SIZE_OF(array) (sizeof(array)/sizeof(array[0]))

void init_index(lindex *l, int db_file, int prefix_file) {
    uchar_t *p, *s;
    int c;

    for (c = 0, p = l->bigram1, s = l->bigram2; c < NBG; c++) {
        p[c] = check_bigram_char(l_getc(db_file));
        s[c] = check_bigram_char(l_getc(db_file));
    }

    /* database */
    l->db_file = db_file;
    l->db_start = sizeof(l->bigram1) + sizeof(l->bigram2);

    /* prefix table */
    if (prefix_file != -1) {
        int r = 0;
        unsigned int i = 0;
        uint32_t *prefixdb = &l->prefixdb[0];
        uint32_t *bigramdb = &l->bigram[0];

        for (i = 0;  i < SIZE_OF(l->prefixdb); ++i)
            r += wl_read(prefix_file, prefixdb + i, sizeof(l->prefixdb[0]));

        for (i = 0; i < SIZE_OF(l->bigram); ++i)
            r += wl_read(prefix_file, bigramdb + i, sizeof(l->bigram[0]));

        if (r != sizeof(l->prefixdb) + sizeof(l->bigram)) {
#ifdef INCLUDE_MAIN
            printf("Failed...to read prefix, bigram.\n");
#endif
        }
    }
}

int load_index(lindex *l, char *path, char *ppath) {
    int db, offset_file = -1;

#ifdef INCLUDE_MAIN
    debug("load_index(0x%p, %s, %s)", l, path, ppath);
#endif

    db = wl_open(path, WL_O_RDONLY);
    if (db < 0)
        return false;

    if(ppath) offset_file = wl_open(ppath, WL_O_RDONLY);

    init_index(l, db, offset_file);

    if (offset_file != -1)
        wl_close(offset_file);

    if (!ppath)
        return db >= 0;
    return db >= 0 && offset_file >= 0;
}

void reset_state(lindex *l, struct search_state *target, const struct search_state *source)
{
    memcpy(target, source, sizeof(*target));
    target->offset = target->this_offset;
    target->skip = true;
    l_lseek(l->db_file, l->db_start + target->offset);
    target->last_c = l_getc(l->db_file);
}

void store_state(lindex *l, const struct search_state *source, struct search_state *dest)
{
    memcpy(dest, source, sizeof(*dest));
}

#ifdef INCLUDE_MAIN
static bool handle_match(uchar_t *s) {
    printf("%s first block: %d\n", s, blocks_read);
    return true;
}

/*
 * extract the start positions of 0-9 and A-Z from the index
 * the switch happens when the count is 0, meaning nothing
 * from the previous string should be taken...
 */
static void scan(lindex *l, char *scan_file) {
    int c;
    int count = 0;
    unsigned int file_offset = 0;
    uchar_t path[MAXSTR];
    uchar_t *p;

    /* don't bother supporting >2gb... an index file that large
     * would have search times so high as to be useless 
     */

    unsigned int i;
    for (i = 0; i < sizeof(l->prefixdb)/sizeof(l->prefixdb[0]); ++i)
        l->prefixdb[i] = INT_MAX;
    for (i = 0; i < sizeof(l->bigram)/sizeof(l->bigram[0]); ++i)
        l->bigram[i] = INT_MAX;

    file_offset = 1;
    c = l_getc(l->db_file);
    for (; c != EOF; ) {
        unsigned int this_offset = file_offset - 1;

        /* go forward or backward */
        if (c == SWITCH) { /* big step, an integer */
            count +=  l_getw(l->db_file) - OFFSET;
            file_offset += sizeof(int);
        } else {	   /* slow step, =< 14 chars */
            count += c - OFFSET;
        }

        /* overlay old path */
        p = path + count;


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

        int index_1 = char_to_index(toupper(path[0]));
        int index_2 = char_to_index(toupper(path[1]));

        if (index_1 == -1) {
            printf("Unhandled char for prefix: '%c' at 0x%x\n",
                   path[0], (int)this_offset);
        } else if (l->prefixdb[index_1] == INT_MAX) {
            l->prefixdb[index_1] = this_offset;
            debug("%c '%s' starts at 0x%x index: %d",
                  path[0], path, (int)this_offset, index_1);
        }

        if (index_2 == -1) {
            printf("Unhandled char for prefix: '%c' '%c' at 0x%x (%d, %d)\n",
                    path[0], path[1], (int)this_offset, index_1, index_2);
        } else if (index_1 >= 0 && l->bigram[create_index(index_1, index_2)] == INT_MAX) {
            l->bigram[create_index(index_1, index_2)] = this_offset;
            debug("%c%c starts at 0x%x index: %d %d %d", path[0], path[1],
                  (int)this_offset, create_index(index_1, index_2), index_1, index_2);
        }
    }

    /* write it out */
    int fp = open(scan_file, O_WRONLY|O_CREAT|O_TRUNC, 0655);
    if (fp < 0)
        return;

    int ret = 0;
    ret += write(fp, &l->prefixdb, sizeof(l->prefixdb));
    ret += write(fp, &l->bigram, sizeof(l->bigram));

    if (ret != sizeof(l->prefixdb) + sizeof(l->bigram))
        fprintf(stderr, "Failed to write db\n");
}


/*
 * Include almost the same function as search_fast and search_slow
 *  - One cheats and is fast
 *  - One is slow and doesn't cheat...
 *  - We don't mix the two paths for performance reasons...
 */
void usage(char *prog) {
    fatal("%s -f <indexFile> [-c <scanFile>] [-s <search>]", prog);
}

int main(int argc, char **argv) {
    extern char *optarg;
    char scanFile[MAXSTR], indexFile[MAXSTR], needle[MAXSTR];
    unsigned char ch; 
    bool doScan = false, doSearch = false, haveScanFile = false, twoRuns = false;
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
        case 'p':
            twoRuns = true;
            break;
        case 'h':
        default:
            usage(argv[0]); 
        }
    }

    if(!load_index(&l, indexFile, (haveScanFile && doSearch) ? scanFile : NULL)) {
        debug("no index file");
        usage(argv[0]);
    }

    if(doScan)
        scan(&l, scanFile);
    else if(doSearch) {
        struct search_state state;
        char *result;
        prepare_search(&l, needle, &state);

        while ((result = search_fast(&l, needle, &state))) {
            handle_match(result);
        }

        debug("During the search %d blocks were read", blocks_read);
        if (twoRuns) {
            blocks_read = 0;
            search_slow(&l, needle, &state, handle_match, NULL);
            debug("During the slow search %d blocks were read", blocks_read);
        }
    } else {
        debug("no action");
        usage(argv[0]);
    }

    return 0;
}
#endif

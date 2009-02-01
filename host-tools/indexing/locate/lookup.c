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
#include <file-io.h>
#include "lsearcher.h"

#if defined(LOOKUP_SLOW)
int search_slow
#elif defined(LOOKUP_FAST)
void prepare_search(lindex *l, char *pathpart, struct search_state *state) {
    state->offset = INT_MAX;
    state->skip = false;
    state->pattern_len = strlen(pathpart);

    if (state->offset == INT_MAX && state->pattern_len > 1) {
        int index_1 = char_to_index(toupper(pathpart[0]));
        int index_2 = char_to_index(toupper(pathpart[1]));
        if (index_1 >= 0 && index_2 >= 0) {
            int index = create_index(index_1, index_2);
            state->offset = l->bigram[index];
            state->path[0] = toupper(pathpart[0]);
            if (state->offset != l->prefixdb[index_1])
                state->count = 1;
        }
    }

    if (state->offset == INT_MAX) {
        state->offset = char_to_index(toupper(*pathpart));
        if (state->offset >= 0)
            state->offset = l->prefixdb[state->offset];
        else
            return;
    }

    if(l->prefixdb && state->offset != INT_MAX) {
        debug("using prefix db seek to 0x%x", state->offset);
        l_lseek(l->db_file, l->db_start + state->offset);
        state->skip = true;
    } else {
        printf("Failed to seek... not searching\n");
        return ;
    }
}

int search_fast
#endif
(lindex *l, char *pathpart, struct search_state *state, resultf f, donef df) {
    register uchar_t *p, *s, *patend, *q;
    register int c, cc;

    patend = (uchar_t *)(pathpart + strlen(pathpart) - 1);
    cc = *patend;

#if defined(LOOKUP_SLOW)
    int found;
    register uchar_t *foundchar;
    uchar_t *cutoff = NULL;
    uchar_t lower_patend = 0xff;
    uchar_t upper_patend = 0xff;
    lower_patend = TOLOWER(*patend);
    upper_patend = toupper(*patend);

    /* main loop */
    found = state->count = 0;
#endif


#if defined(LOOKUP_SLOW)
    /* go back */
    l_lseek(l->db_file, l->db_start);
    state->skip = false;
    foundchar = 0;
#endif

    c = l_getc(l->db_file);
    for (; c != EOF; ) {
        if (c == SWITCH) {
            int local_count =  l_getw(l->db_file) - OFFSET;
            if(!state->skip)
                state->count += local_count;
        } else if(!state->skip) {
            state->count += c - OFFSET;
        }

        p = state->path + state->count;

#if defined(LOOKUP_FAST)
        state->skip = false;
#elif defined(LOOKUP_SLOW)
        foundchar = p - 1;
#endif

        for (;;) {
            c = l_getc(l->db_file);
            /*
             * == UMLAUT: 8 bit char followed
             * <= SWITCH: offset
             * >= PARITY: bigram
             * rest:      single ascii char
             *
             * offset < SWITCH < UMLAUT < ascii < PARITY < bigram
             */
            if (__builtin_expect(c < PARITY, true)) {
                if (__builtin_expect(c < UMLAUT, false)) {
                    break; /* SWITCH */
                } else if  (__builtin_expect(c == UMLAUT, false)) {
                    c = l_getc(l->db_file);
                }
#if defined(LOOKUP_SLOW)
                if (__builtin_expect(c == lower_patend || c == upper_patend, true))
                    foundchar = p;
#endif
                *p++ = c;
            }
            else {	
                /* bigrams are parity-marked */
                TO7BIT(c);

                p[0] = l->bigram1[c];
                p[1] = l->bigram2[c];

#if defined(LOOKUP_SLOW)
                if (__builtin_expect((p[0] == upper_patend || p[0] == lower_patend ||
                    p[1] == upper_patend || p[1] == lower_patend), false))
                    foundchar = p + 1;
#endif

                p += 2;
            }
        }



#if defined(LOOKUP_FAST)
        *p = '\0';
        bool all_bigger = true;
        int longest_match = 0;
        int i = 0;
        for(s = (uchar_t *)state->path, q = (uchar_t *)pathpart, i = 0; *q; s++, q++, ++i) {
            /*
             * Okay some ugly states....
             * 1.) the first letter is always capital so we special case 0
             * 2.) for every but the last charachter we can match with >=
             *     (common prefix) only the last char of the pattern needs
             *     to be bigger. becuase from this point on everything else
             *     will get bigger too.
             */
            if (i != 0 && all_bigger) {
                if (i != state->pattern_len-1 && *s >= *q)
                    ++longest_match;
                else if (i == state->pattern_len-1 && *s > *q)
                    ++longest_match;
                else
                    all_bigger = false;
            } else if (i == 0) {
                if (*s > toupper(*q)) {
                    ++longest_match;
                } 
            }

            if(TOLOWER(*s) != *q)
                break;
        }

        if (longest_match >= 1 && all_bigger)
            return -1;

        if(*q == '\0') {
            if(!f(state->path))
                return 0;
        }
#elif defined(LOOKUP_SLOW)
        if (found) {
            cutoff = state->path;
            *p-- = '\0';
            foundchar = p;
        } else if (foundchar >= state->path + state->count) {
            *p-- = '\0';
            cutoff = state->path + state->count;
        } else {
            continue;
        }

        found = 0;
        for (s = foundchar; s >= cutoff; s--) {
            if (*s == cc || TOLOWER(*s) == cc) {
                for (p = patend - 1, q = s - 1; *p != '\0'; p--, q--)
                    if (*q != *p && TOLOWER(*q) != *p)
                        break;
                if (*p == '\0' && \
                         strncasecmp((const char *)state->path, pathpart, strlen(pathpart))) {
                    found = 1;
                    if(!f(state->path))
                        return 0;
                    break;
                }
            }
        }
#endif
    }

    if(df)
        df();
    return 0;
}


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

int
#if defined(LOOKUP_SLOW)
search_slow
#elif defined(LOOKUP_FAST)
search_fast
#endif
(lindex *l, char *pathpart, resultf f, donef df, bool icase) {
    register uchar_t *p, *s, *patend, *q;
    register int c, cc;
    int count, found;
    uchar_t path[MAXSTR];

    patend = (uchar_t *)(pathpart + strlen(pathpart) - 1);
    cc = *patend;

#if defined(LOOKUP_SLOW)
    register uchar_t *foundchar;
    uchar_t *cutoff = NULL;
    uchar_t lower_patend = 0xff;
    uchar_t upper_patend = 0xff;
    if (icase) {
        lower_patend = TOLOWER(*patend);
        upper_patend = toupper(*patend);
    } else {
        /* well... it does not really matter */
        lower_patend = *patend;
    }
#endif

    /* main loop */
    found = count = 0;

    /* go back */
    l_lseek(l->db_file, l->db_start, SEEK_SET);

#if defined(LOOKUP_FAST)
    debug("checking for prefixdb... %p", l->prefixdb);
    int offset = -1;
    bool skip = false;

    offset = l->prefixdb[toupper(*pathpart)];
    debug("offset: %d", offset);
    if(l->prefixdb && (offset > 0)) {
        debug("using prefix db");
        l_lseek(l->db_file, l->db_start + offset, SEEK_SET);
        skip = true;
    }
#else
    static bool const skip = false;
    foundchar = 0;
#endif:

    c = l_getc(l->db_file);
    for (; c != EOF; ) {
        if (c == SWITCH) {
            int local_count =  l_getw(l->db_file) - OFFSET;
            if(!skip)
                count += local_count;
        } else if(!skip) {
            count += c - OFFSET;
        }

        p = path + count;

#if defined(LOOKUP_FAST)
        skip = false;
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
        for(s = (uchar_t *)path, q = (uchar_t *)pathpart; *q; s++, q++)
            if((icase && TOLOWER(*s) != *q) || (!icase && *s != *q))
                break;
        if(*q == '\0') {
            if(!f(path))
                return 0;
        }
#elif defined(LOOKUP_SLOW)
        if (found) {
            cutoff = path;
            *p-- = '\0';
            foundchar = p;
        } else if (foundchar >= path + count) {
            *p-- = '\0';
            cutoff = path + count;
        } else {
            continue;
        }

        found = 0;
        for (s = foundchar; s >= cutoff; s--) {
            if (*s == cc || (icase && TOLOWER(*s) == cc)) {
                for (p = patend - 1, q = s - 1; *p != '\0'; p--, q--)
                    if (*q != *p && (!icase || TOLOWER(*q) != *p))
                        break;
                if (*p == '\0' && \
                        (icase ? \
                         strncasecmp((const char *)path, pathpart, strlen(pathpart)) : \
                         strncmp((const char *)path, pathpart, strlen(pathpart)))) {
                    found = 1;
                    if(!f(path))
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


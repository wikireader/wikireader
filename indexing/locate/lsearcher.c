#include "lsearcher.h"

int kill_switch;
ucaddr_t addr = 0;

void kill_search() {
  kill_switch = 1;
}

void search_progress(lindex *l, double *d) {
  double prog = ((double) (addr - l->addr)) / ((double) l->size);
  debug("prog: %lf", prog);
  *d = prog;
}

int check_bigram_char(int ch) {
    /* legal bigram: 0, ASCII_MIN ... ASCII_MAX */
    if (ch == 0 || (ch >= ASCII_MIN && ch <= ASCII_MAX))
        return(ch);

    fatal("locate database header corrupt, bigram char outside 0, %d-%d: %d",  
        ASCII_MIN, ASCII_MAX, ch);
    exit(1);
}

uchar_t *tolower_word(uchar_t *word) {
    register uchar_t *p;

    for(p = word; *p != '\0'; p++)
        *p = TOLOWER(*p);

    return(word);
}

int matches = 0;

bool handle_match(uchar_t *s) {
  printf("%s\n", s);
  return true;
}

void init_index(lindex *l, ucaddr_t adr, size_t len, uint32_t *pdb) {
    if (len < (2*NBG))
        fatal("database too small");
  uchar_t *p, *s;
  int c;
    
    for (c = 0, p = l->bigram1, s = l->bigram2; c < NBG; c++, len-= 2) {
        p[c] = check_bigram_char(*adr++);
        s[c] = check_bigram_char(*adr++);
    }

  l->addr = adr;
  l->size = len;

  if(pdb) l->prefixdb = pdb;
}

void load_index(lindex *l, char *path, char *ppath) {
  size_t size;
  uchar_t *adr;
  uint32_t *paddr = NULL;

  debug("load_index(0x%x, %s, %s)", l, path, ppath);

  adr = xmmapf(path, &size);
  if(ppath) paddr = xmmapf(ppath, 0);

  init_index(l, adr, size, paddr);
}

void scan(lindex *l, char *path) {
  int len = l->size;
  int c;
  uchar_t *adr = l->addr;
  int count = 0;
  uint32_t prefixdb[CHAR_MAX] = {0};

  /* don't bother supporting >2gb... an index file that large
   * would have search times so high as to be useless 
   */

  debug("scanning through %d chars...", len);

  c = *adr++;
  len--;
  while(len > 0) {
    uchar_t *this_switch = adr - 1;

    if(c == SWITCH) {
      count += *(int*) adr - OFFSET;
      len -= sizeof(int);
      adr += sizeof(int);
    } else {
      count += c - OFFSET;
    }

    if(count == 0) {
      char x[4];
      int i = 0;
      c = *adr++; len--;
      for(; i < 4; i++) {
        c = *(adr + i - 1);
        if(c >= PARITY) {
          TO7BIT(c);
          x[i] = l->bigram1[c];
        } else if(c > UMLAUT)
          x[i] = c;
        else if(c == UMLAUT) {
          x[i] = *(adr + i);
          adr++; len--;
        }
      }

      if(isalnum(x[0])) {
        /* ignore unicode chars for now */
        prefixdb[x[0]] = this_switch - l->addr;
        debug("%c %d starts at 0x%x (unicode: %4s)", x[0], x[0], this_switch - l->addr, x);
      }
    }

    while(len-- > 0 && (c = *adr++) >= UMLAUT); 
  }

  FILE *fp = xfopen(path, "w");
  fwrite(prefixdb, sizeof(uint32_t), CHAR_MAX, fp);
  fclose(fp);
}

uchar_t *pathprep(uchar_t *path) {
  uchar_t *patend;
  size_t patlen = strlen(path);
    uchar_t *patptr = xalloc(patlen + 2);
  *patptr = '\0';
  strncpy(patptr, path, patlen);
  patend = patptr + patlen - 1;
  *(patptr + patlen) = '\0';
  return patend;
}

int search(lindex *l, uchar_t *pathpart, resultf f, donef df, bool icase, bool strict) {
    register uchar_t *p, *s, *patend, *q, *foundchar;
    register int c, cc;
    int count, found;
  size_t len = l->size;
  addr = l->addr;
    uchar_t *cutoff, path[MAXSTR];
  
  kill_switch = 0;

    /* use a lookup table for case insensitive search */
    uchar_t table[UCHAR_MAX + 1];

    if(icase) tolower_word(pathpart);

  patend = pathprep(pathpart);
    cc = *patend;

    /* set patend char to true */
  for (c = 0; c < UCHAR_MAX + 1; c++)
    table[c] = 0;

    if(icase) {
    table[TOLOWER(*patend)] = 1;
      table[toupper(*patend)] = 1;
  } else
    table[*patend] = 1;

    /* main loop */
    found = count = 0;
    foundchar = 0;

  debug("checking for prefixdb... %p", l->prefixdb);
  int offset = -1;
  bool skip = false;
  if(l->prefixdb) offset = *(l->prefixdb + toupper(*pathpart));
  debug("offset: %d", offset);
  if(strict && l->prefixdb && (offset > 0)) {
    debug("using prefix db");
    addr = l->addr + offset;
    len -= offset;
    skip = true;
  }

    c = *addr++;
  len--;
  while(len > 0) {
    if(kill_switch)
      return -1;

    if (c == SWITCH) {
      if(!skip)
    count += *(int*) addr - OFFSET;
      len -= sizeof(int);
      addr += sizeof(int);
    } else if(!skip) {
      count += c - OFFSET;
    }

    skip = false;

    p = path + count;
    foundchar = p - 1;

    while(len > 0) {
      if(kill_switch)
        return -1;

            c = *addr++; 
      len--;

            if (c < PARITY) {
                if (c <= UMLAUT) {
                    if (c == UMLAUT) {
                        c = (uchar_t)*addr++;
                        len--;
                    } else
                        break;
                }

                if (table[c])
                    foundchar = p;
                *p++ = c;
            } else {        
                TO7BIT(c); 

        if (table[l->bigram1[c]] || table[l->bigram2[c]])
          foundchar = p + 1;

                *p++ = l->bigram1[c];
                *p++ = l->bigram2[c];
            }
        }

        if (found) {
            cutoff = path;
            *p-- = '\0';
            foundchar = p;
        } else if (foundchar >= path + count) {
            *p-- = '\0';
            cutoff = path + count;
        } else if(!strict)
            continue;
    else 
      *p-- = '\0';

        found = 0;
    if(strict) {
      for(s = path, q = pathpart; *q; s++, q++)
        if((icase && TOLOWER(*s) != *q) || (!icase && *s != *q))
          break;
      if(*q == '\0') {
        if(!f(path))
          return 0;
      }
    } else {
      for (s = foundchar; s >= cutoff; s--) {
        if (*s == cc || (icase && TOLOWER(*s) == cc)) {
          for (p = patend - 1, q = s - 1; *p != '\0'; p--, q--)
            if (*q != *p && (!icase || TOLOWER(*q) != *p))
              break;
          if (*p == '\0' && \
                (icase ? \
                  strncasecmp(path, pathpart, strlen(pathpart)) : \
                  strncmp(path, pathpart, strlen(pathpart)))) {
            found = 1;
            if(!f(path))
              return 0;
            break;
          }
        }
      }
    }
    }

  if(strict) search(l, pathpart, f, df, icase, false);
  else if(df) df();
}

#ifdef INCLUDE_MAIN
void usage(char *prog) {
  fatal("%s -f <indexFile> [-c <scanFile>] [-s <search>]", prog);
}

int main(int argc, char **argv) {
  size_t size;
  uchar_t *dbptr;
  extern char *optarg;
  char scanFile[MAXSTR], indexFile[MAXSTR], needle[MAXSTR];
  unsigned char ch; 
  bool doScan = false, doSearch = false, haveScanFile = false, icase = true;
  lindex l = {0};

  debug = false;

  while ((ch = getopt(argc, argv, "dac:s:f:hn")) != 255) {
    switch (ch) {
    case 'c':
      haveScanFile = true;
      strncpy(scanFile, optarg, MAXSTR);
      break;
    case 'n':
      doScan = true;
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
  else if(doSearch)
    search(&l, (uchar_t *) needle, handle_match, NULL, icase, true);
  else {
    debug("no action");
    usage(argv[0]);
  }
}
#endif

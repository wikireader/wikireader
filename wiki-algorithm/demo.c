/* demo.c -- Implementations of multikey quicksort and ternary search trees
   Usage
    demo                  Run basic timings on /usr/dict/words
    demo <file>           Run basic timings on <file>
    demo <file> trysearch Interactive pm and nn search on <file>
    demo <file> nncost    Run near neigbhor expers on <file>
    demo <file> pmcost    Interactive partial match expers on <file>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// MULTIKEY QUICKSORT

#ifndef min
#define min(a, b) ((a)<=(b) ? (a) : (b))
#endif

#define swap(a, b) { char *t=x[a]; \
                     x[a]=x[b]; x[b]=t; }
#define i2c(i) x[i][depth]

void vecswap(int i, int j, int n, char *x[])
{   while (n-- > 0) {
        swap(i, j);
        i++;
        j++;
    }
}

void ssort1(char *x[], int n, int depth)
{   int    a, b, c, d, r, v;
    if (n <= 1)
        return;
    a = rand() % n;
    swap(0, a);
    v = i2c(0);
    a = b = 1;
    c = d = n-1;
    for (;;) {
        while (b <= c && (r = i2c(b)-v) <= 0) {
            if (r == 0) { swap(a, b); a++; }
            b++;
        }
        while (b <= c && (r = i2c(c)-v) >= 0) {
            if (r == 0) { swap(c, d); d--; }
            c--;
        }
        if (b > c) break;
        swap(b, c);
        b++;
        c--;
    }
    r = min(a, b-a);     vecswap(0, b-r, r, x);
    r = min(d-c, n-d-1); vecswap(b, n-r, r, x);
    r = b-a; ssort1(x, r, depth);
    if (i2c(r) != 0)
        ssort1(x + r, a + n-d-1, depth+1);
    r = d-c; ssort1(x + n-r, r, depth);
}

void ssort1main(char *x[], int n)
{ ssort1(x, n, 0); }


// ssort2 -- Faster Version of Multikey Quicksort

void vecswap2(char **a, char **b, int n)
{   while (n-- > 0) {
        char *t = *a;
        *a++ = *b;
        *b++ = t;
    }
}

#define swap2(a, b) { t = *(a); *(a) = *(b); *(b) = t; }
#define ptr2char(i) (*(*(i) + depth))

char **med3func(char **a, char **b, char **c, int depth)
{   int va, vb, vc;
    if ((va=ptr2char(a)) == (vb=ptr2char(b)))
        return a;
    if ((vc=ptr2char(c)) == va || vc == vb)
        return c;       
    return va < vb ?
          (vb < vc ? b : (va < vc ? c : a ) )
        : (vb > vc ? b : (va < vc ? a : c ) );
}
#define med3(a, b, c) med3func(a, b, c, depth)

void inssort(char **a, int n, int d)
{   char **pi, **pj, *s, *t;
    for (pi = a + 1; --n > 0; pi++)
        for (pj = pi; pj > a; pj--) {
            // Inline strcmp: break if *(pj-1) <= *pj
            for (s=*(pj-1)+d, t=*pj+d; *s==*t && *s!=0; s++, t++)
                ;
            if (*s <= *t)
                break;
            swap2(pj, pj-1);
    }
}

void ssort2(char **a, int n, int depth)
{   int d, r, partval;
    char **pa, **pb, **pc, **pd, **pl, **pm, **pn, *t;
    if (n < 10) {
        inssort(a, n, depth);
        return;
    }
    pl = a;
    pm = a + (n/2);
    pn = a + (n-1);
    if (n > 30) { // On big arrays, pseudomedian of 9
        d = (n/8);
        pl = med3(pl, pl+d, pl+2*d);
        pm = med3(pm-d, pm, pm+d);
        pn = med3(pn-2*d, pn-d, pn);
    }
    pm = med3(pl, pm, pn);
    swap2(a, pm);
    partval = ptr2char(a);
    pa = pb = a + 1;
    pc = pd = a + n-1;
    for (;;) {
        while (pb <= pc && (r = ptr2char(pb)-partval) <= 0) {
            if (r == 0) { swap2(pa, pb); pa++; }
            pb++;
        }
        while (pb <= pc && (r = ptr2char(pc)-partval) >= 0) {
            if (r == 0) { swap2(pc, pd); pd--; }
            pc--;
        }
        if (pb > pc) break;
        swap2(pb, pc);
        pb++;
        pc--;
    }
    pn = a + n;
    r = min(pa-a, pb-pa);    vecswap2(a,  pb-r, r);
    r = min(pd-pc, pn-pd-1); vecswap2(pb, pn-r, r);
    if ((r = pb-pa) > 1)
        ssort2(a, r, depth);
    if (ptr2char(a + r) != 0)
        ssort2(a + r, pa-a + pn-pd-1, depth+1);
    if ((r = pd-pc) > 1)
        ssort2(a + n-r, r, depth);
}

void ssort2main(char **a, int n) { ssort2(a, n, 0); }


// TERNARY SEARCH TREE ALGS

typedef struct tnode *Tptr;
typedef struct tnode {
    char splitchar;
    Tptr lokid, eqkid, hikid;
} Tnode;
Tptr root;

// Insert 1 -- Simple Insertion Algorithm

Tptr insert1(Tptr p, char *s)
{   if (p == 0) {
        p = (Tptr) malloc(sizeof(Tnode));
        p->splitchar = *s;
        p->lokid = p->eqkid = p->hikid = 0;
    }
    if (*s < p->splitchar)
        p->lokid = insert1(p->lokid, s);
    else if (*s == p->splitchar) {
        if (*s != 0)
            p->eqkid = insert1(p->eqkid, ++s);
    } else
        p->hikid = insert1(p->hikid, s);
    return p;
}

void cleanup1(Tptr p)
{   if (p) {
        cleanup1(p->lokid);
        cleanup1(p->eqkid);
        cleanup1(p->hikid);
        free(p);
    }
}


// Insert 2 -- Faster version of Insert

#define BUFSIZE 1000
Tptr buf;
int bufn, freen;
void * freearr[10000];
int storestring = 0;

void insert2(char *s)
{   int d;
    char *instr = s;

    Tptr pp, *p;
    p = &root;
    while (pp = *p) {
        if ((d = *s - pp->splitchar) == 0) {
            if (*s++ == 0) return;
            p = &(pp->eqkid);
        } else if (d < 0)
            p = &(pp->lokid);
        else
            p = &(pp->hikid);
    }
    for (;;) {
        // *p = (Tptr) malloc(sizeof(Tnode));
          if (bufn-- == 0) {
              buf = (Tptr) malloc(BUFSIZE *
                              sizeof(Tnode));
              freearr[freen++] = (void *) buf;
              bufn = BUFSIZE-1;
          }
          *p = buf++;
        pp = *p;
        pp->splitchar = *s;
        pp->lokid = pp->eqkid = pp->hikid = 0;
        if (*s++ == 0) {
            if (storestring)
                pp->eqkid = (Tptr) instr;
            return;
        }
        p = &(pp->eqkid);
    }
}
void cleanup2()
{   int i;
    for (i = 0; i < freen; i++)
        free(freearr[i]);
}

// Search Algorithms

int search1(char *s)
{   Tptr p;
    p = root;
    while (p) {
        if (*s < p->splitchar)
            p = p->lokid;
        else if (*s == p->splitchar)  {
            if (*s++ == 0)
                return 1;
            p = p->eqkid;
        } else
            p = p->hikid;
    }
    return 0;
}

int search2(char *s)
{   int d, sc;
    Tptr p;
    sc = *s;
    p = root;
    while (p) {
        if ((d = sc - p->splitchar) == 0) {
            if (sc == 0)
                return 1;
            sc = *++s;
            p = p->eqkid;
        } else if (d < 0)
            p = p->lokid;
        else 
            p = p->hikid;
    }
    return 0;
}

// Advanced searching: Partial match, near words

int nodecnt;
char *srcharr[100000];
int    srchtop;

void pmsearch(Tptr p, char *s)
{   if (!p) return;
    nodecnt++;
    if (*s == '.' || *s < p->splitchar)
        pmsearch(p->lokid, s);
    if (*s == '.' || *s == p->splitchar)
        if (p->splitchar && *s)
            pmsearch(p->eqkid, s+1);
    if (*s == 0 && p->splitchar == 0)
        srcharr[srchtop++] =
            (char *) p->eqkid;
    if (*s == '.' || *s > p->splitchar)
        pmsearch(p->hikid, s);
}

void nearsearch(Tptr p, char *s, int d)
{   if (!p || d < 0) return;
    nodecnt++;
    if (d > 0 || *s < p->splitchar)
        nearsearch(p->lokid, s, d);
    if (p->splitchar == 0) {
        if ((int) strlen(s) <= d)
            srcharr[srchtop++] =
                (char *) p->eqkid;
    } else
        nearsearch(p->eqkid, *s ? s+1:s,
            (*s==p->splitchar) ? d:d-1);
    if (d > 0 || *s > p->splitchar)
        nearsearch(p->hikid, s, d);
}

// OTHER SET ALGORITHMS AND DATA STRUCTURES

// Private scmp to avoid tuned library function
int scmp(char *s, char *t)
{   for ( ; *s == *t; s++, t++)
        if (*s == 0)
            return 0;
    return *s - *t;
}

// Binary search

int sbsearch(char *key, char **a, int n)
{   int    m;
    char    *s, *t;
    while (n >= 1) {
        m = n/2;
        for (s = key, t = a[m]; *s == *t; s++, t++)
            if (*s == 0)
                return 1;
        if (*s < *t) {
            n = m;
        } else {
            a = a + m+1; n = n - m - 1;
        }
    }
    return 0;
}

// Hashing

typedef struct hnode *Hptr;
typedef struct hnode {
    char    *str;
    Hptr    next;
} Hnode;
Hptr *hashtab;
int    tabsize;

int hashfunc(char *s)
{   unsigned n = 0;
    for ( ; *s; s++)
        n = 31 * n + *s;
    return n % tabsize;
}

void hbuild(char **a, int n)
{   int    i, j;
    Hptr    p;
    tabsize = n;
    hashtab = (Hptr *) malloc(tabsize * sizeof(Hptr));
    for (i = 0; i < tabsize; i++)
        hashtab[i] = 0;
    for (i = 0; i < tabsize; i++) {
        j = hashfunc(a[i]);
        p = (Hptr) malloc(sizeof(Hnode));
        p->str = a[i];
        p->next = hashtab[j];
        hashtab[j] = p;
    }
}

int hsearch(char *s)
{   Hptr p;
    for (p = hashtab[hashfunc(s)]; p; p = p->next)
        if (scmp(s, p->str) == 0)
            return 1;
    return 0;
}

int hsearch2(char *ins)
{   Hptr p;
    char *s, *t;
    for (p = hashtab[hashfunc(ins)]; p; p = p->next) {
        // if (scmp(ins, p->str) == 0)
        //     return 1;
        for (s = ins, t = p->str; *s == *t; s++, t++)
            if (*s == 0)
                return 1;
    }
    return 0;
}

int hsearch3(char *s)
{   Hptr p;
    for (p = hashtab[hashfunc(s)]; p; p = p->next)
        if (strcmp(s, p->str) == 0)
            return 1;
    return 0;
}

// Radix Sort -- McIlroy, Bostic and McIlroy

#define SIZE 510
#define THRESHOLD 16

typedef char *string;
typedef struct { string *sa; int sn, si; } rstack_t;

#define rswap(p, q, r)    r = p, p = q, q = r

void simplesort(string *a, int n, int b)    /* insertion sort */
{     string *ak, *ai, s, t;

    for (ak = a+1; --n >= 1; ak++) {
        for (ai = ak; ai > a; ai--) {
            for (s = ai[0]+b, t = ai[-1]+b; *s; s++, t++)
                if (*s != *t)
                    break;
            if (*s >= *t)
                break;
            rswap(ai[0], ai[-1], s);
        }
    }
}

#define push(a, n, i)    sp->sa = a, sp->sn = n, (sp++)->si = i
#define pop(a, n, i)    a = (--sp)->sa, n = sp->sn, i = sp->si
#define stackempty()    (sp <= stack)

void rsorta(string *a, int n, int b)
{   rstack_t        stack[SIZE], stmp, *oldsp, *bigsp, *sp = stack;
    string        *pile[256], *ak, *an, r, t;
    static int    count[256], cmin, nc;
    int        c, *cp;

    push(a, n, b);
    while (!stackempty()) {
        pop(a, n, b);
        if(n < THRESHOLD) {        // divert
            simplesort(a, n, b);
            continue;
        }
        an = a + n;
        if(nc == 0) {            // nonrecursive?
            cmin = 255;        // tally
            for (ak = a; ak < an; ) {
                c = (*ak++)[b];
                if(++count[c] == 1 && c) {
                    if(c < cmin) cmin = c;
                    nc++;
            }    }
            if(sp+nc > stack+SIZE) { // stack overflow
                rsorta(a, n, b);
                continue;
        }    }
        oldsp = bigsp = sp, c = 2;    // logarithmic stack
        pile[0] = ak = a + count[0];    // find places
        for(cp = count+cmin; nc > 0; cp++, nc--) {
            while(*cp == 0) cp++;
            if(*cp > 1) {
                if(*cp > c) c = *cp, bigsp = sp;
                push(ak, *cp, b+1);
            }
            pile[cp-count] = ak += *cp;
        }
        rswap(*oldsp, *bigsp, stmp);
                        // permute home
        for(ak = a; ak < an; ak += count[c], count[c] = 0) {
            r = *ak;
            while(--pile[c = r[b]] > ak)
                rswap(*pile[c], r, t);
            *ak = r;
        }        // here nc = count[...] = 0
}    }

void rsort(string *a, int n) { rsorta(a, n, 0); }



// TIMING

// Sort support functions

void scramble(char *x[], int n)
{   int i;
    for ( ; n > 0; n--) {
        i = rand() % (n+1);
        swap(n, i);
    }
}

int qscomp(const void *p, const void *q )
{   return strcmp(* (char**) p, * (char**) q );
}
#define DOQSORT qsort((void *) a, (size_t) n, sizeof(char *), qscomp)


// Insertion support functions

int instype;

void rinsall(char **a, int n)
{   int m;
    if (n < 1) return;
    m = n/2;
    switch (instype) {
    case 1: root = insert1(root,a[m]); break;
    case 2: insert2(a[m]); break;
    case 9: break;
    }
    rinsall(a, m);
    rinsall(a + m+1, n-m-1);
}

void insall(char **a, int n)
{   switch (instype) {
    case 1: root = 0; rinsall(a, n); break;
    case 2: root = 0; bufn = freen = 0; rinsall(a, n); break;
    case 9: rinsall(a, n); break;
    }
}

// Search support functions

int searchtype;

void searchall(char **a, int n)
{   int i, j;
    char s[100]; 
    for (i = 0; i < n; i++) {
        strcpy(s, a[i]);
        // s[0]++;  // Uncomment for unsuccessful searches
        switch(searchtype) {
        case 1: j = search1(s); break;
        case 2: j = search2(s); break;
        case 3: srchtop = 0; pmsearch(root, s); j = srchtop; break;
        case 4: srchtop = 0; nearsearch(root, s, 0); j = srchtop; break;
        case 5: j = sbsearch(s, a, n); break;
        case 6: j = hsearch(s); break;
        case 7: j = hsearch2(s); break;
        case 8: j = hsearch3(s); break;
        case 9: j = 1; break;
        }
        if (j != 1)  // Comment for unsuccessful searches
           printf("search bug at %d, val %d\n", i, j);
    }    
}


// Main timing

int    n = 0, starttime,  rptctr;

#define TASK(s)    printf("%s", s);
#define CIN        starttime = clock();
#define COUT    printf("\t%d", clock()-starttime);
#define NL    printf("\n")
#define REPEAT(s)    for (rptctr=0; rptctr<5; rptctr++) { s }; NL;

char space[1000000], *a[100000];

void timeall()
{   TASK("System Qsort")
        REPEAT(scramble(a, n); CIN; DOQSORT; COUT;)
    TASK("Simple MKQSort");
        REPEAT(scramble(a, n); CIN; ssort1main(a, n); COUT;)
    TASK("Faster MKQSort");
        REPEAT(scramble(a, n); CIN; ssort2main(a, n); COUT;)
    TASK("Radix Sort");
        REPEAT(scramble(a, n); CIN; rsort(a, n); COUT;)
    TASK("Null Insert");
        instype = 9;
        REPEAT(CIN; insall(a, n); COUT;)
    TASK("Simp TST Insert");
        instype = 1;
        REPEAT(CIN; insall(a, n); COUT; cleanup1(root);)
    TASK("Fast TST Insert");
        instype = 2;
        REPEAT(CIN; insall(a, n); COUT; cleanup2();)
    TASK("Null Search");
        searchtype = 9;
        REPEAT(CIN; searchall(a, n); COUT;)
instype = 2; insall(a, n);    // Build TST for searching
    TASK("Simp TST Search");
        searchtype = 1;
        REPEAT(CIN; searchall(a, n); COUT;)
    TASK("Fast TST Search");
        searchtype = 2;
        REPEAT(CIN; searchall(a, n); COUT;)
    TASK("PM TST Search");
        searchtype = 3;
        REPEAT(CIN; searchall(a, n); COUT;)
    TASK("Near TST Search");
        searchtype = 4;
        REPEAT(CIN; searchall(a, n); COUT;)
cleanup2();                   // Remove TST
    TASK("Binary Search");
        searchtype = 5;
        REPEAT(CIN; searchall(a, n); COUT;)
    TASK("Build Hash");
        CIN;
        hbuild(a, n); // Build once -- no cleanup
        COUT; NL;
    TASK("Hash Search");
        searchtype = 6;
        REPEAT(CIN; searchall(a, n); COUT;)
    TASK("Hash, Inln Cmp");
        searchtype = 7;
        REPEAT(CIN; searchall(a, n); COUT;)
    TASK("Hash, Sys Cmp");
        searchtype = 8;
        REPEAT(CIN; searchall(a, n); COUT;)
// Hash table is still using space
}

void buildptrtree()
{   TASK("Building TST");
        CIN;
        storestring = 1;    // Sleazy hack: store strings in eqkid
        ssort2main(a, n);
        instype = 2;
        insall(a, n);
        COUT; NL;
}

void trysearch()
{   char s[100];
    int i, d;

    buildptrtree();
    printf("Enter searches: <nndistance> <word>  (dist=-1 for pm search)\n");
    while (scanf("%d %s", &d, s) != EOF) {
        srchtop = 0;
        nodecnt = 0;
        CIN;
        if (d < 0)
            pmsearch(root, s);
        else
            nearsearch(root, s, d);
        printf(" matches=%d nodes=%d clicks=", srchtop, nodecnt);
        COUT; NL;
        for (i = 0; i < min(srchtop, 40); i++)
            printf(" %s", srcharr[i]);
        NL;
    }
}

void nncost()
{   int i, d, mincost, minind, maxcost, maxind; 
 	double sum;
    buildptrtree();
    for (d = 0; d <= 4; d++) {
        CIN;
        mincost = 999999; maxcost = -1; sum = 0.0;
        for (i = 0; i < n; i++) {
            srchtop = 0; nodecnt = 0;
            nearsearch(root, a[i], d);
			sum += nodecnt;
            if (nodecnt <= mincost) { mincost = nodecnt; minind = i; }
            if (nodecnt >= maxcost) { maxcost = nodecnt; maxind = i; }
        }
        printf("Dist %d\t%d %s\t%d %s\t%g", d,
            mincost, a[minind],
            maxcost, a[maxind],
			sum/n);
        COUT; NL;
    }    
}

void pmcost()
{   int i, j, l, u, mincost, minind, maxcost, maxind;
	double sum;
	char buf[100]; 
    buildptrtree();
    printf("Enter l, u pair\n");
	while (scanf("%d %d", &l, &u) != EOF) {
        CIN;
        mincost = 999999; maxcost = -1; sum = 0.0;
        for (i = 0; i < n; i++) {
            strcpy(buf, a[i]);
            for (j = l; j <= u; j++)
				buf[j] = '.';
            srchtop = 0; nodecnt = 0;
            pmsearch(root, buf);
			sum += nodecnt;
            if (nodecnt <= mincost) { mincost = nodecnt; minind = i; }
            if (nodecnt >= maxcost) { maxcost = nodecnt; maxind = i; }
        }
        printf("Range: %d,%d\t%d %s\t%d %s\t%g", l, u,
            mincost, a[minind],
            maxcost, a[maxind],
			sum/n);
        COUT; NL;
	}
}


int main(int argc, char *argv[])
{   int    i, globalstarttime;
    char *s = space, *t, *fname;
    FILE *fp;

    if (argc == 1) // no args
        fname = "/usr/dict/words";
    else // one arg: file name
        fname = argv[1];
    setbuf(stdout, 0);
    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "  Can't open file\n");
        exit(1);
    }
    globalstarttime = clock();
    TASK("Big Malloc");
        CIN;
        t = (char *) malloc(8000000*sizeof(char));
        free(t);
        COUT; NL;
    TASK("Reading Input");
        CIN;
        a[0] = s;
        while ((i = getc(fp)) != EOF) {
            if (i == '\n') {
                *s++ = 0;
                a[++n] = s;
            } else
                *s++ = i;
        }
        COUT; NL;
    if (argc < 3) { // one arg: file name
        timeall();
    } else {
        if (strcmp(argv[2], "trysearch") == 0) {
            trysearch();
        } else if (strcmp(argv[2], "nncost") == 0) {
            nncost();
        } else if (strcmp(argv[2], "pmcost") == 0) {
			pmcost();
		} else
            printf("Unrecognized option\n");
    }
    i = clock() - globalstarttime;
    printf("Total clicks\t%d\nTotal secs\t%4.3f\n",
        i, (double) i / CLOCKS_PER_SEC);
    return 0;
}


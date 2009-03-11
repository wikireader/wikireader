PROGRAMS = lcode lbigram lsearcher
.PHONY:	all
all: $(PROGRAMS)

CFLAGS = -ggdb3 -Os -Wall -DDEBUG -DINCLUDE_MAIN -I../../../wiki-lib/include/

.c.o:
	$(CC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

file-io-general.o:
	$(CC) -M $(CFLAGS) ../../../wiki-lib/generic/file-io-general.c > file-io-general.d
	$(CC) $(CFLAGS) -c ../../../wiki-lib/generic/file-io-general.c -o file-io-general.o

file-io-posix.o:
	$(CC) -M $(CFLAGS) ../../../wiki-lib/specific/file-io-posix.c > file-io-posix.d
	$(CC) $(CFLAGS) -c ../../../wiki-lib/specific/file-io-posix.c -o file-io-posix.o

lsearcher: lsearcher.o file-io-general.o file-io-posix.o fast_search.o slow_search.o
	$(CC) -o $@ $^

clean:
	$(RM) *.d *.o $(PROGRAMS)

-include $(wildcard *.d) dummy

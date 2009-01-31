PROGRAMS = lcode lbigram lsearcher
all: $(PROGRAMS)

CFLAGS = -ggdb3 -O3 -Wall -DDEBUG -DINCLUDE_MAIN -I../../../wiki-lib/include/

.c.o:
	$(CC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

file-io-posix.o:
	$(CC) -M $(CFLAGS) ../../../wiki-lib/specific/file-io-posix.c > file-io-posix.d
	$(CC) $(CFLAGS) -c ../../../wiki-lib/specific/file-io-posix.c -o file-io-posix.o

lsearcher: lsearcher.o file-io-posix.o
	$(CC) -o lsearcher lsearcher.o file-io-posix.o

clean:
	$(RM) *.d *.o $(PROGRAMS)

-include $(wildcard *.d) dummy

PROGRAMS = lcode lbigram lsearcher
all: $(PROGRAMS)

CFLAGS = -ggdb3 -O3 -Wall -DDEBUG -DINCLUDE_MAIN

.c.o:
	$(CC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

lsearcher: lsearcher.o safe.o
	$(CC) -o lsearcher lsearcher.o safe.o

clean:
	$(RM) *.d *.o $(PROGRAMS)

-include $(wildcard *.d) dummy

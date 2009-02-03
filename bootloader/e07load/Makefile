GCC=gcc
CC=$(GCC)

LDFLAGS=

CFLAGS=-Wall -I. -I../../common/

all: e07load

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

e07load: eeprom.o e07.o misc.o mapfile.o tty.o main.o
	$(GCC) -o $@ -Wall $^

clean::
	rm -fr e07load *.o *.d

-include $(wildcard *.d) dummy


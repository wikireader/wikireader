GCC=gcc
CC=$(GCC)

LDFLAGS=

CFLAGS=-Wall -I.

all: l17load

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

l17load: eeprom.o l17.o misc.o mapfile.o tty.o main.o
	$(GCC) -o $@ -Wall $^

clean::
	rm -fr l17load *.o *.d

-include $(wildcard *.d) dummy


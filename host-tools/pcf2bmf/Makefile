RM = rm -f
CC = gcc
LD = ld

CFLAGS = -g -O3 -Wall -Werror -MD -D_REENTRANT
LDFLAGS = -g -dynamic
C_OBJS = pcf.o

vpath %.c

TARGETS = pcf2bmf
OBJS = $(C_OBJS) pcf2bmf.o

all: $(TARGETS)

pcf2bmf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) $(TARGETS) *.o *.d

-include $(wildcard *.d) dummy


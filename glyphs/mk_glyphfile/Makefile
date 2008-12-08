RM = rm -f
CC = gcc
LD = ld
CFLAGS = -g -O3 -Wall -MD -D_REENTRANT
LDFLAGS = -g -dynamic -lpthread -lgd

TARGETS = mk_glyphfile
OBJS = image.o

all: $(TARGETS)

mk_glyphfile: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) $(TARGETS) *.o *.d 

-include $(wildcard *.d) dummy	


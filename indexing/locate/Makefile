PROGRAMS = lcode lbigram
all: $(PROGRAMS)

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

clean:
	$(RM) *.d *.o $(PROGRAMS)

-include $(wildcard *.d) dummy

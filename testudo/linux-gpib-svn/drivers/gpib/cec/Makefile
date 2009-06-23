
obj-m += cec_gpib.o

cec_gpib-objs := init.o interrupt.o

cec_gpib.o: $(cec_gpib-objs)
	$(LD) -r -o cec_gpib.o $(cec_gpib-objs)


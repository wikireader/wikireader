#get flags for kernel modules from linux kernel source tree

obj-m += fake.o

$(obj)/fake.c: flags
	touch $(obj)/fake.c

$(obj)/fake.o: flags

ESCAPED_LINUXDIR=$(shell echo $(LINUXDIR) | sed 's^/^\\/^g')

.PHONY: flags
flags:
	echo LINUX_ARCH="$(ARCH)" >$(obj)/flags
	echo LINUX_AFLAGS="$(AFLAGS)" | sed 's/Iinclude/I$(ESCAPED_LINUXDIR)\/include/g'>>$(obj)/flags
	echo LINUX_LDFLAGS="" >>$(obj)/flags
	echo LINUX_ARFLAGS="$(ARFLAGS)" >>$(obj)/flags
	echo LINUX_CROSS_COMPILE="$(CROSS_COMPILE)" >>$(obj)/flags
	echo LINUX_KERNELRELEASE="$(KERNELRELEASE)" >>$(obj)/flags
	echo LINUX_CFLAGS="$(CFLAGS)" | sed 's/Iinclude/I$(ESCAPED_LINUXDIR)\/include/g'>>$(obj)/flags
	echo LINUX_CFLAGS_MODULE="$(CFLAGS_MODULE)" >>$(obj)/flags
	echo LINUX_CC="$(CC)" >>$(obj)/flags
	echo LINUX_LD="$(LD) $(LDFLAGS) $(LDFLAGS_MODULE)" >>$(obj)/flags
	echo LINUX_AS="$(AS)" >>$(obj)/flags
	echo LINUX_MODLIB="$(MODLIB)" >>$(obj)/flags
	
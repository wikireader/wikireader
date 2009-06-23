#
#  Makefile for ieee488 application suite
#
#  Please don't change this file !
#  Subdirectories will be automagically scanned
#



SUBDIRS = `../util/subdirs`

GPIB_HOME= `(cd ..;pwd)`
MAKE = make GPIB_HOME=$(GPIB_HOME) -C

all: subdirs

subdirs:  
	set -e;for i in $(SUBDIRS); do $(MAKE) $$i ; done

clean:
	set -e;for i in $(SUBDIRS); do $(MAKE) $$i clean; done
	-find . -name "*.o" -exec rm {} \;

install:
	set -e;for i in $(SUBDIRS); do $(MAKE) $$i install; done

rcsput:
	set -e;for i in $(SUBDIRS); do $(MAKE) $$i rcsput; done

rcsget:
	set -e;for i in $(SUBDIRS); do $(MAKE) $$i rcsget; done










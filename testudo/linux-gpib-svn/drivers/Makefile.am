# driver/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

GPIB_MODULE_SUBDIR = gpib
GPIB_ABS_MODULE_SUBDIR = @abs_srcdir@/$(GPIB_MODULE_SUBDIR)

EXTRA_DIST = $(GPIB_MODULE_SUBDIR)
SUBDIRS = gpib/include

-include $(top_srcdir)/util/linux_flags/flags

#ignore errors from kernel's make modules in all-local, to avoid failure of make install
#on nfs filesystem with root_squash
all-local:
	-$(MAKE) -C $(LINUX_SRCDIR) V=1 modules\
		CC="$(LINUX_CC) -I@abs_top_srcdir@ -I@abs_top_srcdir@/drivers/gpib/include -I@abs_top_srcdir@/include" \
		SUBDIRS="$(GPIB_ABS_MODULE_SUBDIR)"

clean-local:
	$(MAKE) -C $(LINUX_SRCDIR) V=1 clean\
		CC="$(LINUX_CC) -I@abs_top_srcdir@ -I@abs_top_srcdir@/drivers/gpib/include -I@abs_top_srcdir@/include" \
		SUBDIRS="$(GPIB_ABS_MODULE_SUBDIR)"

install-data-hook: device-file-check /etc/gpib.conf
	$(MAKE) -C $(LINUX_SRCDIR) V=1 modules_install\
		CC="$(LINUX_CC) -I@abs_top_srcdir@ -I@abs_top_srcdir@/driver/include -I@abs_top_srcdir@/include" \
		SUBDIRS="$(GPIB_ABS_MODULE_SUBDIR)" INSTALL_MOD_DIR="gpib"
	$(DEPMOD) -ae

/dev/gpib0:
	groupadd gpib || echo "group gpib exists"
	for i in `seq 0 15`; \
	do \
		mknod -m u=rw,g=rw,o= /dev/gpib$${i} c $(IBMAJOR) $${i} || exit 1; \
		chown root:gpib /dev/gpib$${i}; \
	done

.PHONY : device-file-check
device-file-check: /dev/gpib0
	@if [ ! -c /dev/gpib0 ]; then \
		if [ -a /dev/gpib0 ]; then \
			echo "A file or directory called /dev/gpib0 exists but it is not" \
				"a character device.  Delete or move it and try again."; \
			exit 1; \
		fi; \
	fi
	@ls -l /dev/gpib0 | grep -q "$(IBMAJOR)"; \
	if [ $$? != 0 ]; then \
		echo "/dev/gpib0 has the wrong major number. " \
			"Delete your /dev/gpibX files and try again."; \
		exit 1; \
	fi

#should move this to util/templates Makefile.am when it exists
/etc/gpib.conf:
	$(INSTALL_DATA) -D $(top_srcdir)/util/templates/gpib.conf /etc/gpib.conf

#make sure compiled files, etc don't make it into distribution tarballs
dist-hook:
	-find $(distdir) \( -name '*.[oas]' -o -name '*.ko' -o -name core -o -name '.*.cmd' -o -name '*.mod.c' \) -type f \
		-exec $(RM) {} \;
	-find $(distdir) -depth -name '.tmp_versions' -type d -exec $(RM) -r \{\} \;


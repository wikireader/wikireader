# Makefile.am
# copyright (C) 2004 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

EXTRA_DIST = ni_usb_gpib ni_usb_gpib.usermap

HOTPLUG_USB_CONF_DIR=/etc/hotplug/usb
USB_FIRMWARE_DIR=/usr/share/usb/

install-data-local:
	test -e $(HOTPLUG_USB_CONF_DIR)/ni_usb_gpib || $(INSTALL_SCRIPT) -D ni_usb_gpib $(HOTPLUG_USB_CONF_DIR)/ni_usb_gpib
	test -e $(HOTPLUG_USB_CONF_DIR)/ni_usb_gpib.usermap || $(INSTALL_DATA) -D ni_usb_gpib.usermap $(HOTPLUG_USB_CONF_DIR)/ni_usb_gpib.usermap
	$(INSTALL) -d $(USB_FIRMWARE_DIR)/ni_usb_gpib
	
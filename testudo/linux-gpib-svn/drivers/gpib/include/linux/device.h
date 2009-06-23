/*
 * linux/device.h compatibility header
 */
/*
    Copyright (C) 2004-2006 Frank Mori Hess <fmhess@users.sourceforge.net>
    Copyright (C) 2005-2006 Ian Abbott

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __COMPAT_LINUX_DEVICE_H_
#define __COMPAT_LINUX_DEVICE_H_

#include <linux/version.h>

/*
 * Notes:
 *
 * The 'struct device *' returned by the device_create() compatibility
 * functions (assuming the return value is not an error pointer for which
 * 'IS_ERR(ptr)' is true) is not really a 'struct device *' and should not
 * be treated as such.  For kernel versions 2.5.0 to 2.6.17, the return
 * value is actually a 'struct class_device *' in disguise and we assume
 * the 'parent' parameter of device_create() is also a 'struct class_device *'
 * in disguise from a previous call to device_create().
 *
 * The main limitation is that we cannot use a *real* 'struct device *' as
 * the parent parameter of device_create(), only a pointer from a previous
 * call to device_create().
 *
 * Call CLASS_DEVICE_CREATE() instead of device_create() for compatibility
 * with kernel versions prior to 2.6.27.
 *
 * We do not currently support 'dev_get_drvdata()' and 'dev_set_drvdata()'
 * for kernel versions prior to 2.6.26, so the 'drvdata' parameter of
 * CLASS_DEVICE_CREATE() is pretty useless.
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#include <pcmcia/cs_types.h>

struct device_driver {
	char *name;
};

struct class;
struct device;

static inline struct class *class_create(struct module *owner, char *name)
{
	return NULL;
}

static inline void class_destroy(struct class *cs)
{
}

static inline struct device *device_create(struct class *cls,
	struct device *parent, dev_t devt, void *drvdata, char *fmt, ...)
{
	return NULL;
}

static inline void device_destroy(struct class *cs, dev_t devt)
{
}

#else

#include_next <linux/device.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)

#define class_create(owner, name) \
	(struct class *)class_simple_create(owner, name)
#define class_destroy(cs) \
	class_simple_destroy((struct class_simple *)(cs))
#define CLASS_DEVICE_CREATE(cs, parent, devt, drvdata, fmt...) \
	(struct device *)class_simple_device_add((struct class_simple *)(cs), \
		devt, NULL, fmt)
#define device_destroy(cs, devt) \
	class_simple_device_remove(devt)

#else

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)

#define CLASS_DEVICE_CREATE(cs, parent, devt, drvdata, fmt...) \
	(struct device *)class_device_create(cs, devt, NULL, fmt)
#define device_destroy(cs, devt) \
	class_device_destroy(cs, devt)

#else
/* device_create does not work for NULL parent with 2.6.18, not sure
exactly which kernel version it was fixed in. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)

#define CLASS_DEVICE_CREATE(cs, parent, devt, drvdata, fmt...) \
	(struct device *)class_device_create( \
			cs, (struct class_device *)parent, devt, NULL, fmt)
#define device_destroy(cs, devt) \
	class_device_destroy(cs, devt)

#else

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)

#define CLASS_DEVICE_CREATE(cs, parent, devt, drvdata, fmt...) \
	device_create(cs, parent, devt, fmt)

#else

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

#define CLASS_DEVICE_CREATE(cs, parent, devt, drvdata, fmt...) \
	device_create_drvdata(cs, parent, devt, drvdata, fmt)

#else

#define CLASS_DEVICE_CREATE(cs, parent, devt, drvdata, fmt...) \
	device_create(cs, parent, devt, drvdata, fmt)

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)

#endif // __COMPAT_LINUX_DEVICE_H_


dnl Process this file with autoconf to produce a configure script.

AC_INIT([linux-gpib], [3.2.11])
# libtool version: current:revision:age
#
# If the library source code has changed at all since the last update, then
# increment revision (`c:r:a' becomes `c:r+1:a').
#
# If any interfaces have been added, removed, or changed since the last update,
# increment current, and set revision to 0.
#
# If any interfaces have been added since the last public release, then
# increment age.
#
# If any interfaces have been removed since the last public release, then set
# age to 0.
GPIB_SO_VERSION=1:5:1
AC_CONFIG_SRCDIR([drivers/gpib/sys/osfuncs.c])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_HEADERS([config.h])

AC_SUBST([VERSION])
AC_SUBST([GPIB_SO_VERSION])

[ISODATE=`date +%Y-%m-%d`]
AC_SUBST([ISODATE])

AC_CANONICAL_HOST

AC_ENABLE_SHARED([yes])
AC_ENABLE_STATIC([no])
AC_ARG_ENABLE([pcmcia],[  --enable-pcmcia	Enable support for PCMCIA cards in GPIB kernel drivers],
	[ENABLE_PCMCIA=$enableval],[ENABLE_PCMCIA="no"])
if test "$ENABLE_PCMCIA" == "yes" ; then
	AC_DEFINE([GPIB_CONFIG_PCMCIA],[1],[Define to enable pcmcia support in drivers])
fi
AC_ARG_ENABLE([driver-debug],[  --enable-driver-debug	Enable debug spam to console in GPIB kernel drivers],
	[ENABLE_DRIVER_SPAM=$enableval],[ENABLE_DRIVER_SPAM="no"])
if test "$ENABLE_DRIVER_SPAM" == "yes" ; then
	AC_DEFINE([GPIB_CONFIG_KERNEL_DEBUG],[1],[Define to enable debug spam to console in drivers])
fi
AC_ARG_ENABLE([guile-binding],[  --disable-guile-binding	Disable Guile binding to libgpib],
	[BIND_GUILE=$enableval],[BIND_GUILE="yes"])
AC_ARG_ENABLE([perl-binding],[  --disable-perl-binding	Disable Perl binding to libgpib],
	[BIND_PERL=$enableval],[BIND_PERL="yes"])
AC_ARG_ENABLE([php-binding],[  --disable-php-binding	Disable PHP binding to libgpib],
	[BIND_PHP=$enableval],[BIND_PHP="yes"])
AC_ARG_ENABLE([python-binding],[  --disable-python-binding	Disable Python binding to libgpib],
	[BIND_PYTHON=$enableval],[BIND_PYTHON="yes"])
AC_ARG_ENABLE([tcl-binding],[  --disable-tcl-binding	Disable TCL binding to libgpib],
	[BIND_TCL=$enableval],[BIND_TCL="yes"])
AC_ARG_ENABLE([documentation],[  --disable-documentation	Disable building documentation from SGML sources],
	[BUILD_DOCS=$enableval],[BUILD_DOCS="yes"])

dnl check kernel source directory
AC_ARG_WITH([linux-srcdir],
	[  --with-linux-srcdir=DIR	location of Linux kernel source directory [[DIR=/lib/modules/$(uname -r)/build/]]],
	[LINUX_SRCDIR=$withval],[LINUX_SRCDIR=/lib/modules/$(uname -r)/build/])
if test "$LINUX_SRCDIR" == "no" ; then
	AC_MSG_WARN([A Linux kernel source directory is required to compile driver modules.  Use --with-linux-srcdir=DIR.])
fi
AC_SUBST([LINUX_SRCDIR])
AC_MSG_CHECKING([Linux kernel directory])
if [[ ! -d "$LINUX_SRCDIR" ]]; then
	AC_MSG_WARN([Linux kernel directory $LINUX_SRCDIR does not exist.  Specify using --with-linux-srcdir=DIR.])
fi
if [[ ! -f "$LINUX_SRCDIR/.config" ]]; then
	AC_MSG_WARN([Kernel source tree at $LINUX_SRCDIR is not configured.  Copy the
	appropriate configuration file to $LINUX_SRCDIR/.config and then run 'make oldconfig'
	in the kernel source directory.])
fi
AC_MSG_RESULT([ok])
dnl get compile flags for modules from linux kernel source tree
AC_MSG_CHECKING([Linux kernel compile flags])
FLAGDIR=$(pwd)/util/linux_flags
env LDFLAGS="" CFLAGS="" make --quiet -C ${LINUX_SRCDIR} V=1 SUBDIRS=${FLAGDIR} LINUXDIR=${LINUX_SRCDIR} modules > /dev/null
if [[ $? != 0 ]]; then
	AC_MSG_WARN([Failed to get compile flags from Linux kernel directory.])
fi
AC_MSG_RESULT([ok])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_PATH_PROG([JW_PATH], [jw], [no])
if test "$JW_PATH" == "no" ; then
	AC_MSG_NOTICE([docbook-tools (jw) not found, disabling documentation])
	BUILD_DOCS="no"
fi
AC_PATH_PROG([DEPMOD], [depmod], [true], [PATH:/sbin:/usr/sbin:/usr/local/sbin])
if test "$DEPMOD" == "true" ; then
	AC_MSG_WARN([unable to find the 'depmod' program.  Is it in your path?])
fi

if test "$BIND_PYTHON" == "yes"; then
	AM_PATH_PYTHON()
	PYTHON_MAJOR_VERSION=$(echo $PYTHON_VERSION | cut -d. -f1 )
	PYTHON_MINOR_VERSION=$(echo $PYTHON_VERSION | cut -d. -f2 )
	if (( $PYTHON_MAJOR_VERSION < 2 )); then
		AC_MSG_WARN([python version less than 2.0, disabling python binding])
		BIND_PYTHON="no"
	fi
fi
AC_PATH_PROG([PERL], [perl], [no], [])
if test "$PERL" == "no"; then
	AC_MSG_NOTICE([perl not found, disabling perl binding])
	BIND_PERL="no"
fi
SC_PATH_TCLCONFIG
if [[ $no_tcl ]]; then
	AC_MSG_NOTICE([TCL config not found, disabling TCL binding])
	BIND_TCL="no"
else
	SC_LOAD_TCLCONFIG
fi
AC_PATH_PROG([PHP_CONFIG], [php-config], [no],[])
if test "$PHP_CONFIG" == "no"; then
	AC_MSG_NOTICE([php-config not found, disabling PHP binding])
	BIND_PHP="no"
fi

dnl Checks for libraries.
AC_CHECK_LIB([guile],[gh_define],[echo this prevents default actions from occurring > /dev/null],
	[BIND_GUILE="no";AC_MSG_NOTICE([libguile not found, disabling guile binding])],[])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )
AM_CHECK_PYTHON_HEADERS([],[BIND_PYTHON=no;AC_MSG_NOTICE([python headers not found, disabling python binding])])
AC_CHECK_HEADER([libguile.h],[],[BIND_GUILE="no";AC_MSG_NOTICE([libguile headers not found, disabling guile binding])])

if test "$BIND_TCL" != "no"; then
AC_CHECK_HEADER([$TCL_INC_DIR/tcl.h],[],[BIND_TCL="no";AC_MSG_NOTICE([TCL headers not found, disabling TCL binding])])
fi

if test "$PHP_CONFIG" != "no"; then
SAVE_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $($PHP_CONFIG --includes)"
AC_CHECK_HEADER([php.h],[],[BIND_PHP="no";AC_MSG_NOTICE([PHP headers not found, disabling PHP binding])])
CPPFLAGS=$SAVE_CPPFLAGS
fi

AC_HEADER_MAJOR
AC_FUNC_ALLOCA
AC_STRUCT_TM
AC_STRUCT_ST_BLOCKS
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS([mkfifo])
AC_CHECK_FUNC([mknod])

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

LIBGPIB_CFLAGS="-I\$(top_srcdir)/include -D_REENTRANT"
LIBGPIB_LDFLAGS="\$(top_srcdir)/lib/libgpib.la -lpthread"
AC_SUBST([LIBGPIB_CFLAGS])
AC_SUBST([LIBGPIB_LDFLAGS])
IBMAJOR=160
AC_SUBST([IBMAJOR])
AC_DEFINE([IBMAJOR],[160],[Major number of device files])

echo
echo Configuration:
AM_CONDITIONAL([BUILD_DOCS], [test "$BUILD_DOCS" == "yes"])
echo "SGML Documentation: $BUILD_DOCS"
AM_CONDITIONAL([BIND_GUILE], [test "$BIND_GUILE" == "yes"])
echo "Guile binding: $BIND_GUILE"
AM_CONDITIONAL([BIND_PERL], [test "$BIND_PERL" == "yes"])
echo "Perl binding: $BIND_PERL"
AM_CONDITIONAL([BIND_PHP], [test "$BIND_PHP" == "yes"])
echo "PHP binding: $BIND_PHP"
AM_CONDITIONAL([BIND_PYTHON], [test "$BIND_PYTHON" == "yes"])
echo "Python binding: $BIND_PYTHON"
AM_CONDITIONAL([BIND_TCL], [test "$BIND_TCL" == "yes"])
echo "TCL binding: $BIND_TCL"

AC_OUTPUT([\
	Makefile \
	lib/Makefile \
	lib/gpib_config/Makefile \
	examples/Makefile \
	test/Makefile \
	drivers/Makefile \
	drivers/gpib/include/Makefile \
	doc/Makefile \
	include/Makefile \
	language/Makefile \
	language/guile/Makefile \
	language/php/Makefile \
	language/php/TESTS/Makefile \
	language/python/Makefile \
	language/tcl/Makefile \
	usb/Makefile \
	usb/ni_usb_gpib/Makefile \
	usb/agilent_82357a/Makefile \
]
)


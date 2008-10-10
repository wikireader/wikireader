AC_INIT(tst_init.c)

AC_CANONICAL_TARGET
case "$target_os" in
   darwin*) CFLAGS="$CFLAGS -no-cpp-precomp" ;;
esac

AC_PROG_CC()
case "$CC" in
        gcc*) CFLAGS="$CFLAGS -Wall -g -O2" ;;
        cc*) CFLAGS="$CFLAGS" ;;
esac

AC_PROG_LIBTOOL
AC_OUTPUT(Makefile)

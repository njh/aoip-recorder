dnl Require autoconf version >= 2.59
AC_PREREQ(2.59)


dnl ############# Initialization

AC_INIT([aoip-recorder], [0.0.1], [])
AC_CONFIG_SRCDIR([src/aoip-recorder.c])
AC_CONFIG_AUX_DIR([build-scripts])
AC_CONFIG_MACRO_DIR([build-scripts])
AM_INIT_AUTOMAKE


dnl ############# Compiler and tools Checks

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_C_INLINE



dnl ############## Library Checks

AC_CHECK_LIB([m], [sqrt], , [AC_MSG_ERROR(Can't find libm)])
AC_CHECK_LIB([m], [lrintf])
AC_CHECK_LIB([mx], [powf])

# Check for libsndfile
PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.18)


PKG_CHECK_MODULES(CHECK, check >= 0.9.4, have_check="yes", have_check="no")
if test x"$have_check" = "xyes"; then
  AC_CHECK_PROG(have_checkmk, [checkmk], [yes], [no])
  if test x"$have_checkmk" = "xyes"; then
    AC_DEFINE([HAVE_CHECK], 1, [Define to 1 if check library is available])
	else
    AC_MSG_WARN([Command 'checkmk' not found.])
    AC_MSG_WARN([Download it here: http://micah.cowan.name/projects/checkmk/])
  fi
fi
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes" &&
                           test x"$have_checkmk" = "xyes")


dnl ############## Header Checks

AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
AC_CHECK_HEADERS( [termios.h] )



dnl ############## Function Checks

AC_CHECK_FUNCS( usleep )



dnl ############## Compiler and Linker Flags

CFLAGS="$CFLAGS -Wunused -Wall $SNDFILE_CFLAGS"
LIBS="$LIBS $SNDFILE_LIBS"



dnl ############## Output files

AC_CONFIG_HEADERS([src/config.h])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	test/Makefile
])

AC_OUTPUT


dnl ############## Summary

echo ""
echo "Building $PACKAGE_NAME version $PACKAGE_VERSION."
echo ""
echo "Next type 'make' to begin compilation."


dnl Relayctl

dnl Initialization
AC_INIT([relayctl], [0.1], [Darryl Newberry])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

dnl Archiver support.
AM_PROG_AR

dnl Enhanced libtool support.
LT_INIT

dnl Compiler check
AC_PROG_CC
AC_CHECK_PROG([have_cc],[$CC],[yes],[no])
if test [$have_cc] != [yes]; then
  AC_MSG_ERROR([$CC is missing])
fi

dnl Configure macros
AC_CONFIG_MACRO_DIR([m4])

AC_CHECK_HEADER([ftdi.h], [], 
  [AC_MSG_ERROR([ftdi.h header not found])])

AC_MSG_NOTICE([CPPFLAGS = $CPPFLAGS])
AC_MSG_NOTICE([LDFLAGS  = $LDFLAGS])

dnl Makefiles
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  ])

dnl Generate files
AC_OUTPUT

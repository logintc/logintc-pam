AC_INIT([logintc-pam], [1.0.0], [support@cyphercor.com])

# Init automake
AM_INIT_AUTOMAKE([foreign])

LT_INIT

AC_PROG_CC
AC_CHECK_HEADERS([logintc/globals.h logintc/logintc.h logintc/session.h security/pam_modules.h time.h unistd.h])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_MACRO_DIR([m4])

AC_OUTPUT

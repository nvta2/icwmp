AC_INIT([icwmp], [0.1], [mohamed.kallel@pivasoftware.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([cwmp.c])

AM_INIT_AUTOMAKE([subdir-objects])

# additional options

AC_ARG_ENABLE(acs, [AS_HELP_STRING([--enable-acs],
	[specify which ACS is going to be used; there are some optimizations available for HDM and Fusion ACS (default --enable-acs=multi]))],,
	[AC_DEFINE(ACS_MULTI)])

AS_IF([test "x$enable_acs" = "xhdm"], [AC_DEFINE(ACS_HDM)])
AS_IF([test "x$enable_acs" = "xfusion"], [AC_DEFINE(ACS_FUSION)])
AS_IF([test "x$enable_acs" = "xmulti"], [AC_DEFINE(ACS_MULTI)])
AS_IF([test "x$enable_acs" = "xyes"], [AC_DEFINE(ACS_MULTI)])
AS_IF([test "x$enable_acs" = "xno"], [AC_DEFINE(ACS_MULTI)])

AC_ARG_ENABLE(http, [AS_HELP_STRING([--enable-http],
	[specify which HTTP library is going to be used; one can choose from zstream and curl (default --enable-http=curl]))],,)

AM_CONDITIONAL([HTTP_ZSTREAM], [test "x$enable_http" = "xzstream" ])
AM_CONDITIONAL([HTTP_CURL], [test "x$enable_http" = "xcurl" ])

AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug], [enable debugging messages])], AC_DEFINE(WITH_CWMP_DEBUG),)
AC_ARG_ENABLE(devel, [AS_HELP_STRING([--enable-devel], [enable development messages])], AC_DEFINE(WITH_DEV_DEBUG),)
AC_ARG_ENABLE(dummy_mode, [AS_HELP_STRING([--enable-dummy-mode], [enable dummy mode])], AC_DEFINE(DUMMY_MODE),)

AC_ARG_ENABLE(icwmp_tr098, [AS_HELP_STRING([--enable-icwmp_tr098], [enable icwmp tr098])], AC_DEFINE(ICWMP_TR098),)
AM_CONDITIONAL([ICWMP_TR098],[test "x$enable_icwmp_tr098" = "xyes"])

AC_ARG_ENABLE(icwmp_xmpp, [AS_HELP_STRING([--enable-icwmp_xmpp], [enable icwmp xmpp])], AC_DEFINE(ICWMP_XMPP),)
AM_CONDITIONAL([ICWMP_XMPP],[test "x$enable_icwmp_xmpp" = "xyes"])

AC_ARG_ENABLE(icwmp_stun, [AS_HELP_STRING([--enable-icwmp_stun], [enable icwmp stun])], AC_DEFINE(ICWMP_STUN),)
AM_CONDITIONAL([ICWMP_STUN],[test "x$enable_icwmp_stun" = "xyes"])

AC_ARG_ENABLE(icwmp_udpechoserver, [AS_HELP_STRING([--enable-icwmp_udpechoserver], [enable icwmp udpechoserver])], AC_DEFINE(ICWMP_UDPECHOSERVER),)
AM_CONDITIONAL([ICWMP_UDPECHOSERVER],[test "x$enable_icwmp_udpechoserver" = "xyes"])

AC_ARG_ENABLE(icwmp_twamp, [AS_HELP_STRING([--enable-icwmp_twamp], [enable icwmp twamp])], AC_DEFINE(ICWMP_TWAMP),)
AM_CONDITIONAL([ICWMP_TWAMP],[test "x$enable_icwmp_twamp" = "xyes"])

AC_ARG_ENABLE(icwmp_bulkdata, [AS_HELP_STRING([--enable-icwmp_bulkdata], [enable icwmp bulkdata])], AC_DEFINE(ICWMP_BULKDATA),)
AM_CONDITIONAL([ICWMP_BULKDATA],[test "x$enable_icwmp_bulkdata" = "xyes"])

# checks for programs
AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT
AC_ENABLE_SHARED

# checks for libraries
LIBPTHREAD_LIBS='-lpthread'
AC_SUBST([LIBPTHREAD_LIBS])

# checks for libraries
LIBSTROPHE_LIBS='-lstrophe'
AC_SUBST([LIBSTROPHE_LIBS])

LIBZ_LIBS='-lz'
AC_SUBST([LIBZ_LIBS])

LIBM_LIBS='-lm'
AC_SUBST([LIBM_LIBS])
LIBJSON_LIBS='-ljson-c'
AC_SUBST([LIBJSON_LIBS])

AC_ARG_WITH([uci-include-path],
  [AS_HELP_STRING([--with-uci-include-path],
    [location of the uci library headers])],
  [LIBUCI_CFLAGS="-I$withval"])
AC_SUBST([LIBUCI_CFLAGS])

AC_ARG_WITH([uci-lib-path],
  [AS_HELP_STRING([--with-uci-lib-path], [location of the uci library])], [LIBUCI_LDFLAGS="-L$withval"])
AC_SUBST([LIBUCI_LDFLAGS])

LIBUCI_LIBS='-luci'
AC_SUBST([LIBUCI_LIBS])

AC_ARG_WITH([libubox-include-path],
  [AS_HELP_STRING([--with-libubox-include-path],
    [location of the libubox library headers])],
  [LIBUBOX_CFLAGS="-I$withval"])
AC_SUBST([LIBUBOX_CFLAGS])

AC_ARG_WITH([libubox-lib-path],
  [AS_HELP_STRING([--with-libubox-lib-path], [location of the libubox library])], [LIBUBOX_LDFLAGS="-L$withval"])
AC_SUBST([LIBUBOX_LDFLAGS])

LIBUBOX_LIBS='-lubox'
AC_SUBST([LIBUBOX_LIBS])

AC_ARG_WITH([libubus-include-path],
  [AS_HELP_STRING([--with-libubus-include-path],
    [location of the libubus library headers])],
  [LIBUBUS_CFLAGS="-I$withval"])
AC_SUBST([LIBUBUS_CFLAGS])

AC_ARG_WITH([libubus-lib-path],
  [AS_HELP_STRING([--with-libubus-lib-path], [location of the libubus library])], [LIBUBOX_LDFLAGS="-L$withval"])
AC_SUBST([LIBUBUS_LDFLAGS])

LIBUBUS_LIBS='-lubus'
AC_SUBST([LIBUBUS_LIBS])

LBLOBMSG_LIBS='-lblobmsg_json'
AC_SUBST([LBLOBMSG_LIBS])

LCRYPTO_LIBS='-lcrypto'
AC_SUBST([LCRYPTO_LIBS])

LSSL_LIBS='-lssl'
AC_SUBST([LSSL_LIBS])

AM_COND_IF([HTTP_CURL], [
 AC_DEFINE(HTTP_CURL)
 PKG_CHECK_MODULES(LIBCURL, [libcurl])
 AC_SUBST(LIBCURL_CFLAGS)
 AC_SUBST(LIBCURL_LDFLAGS)
 AC_SUBST(LIBCURL_LIBS)
])

AM_COND_IF([HTTP_ZSTREAM], [
 AC_DEFINE(HTTP_ZSTREAM)
 AC_ARG_WITH([zstream-include-path],
   [AS_HELP_STRING([--with-zstream-include-path],
     [location of the zstream library headers])],
   [LIBZSTREAM_CFLAGS="-I$withval"])
 AC_SUBST([LIBZSTREAM_CFLAGS])

 AC_ARG_WITH([zstream-lib-path],
   [AS_HELP_STRING([--with-zstream-lib-path], [location of the zstream library])], [LIBZSTREAM_LDFLAGS="-L$withval"])

 LIBZSTREAM_LIBS='-lzstream'
 AC_SUBST([LIBZSTREAM_LIBS])
])

PKG_CHECK_MODULES(MICROXML, [microxml])
AC_SUBST(MICROXML_CFLAGS)
AC_SUBST(MICROXML_LDFLAGS)
AC_SUBST(MICROXML_LIBS)

# checks for header files
AC_CHECK_HEADERS([stdlib.h string.h])

# checks for typedefs, structures, and compiler characteristics
AC_TYPE_UINT8_T

# Makefiles
AC_CONFIG_FILES([
Makefile
bin/Makefile
])

AC_OUTPUT

AC_INIT([rezlooks], [0.6])
AC_CONFIG_SRCDIR([README])

AM_INIT_AUTOMAKE

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Initialize libtool
AM_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

enableval="n"
disableval="y"

AC_ARG_ENABLE(animation, 	[  --enable-animation		compile rezlooks with animation support], 	[animation=$disableval], 	[animation=$enableval])

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.8.0,,
                  AC_MSG_ERROR([GTK+-2.8 is required to compile rezlooks]))

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-2.0`
AC_SUBST(GTK_VERSION)

AC_SUBST(BUILD_ENGINES)
AC_SUBST(BUILD_THEMES)

if test $animation = "y"; then
	AC_DEFINE_UNQUOTED(HAVE_ANIMATION, 1, [Defines whether to compile with animation support])
fi

AM_CONFIG_HEADER([src/config.h])

AC_CONFIG_FILES([
Makefile
rezlooks.pc
]) 

AC_OUTPUT

echo "Now run make."

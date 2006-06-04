# Process this file with autoconf to produce a configure script.
# require autoconf 2.59
AC_PREREQ(2.59)

AC_INIT([gcontainer],[1.0.0],[ntd@users.sourceforge.net],[gcontainer])
AM_INIT_AUTOMAKE
PACKAGE_DESCRIPTION="A glib-object based container manager"
AC_SUBST([PACKAGE_DESCRIPTION])

AC_CONFIG_SRCDIR(configure.ac)

AM_MAINTAINER_MODE
AC_CANONICAL_HOST


##################################################
# Checks for programs.
##################################################

AC_PROG_CC
AC_DISABLE_SHARED
AC_PROG_LIBTOOL


##################################################
# Checking typedefs, structures and compiler characteristics
##################################################

AC_LANG(C)
AC_C_CONST


##################################################
# Check for packages.
##################################################

PKG_CHECK_MODULES([GOBJECT],[glib-2.0 >= 2.10.0 gobject-2.0 >= 2.10.0])


##################################################
# Check for gtk-doc.
##################################################

GTK_DOC_CHECK(1.4)


##################################################
# Final step.
##################################################

AC_CONFIG_FILES([Makefile
		 gcontainer.pc
		 gcontainer/Makefile
		 test/Makefile
		 docs/Makefile
		 docs/version.xml])
AC_OUTPUT

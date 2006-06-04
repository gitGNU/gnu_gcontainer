## Process this file with automake to produce Makefile.in

AM_CFLAGS  =			@GOBJECT_CFLAGS@ -I$(top_srcdir)
AM_LDFLAGS =			@GOBJECT_LIBS@

libgcontainer_includedir =	$(includedir)/gcontainer
libgcontainer_include_DATA =	gchild.h \
				gchildable.h \
				gcontainer.h \
				gcontainerable.h

lib_LTLIBRARIES = 		libgcontainer.la
libgcontainer_la_LDFLAGS =	-release @PACKAGE_VERSION@
libgcontainer_la_SOURCES =	gchild.c \
				gchild.h \
				gchildable.c \
				gchildable.h \
				gcontainer.c \
				gcontainer.h \
				gcontainerable.c \
				gcontainerable.h \
				gcontainerintl.h

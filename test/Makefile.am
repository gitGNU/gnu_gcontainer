## Process this file with automake to produce Makefile.in

AM_CFLAGS  =		@GOBJECT_CFLAGS@ -I$(top_srcdir)
AM_LDFLAGS =		@GOBJECT_LIBS@
LDADD =			$(top_builddir)/gcontainer/libgcontainer.la

exampledir =		$(pkgdatadir)/examples
example_PROGRAMS =	demo misuse

demo_SOURCES =		demo.c \
			demo.h \
			debug.c
misuse_SOURCES =	misuse.c \
			demo.h \
			debug.c

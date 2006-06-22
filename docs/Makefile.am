## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.6

DOC_MODULE=gcontainer
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR=../gcontainer
SCANGOBJ_OPTIONS=
SCAN_OPTIONS=
MKDB_OPTIONS=--sgml-mode --output-format=xml
MKTMPL_OPTIONS=
FIXXREF_OPTIONS=
HFILE_GLOB=$(top_srcdir)/gcontainer/*.h
CFILE_GLOB=$(top_srcdir)/gcontainer/*.c
IGNORE_HFILES=		\
       gcontainerintl.h \
       gobjectmissings.h
HTML_IMAGES=
content_files=
expand_content_files=

INCLUDES=@GOBJECT_CFLAGS@ -I$(top_srcdir)
GTKDOC_LIBS=@GOBJECT_LIBS@ -L$(top_builddir)/gcontainer -lgcontainer

include $(top_srcdir)/gtk-doc.make

EXTRA_DIST += version.xml.in

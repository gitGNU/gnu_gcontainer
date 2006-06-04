#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/configure.ac && test -d $srcdir/gcontainer && test -f $srcdir/gcontainer/gcontainer.h) ||
{
        echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
        echo " top-level gcontainer directory"
        exit 1
}

pushd $srcdir
gtkdocize
autoreconf -is
popd

#!/bin/sh
# Run this to generate all the initial makefiles, etc.
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="Tenes Empanadas Graciela"

(test -f $srcdir/configure.in \
  && test -f $srcdir/ChangeLog \
  && test -d $srcdir/client) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

echo "Running glib-gettextize..."
glib-gettextize --force --copy

echo "Running intltoolize..."
intltoolize --copy --force --automake

echo "Running autoreconf..."
autoreconf --verbose --install --force

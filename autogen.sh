#!/bin/sh 

PROJECT="The GIMP Extra Data Files"
TEST_TYPE=-f
FILE="brushes/flower.gbr"

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
ORIGDIR=`pwd`
cd $srcdir


DIE=0

echo -n "checking for autoconf ... "
(autoconf --version) < /dev/null > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo
    echo "  You must have autoconf installed to compile $PROJECT."
    echo "  Download the appropriate package for your distribution,"
    echo "  or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
    DIE=1;
fi

echo
echo -n "checking for automake ... "
(automake --version) < /dev/null > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo
    echo "  You must have automake installed to compile $PROJECT."
    echo "  Get ftp://ftp.gnu.org/pub/gnu/automake/automake-1.7.3.tar.gz"
    echo "  (or a newer version if it is available)."
    DIE=1
fi

if test "$DIE" -eq 1; then
    echo
    echo "Please install/upgrade the missing tools and call me again."
    echo	
    exit 1
fi


test $TEST_TYPE $FILE || {
    echo
    echo "You must run this script in the top-level $PROJECT directory."
    echo
    exit 1
}


if test -z "$*"; then
    echo
    echo "I am going to run ./configure with no arguments - if you wish "
    echo "to pass any to it, please specify them on the $0 command line."
    echo
fi

if ! aclocal $ACLOCAL_FLAGS; then
   echo
   echo "aclocal gave errors. Please fix the error conditions and try again."
   exit 1
fi

automake --add-missing
autoconf

cd $ORIGDIR

if $srcdir/configure --enable-maintainer-mode "$@"; then
  echo
  echo "Now type 'make' to compile $PROJECT."
else
  echo
  echo "Configure failed or did not finish!"
fi

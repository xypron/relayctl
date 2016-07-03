#!/bin/sh

test -f configure.ac || {
  echo "Please, run this script in the top level project directory."
  exit
}

libtoolize --force --copy
aclocal -I m4
autoconf
automake --add-missing --force --copy

echo "For installation instructions, please, refer to file INSTALL."

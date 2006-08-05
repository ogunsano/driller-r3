#! /bin/sh

autoheader
aclocal $ACLOCAL_FLAGS \
&& automake --foreign \
&& autoconf

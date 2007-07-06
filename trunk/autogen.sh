#! /bin/sh

autoheader
aclocal $ACLOCAL_FLAGS \
&& automake \
&& autoconf

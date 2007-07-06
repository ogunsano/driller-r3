#!/bin/sh
NAME="driller-0.7"
TEMPDIR=`mktemp -d -t packagebuild.XXXXXX`

svn export -q http://66.122.11.187/svn/Driller/trunk ${TEMPDIR}/${NAME} \
&& cd ${TEMPDIR}/${NAME} \
&& ./autogen.sh \
&& rm -rf autom4te.cache \
&& cd - > /dev/null \
&& tar -C ${TEMPDIR} -czf ${NAME}.tar.gz ${NAME} \
&& rm -r ${TEMPDIR}

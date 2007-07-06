#!/bin/sh
ORIG_NAME="driller-0.7"
DEB_NAME="driller_0.7"
ORIGINAL_PWD=$PWD

# Unpack the tarball
cp $ORIG_NAME.tar.gz $DEB_NAME.orig.tar.gz \
&& tar -xzf $DEB_NAME.orig.tar.gz
cd $ORIG_NAME

# Copy the Ubuntu packaging info into the debian/ directory
cp -r metadata/ubuntu debian

dpkg-buildpackage -us -uc -S -rfakeroot \
; sudo pbuilder build ../*.dsc \
&& cp /var/cache/pbuilder/result/${DEB_NAME}* $ORIGINAL_PWD

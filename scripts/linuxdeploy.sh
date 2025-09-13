#! /bin/bash

# This is meant to be run from a built vicinae source tree, inside
# the ubuntu-22.04 based build environment.

APPDIR=$PWD/build/appdir

rm -rf $APPDIR

mkdir -p ${APPDIR}/usr/bin
mkdir -p ${APPDIR}/usr/lib

cp build/vicinae/vicinae ${APPDIR}/usr/bin/vicinae
cp extra/vicinae.png ${APPDIR}
cp extra/vicinae.desktop ${APPDIR}

# for some reason we need this, otherewise libssl is not getting included
cp /usr/lib/x86_64-linux-gnu/libssl.so.3 ${APPDIR}/usr/lib/

linuxdeployqt ${APPDIR}/usr/bin/vicinae -verbose=2 -appimage -extra-plugins=platforms/libqwayland-generic.so,platforms/libqwayland-egl.so,wayland-graphics-integration-client,wayland-decoration-client,wayland-shell-integration,tls

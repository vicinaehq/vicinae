#! /bin/bash

# This is meant to be run from a built vicinae source tree, inside
# the ubuntu-22.04 based build environment.

function die() {
	echo $1
	exit 1
}

[ $# -ne 2 ] && die "Usage: $0 <install_dir> <app_dir>"

command -v linuxdeployqt || die "linuxdeployqt needs to be in PATH"

APPDIR=$2

mkdir -p $APPDIR/usr
cp -r $1/* $APPDIR/usr

cp $(which node) ${APPDIR}/usr/bin/node
cp extra/vicinae.png ${APPDIR}
cp extra/vicinae.desktop ${APPDIR}

# for some reason we need this, otherewise libssl is not getting included
cp /usr/lib/x86_64-linux-gnu/libssl.so* ${APPDIR}/usr/lib/

linuxdeployqt					\
	$APPDIR/usr/bin/vicinae		\
	-verbose=2					\
	-appimage					\
	-extra-plugins=platforms/libqwayland-generic.so,platforms/libqwayland-egl.so,wayland-graphics-integration-client,wayland-decoration-client,wayland-shell-integration,tls

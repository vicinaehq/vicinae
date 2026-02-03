#! /bin/bash

# This is meant to be run from a built vicinae source tree, inside
# the ubuntu-22.04 based build environment.

function die() {
	echo $1
	exit 1
}

[ $# -ne 2 ] && die "Usage: $0 <install_dir> <app_dir>"

command -v linuxdeploy || die "linuxdeployqt needs to be in PATH"
command -v linuxdeploy-plugin-qt || die "linuxdeploy-plugin-qt needs to be in PATH"

APPDIR=$2

rm -rf $APPDIR
mkdir -p $APPDIR/usr
cp -r $1/* $APPDIR/usr

cp $(which node) ${APPDIR}/usr/bin/node
cp extra/vicinae.png ${APPDIR}
cp extra/vicinae.desktop ${APPDIR}

# https://github.com/linuxdeploy/linuxdeploy-plugin-qt/issues/57
cp /usr/lib/x86_64-linux-gnu/libssl.so* ${APPDIR}/usr/lib/

export EXTRA_PLATFORM_PLUGINS=libqwayland.so
export EXTRA_QT_PLUGINS=waylandcompositor

linuxdeploy --appdir $APPDIR --executable $APPDIR/usr/bin/vicinae --executable $APPDIR/usr/libexec/vicinae/vicinae-server --plugin qt --output appimage

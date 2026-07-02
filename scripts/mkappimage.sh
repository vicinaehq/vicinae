#! /bin/bash

# This is meant to be run from a built vicinae source tree, inside
# the ubuntu-22.04 based build environment.

function die() {
	echo $1
	exit 1
}

[ $# -ne 2 ] && die "Usage: $0 <install_dir> <app_dir>"

APPDIR=$2

rm -rf $APPDIR
mkdir -p $APPDIR/usr
cp -r $1/* $APPDIR/usr

cp $(which node) ${APPDIR}/usr/bin/node
cp extra/vicinae.png ${APPDIR}
cp extra/vicinae.desktop ${APPDIR}

# https://github.com/linuxdeploy/linuxdeploy-plugin-qt/issues/57
cp /usr/lib/$(uname -m)-linux-gnu/libssl.so* ${APPDIR}/usr/lib/

export QML_SOURCES_PATHS=$PWD/src/server/src/qml/qml
export EXTRA_PLATFORM_PLUGINS=libqwayland.so
export EXTRA_QT_PLUGINS=waylandcompositor

# deploy every libexec helper so none of them silently link against system Qt
EXECUTABLE_ARGS=(--executable $APPDIR/usr/bin/vicinae)
for bin in $APPDIR/usr/libexec/vicinae/*; do
	EXECUTABLE_ARGS+=(--executable $bin)
done

linuxdeploy --appdir $APPDIR "${EXECUTABLE_ARGS[@]}" --plugin qt --output appimage

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

cp extra/vicinae.png ${APPDIR}

# https://github.com/linuxdeploy/linuxdeploy-plugin-qt/issues/57
cp /usr/lib/$(uname -m)-linux-gnu/libssl.so* ${APPDIR}/usr/lib/

# Our GCC toolchain is newer than the libstdc++ shipped by LTS distros, so the
# binaries can reference GLIBCXX versions the host doesn't have. linuxdeploy
# excludelists the GCC runtime, so bundle it ourselves; RUNPATH ($ORIGIN/../lib)
# makes our binaries prefer it over the host copy (#1841).
for lib in libstdc++.so.6 libgcc_s.so.1; do
	src=$(${CXX:-g++} -print-file-name=$lib)
	case "$src" in
		/*) cp -L "$src" ${APPDIR}/usr/lib/ ;;
		*) die "$lib not found in toolchain" ;;
	esac
done

# qtkeychain dlopens libsecret instead of linking it, so linuxdeploy can't see it in the
# dependency tree and the host copy can't be loaded next to our bundled glib/openssl.
# Without it qtkeychain silently falls back to kwallet or errors out (#1632).
LIBSECRET=/usr/lib/$(uname -m)-linux-gnu/libsecret-1.so.0
[ -e "$LIBSECRET" ] || die "$LIBSECRET not found: install libsecret-1-dev in the build image"

export QML_SOURCES_PATHS=$PWD/src/server/src/ui/qml
export EXTRA_PLATFORM_PLUGINS=libqwayland.so
export EXTRA_QT_PLUGINS=waylandcompositor

# Deploy every libexec helper so none of them silently link against system Qt.
# vicinae-input-server is excluded on purpose: it links no Qt and the usr/bin
# copy linuxdeploy would make shadows the to-be setcap'd libexec one, breaking
# snippet expansion (#1691).
EXECUTABLE_ARGS=(--executable $APPDIR/usr/bin/vicinae)
for bin in $APPDIR/usr/libexec/vicinae/*; do
	[ "$(basename $bin)" = "vicinae-input-server" ] && continue
	EXECUTABLE_ARGS+=(--executable $bin)
done

# AppImage managers (AppManager, AppImageUpdate, Gear Lever) read this from
# the .upd_info ELF section. Without it, users have to paste the GitHub URL
# by hand. The glob must match linuxdeploy's output name, e.g.
# Vicinae-x86_64.AppImage.zsync. Always point at vicinaehq/vicinae so a
# rebuild from a fork still updates from official releases.
ARCH=$(uname -m)
export LDAI_UPDATE_INFORMATION="gh-releases-zsync|vicinaehq|vicinae|latest|*${ARCH}.AppImage.zsync"
export UPDATE_INFORMATION="$LDAI_UPDATE_INFORMATION"

if ! command -v zsyncmake >/dev/null 2>&1; then
	echo "warning: zsyncmake not found; the AppImage will embed update info but no .zsync sidecar will be generated" >&2
fi

linuxdeploy --appdir $APPDIR "${EXECUTABLE_ARGS[@]}" \
	--library "$LIBSECRET" \
	--desktop-file $APPDIR/usr/share/applications/vicinae.desktop \
	--plugin qt --output appimage

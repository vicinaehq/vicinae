#!/usr/bin/env bash
# Assemble Vicinae.app from build outputs. macdeployqt handles Qt frameworks
# and QML modules; dylibbundler handles the remaining non-Qt third-party
# dylibs.
# Set VICINAE_CODESIGN_IDENTITY to use a real Developer ID identity.
#
# Usage: macdeploy.sh [build-dir]
set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "macdeploy.sh: macOS only" >&2
  exit 1
fi

BUILD_DIR="${1:-build}"
if [[ ! -d "$BUILD_DIR" ]]; then
  echo "macdeploy.sh: build dir not found at $BUILD_DIR" >&2
  exit 1
fi
BUILD_DIR="$(cd "$BUILD_DIR" && pwd)"
SRC_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUNDLE="$BUILD_DIR/bin/Vicinae.app"
SIGN_IDENTITY="${VICINAE_CODESIGN_IDENTITY:--}"

SERVER_BIN="$BUILD_DIR/bin/vicinae-server"
CLI_BIN="$BUILD_DIR/bin/vicinae"
BROWSER_LINK_BIN="$BUILD_DIR/bin/vicinae-browser-link"
INFO_PLIST="$BUILD_DIR/Info.plist"

for f in "$SERVER_BIN" "$CLI_BIN" "$BROWSER_LINK_BIN" "$INFO_PLIST"; do
  if [[ ! -f "$f" ]]; then
    echo "macdeploy.sh: missing $f (build first)" >&2
    exit 1
  fi
done

QT6_CMAKE_DIR="$(sed -n 's/^Qt6_DIR:PATH=//p' "$BUILD_DIR/CMakeCache.txt" 2>/dev/null | head -1)"
if [[ -n "$QT6_CMAKE_DIR" && -d "$QT6_CMAKE_DIR/../../../bin" ]]; then
  PATH="$(cd "$QT6_CMAKE_DIR/../../../bin" && pwd):$PATH"
fi

for tool in macdeployqt dylibbundler; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "macdeploy.sh: $tool not on PATH (run 'make mac-deps')" >&2
    exit 1
  fi
done

echo "==> assembling skeleton at $BUNDLE"
rm -rf "$BUNDLE"
mkdir -p "$BUNDLE/Contents/MacOS" "$BUNDLE/Contents/Resources"
cp "$INFO_PLIST" "$BUNDLE/Contents/Info.plist"
cp "$SRC_DIR/extra/vicinae.icns" "$BUNDLE/Contents/Resources/vicinae.icns"
cp -R "$SRC_DIR/extra/themes" "$BUNDLE/Contents/Resources/themes"
cp "$SERVER_BIN" "$BUNDLE/Contents/MacOS/Vicinae"
cp "$CLI_BIN" "$BUNDLE/Contents/MacOS/vicinae-cli"
cp "$BROWSER_LINK_BIN" "$BUNDLE/Contents/MacOS/vicinae-browser-link"
chmod +w "$BUNDLE/Contents/MacOS/Vicinae" "$BUNDLE/Contents/MacOS/vicinae-cli" \
         "$BUNDLE/Contents/MacOS/vicinae-browser-link"

# must run before macdeployqt so the framework is treated as already deployed
bundle_soulver_core() {
  otool -L "$BUNDLE/Contents/MacOS/Vicinae" | grep -q "SoulverCore.framework" || return 0

  local fw
  fw="$(find "$BUILD_DIR/_deps" -type d -path "*macos-arm64_x86_64/SoulverCore.framework" 2>/dev/null | head -1)"
  if [[ -z "$fw" ]]; then
    echo "macdeploy.sh: server links SoulverCore but SoulverCore.framework not found under $BUILD_DIR/_deps" >&2
    exit 1
  fi

  echo "==> bundling SoulverCore.framework"
  local dest="$BUNDLE/Contents/Frameworks/SoulverCore.framework"
  mkdir -p "$BUNDLE/Contents/Frameworks"
  cp -R "$fw" "$BUNDLE/Contents/Frameworks/"
  rm -rf "$dest/Versions/A/Headers" "$dest/Versions/A/Modules"
  rm -f "$dest/Headers" "$dest/Modules"
}

bundle_soulver_core

echo "==> macdeployqt"
macdeployqt "$BUNDLE" -qmldir="$SRC_DIR/src/server/src/qml" -verbose=2

# we prune all the frameworks and modules we don't need
# in particular, we prune the default openssl-backed tls backend because we want to force QT to use SecureTransport
PRUNE_FRAMEWORKS=(
  QtSql QtWidgets QtLabsFolderListModel QtQmlLocalStorage QtQmlXmlListModel
  QtQuickParticles QtQuickShapes QtQuickShapesDesignHelpers
  QtQuickVectorImage QtQuickVectorImageGenerator QtQuickVectorImageHelpers
  QtQuickControls2Fusion QtQuickControls2FusionStyleImpl
  QtQuickControls2Imagine QtQuickControls2ImagineStyleImpl
  QtQuickControls2Material QtQuickControls2MaterialStyleImpl
  QtQuickControls2Universal QtQuickControls2UniversalStyleImpl
  QtQuickControls2MacOSStyleImpl QtQuickControls2IOSStyleImpl
  QtQuickControls2FluentWinUI3StyleImpl
)
for fw in "${PRUNE_FRAMEWORKS[@]}"; do
  rm -rf "$BUNDLE/Contents/Frameworks/$fw.framework"
done

for style in fusion imagine material universal macos ios fluentwinui3 native; do
  rm -f "$BUNDLE/Contents/PlugIns/quick/libqtquickcontrols2${style}styleplugin.dylib" \
        "$BUNDLE/Contents/PlugIns/quick/libqtquickcontrols2${style}styleimplplugin.dylib"
done

rm -rf "$BUNDLE/Contents/PlugIns/sqldrivers" \
       "$BUNDLE/Contents/PlugIns/styles" \
       "$BUNDLE/Contents/PlugIns/tls/libqopensslbackend.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libparticlesplugin.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libqmlshapesplugin.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libqmlfolderlistmodelplugin.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libqmlxmllistmodelplugin.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libqmllocalstorageplugin.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libqquickvectorimageplugin.dylib" \
       "$BUNDLE/Contents/PlugIns/quick/libqquickvectorimagehelpersplugin.dylib" \
       "$BUNDLE/Contents/Resources/qml/Qt" \
       "$BUNDLE/Contents/Resources/qml/QtQml/XmlListModel" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/LocalStorage" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Particles" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Shapes" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/VectorImage" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/NativeStyle" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/designer" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/Fusion" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/Imagine" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/Material" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/Universal" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/macOS" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/iOS" \
       "$BUNDLE/Contents/Resources/qml/QtQuick/Controls/FluentWinUI3"

# official Qt ships universal binaries; we only target Apple Silicon
echo "==> thin to arm64"
while IFS= read -r -d '' bin; do
  lipo -archs "$bin" 2>/dev/null | grep -q x86_64 || continue
  lipo -thin arm64 "$bin" -output "$bin.thin" && mv "$bin.thin" "$bin"
done < <(find "$BUNDLE/Contents" -type f -print0)

echo "==> dylibbundler"
dyl_args=(-of -b -cd
  -d "$BUNDLE/Contents/Frameworks"
  -p "@executable_path/../Frameworks/"
  -s "$BUNDLE/Contents/Frameworks"
  -x "$BUNDLE/Contents/MacOS/Vicinae"
  -x "$BUNDLE/Contents/MacOS/vicinae-cli"
  -x "$BUNDLE/Contents/MacOS/vicinae-browser-link")
# loose Frameworks/ dylibs included so dylibbundler rewrites any absolute
# LC_ID_DYLIB macdeployqt left in place
while IFS= read -r -d '' p; do
  dyl_args+=(-x "$p")
done < <(find "$BUNDLE/Contents/PlugIns" "$BUNDLE/Contents/Frameworks" \
              -name "*.dylib" -not -path "*.framework/*" -print0)
# dylibbundler prompts on stdin for unresolvable libraries; answer 'quit' so it
# fails instead of hanging forever
{ yes quit 2>/dev/null || true; } | dylibbundler "${dyl_args[@]}"

# macdeployqt leaves duplicate LC_RPATH entries, which dyld refuses to load
echo "==> dedupe rpaths"
FRAMEWORKS_RPATH="@executable_path/../Frameworks/"
while IFS= read -r -d '' bin; do
  count="$(otool -l "$bin" 2>/dev/null | grep -cF "path $FRAMEWORKS_RPATH " || true)"
  [[ "$count" -gt 1 ]] || continue
  echo "  deduping rpath in ${bin#"$BUNDLE"/}"
  while install_name_tool -delete_rpath "$FRAMEWORKS_RPATH" "$bin" 2>/dev/null; do :; done
  install_name_tool -add_rpath "$FRAMEWORKS_RPATH" "$bin"
done < <(find "$BUNDLE/Contents/MacOS" "$BUNDLE/Contents/Frameworks" "$BUNDLE/Contents/PlugIns" -type f -print0)

echo "==> remove build-tree rpaths"
while IFS= read -r -d '' bin; do
  while IFS= read -r rp; do
    [[ "$rp" == "$BUILD_DIR"* ]] || continue
    echo "  removing rpath $rp from ${bin#"$BUNDLE"/}"
    install_name_tool -delete_rpath "$rp" "$bin"
  done < <(otool -l "$bin" 2>/dev/null | awk '/LC_RPATH/{f=2} f && /^ *path /{print $2; f=0}')
done < <(find "$BUNDLE/Contents/MacOS" -type f -print0)

echo "==> strip"
strip -x "$BUNDLE/Contents/MacOS/Vicinae" "$BUNDLE/Contents/MacOS/vicinae-cli" \
      "$BUNDLE/Contents/MacOS/vicinae-browser-link"

echo "==> signing bundle"
sign_args=(--force --deep --sign "$SIGN_IDENTITY")

if [[ "$SIGN_IDENTITY" != "-" ]]; then
  sign_args+=(--options runtime --timestamp)
fi

codesign "${sign_args[@]}" "$BUNDLE" 2>&1 | tail -3

echo "==> audit"
fail=0
while IFS= read -r -d '' f; do
  if ! file -b "$f" 2>/dev/null | grep -q "Mach-O"; then continue; fi
  bad="$(otool -L "$f" 2>/dev/null | tail -n +2 | awk '{print $1}' |
         grep -E '^(/opt/homebrew|/usr/local|/opt/local)' || true)"
  if [[ -n "$bad" ]]; then
    echo "  $f" >&2
    echo "$bad" | sed 's/^/    /' >&2
    fail=1
  fi
done < <(find "$BUNDLE/Contents" -type f \
           \( -name "*.dylib" -o -name "*.so" -o -perm -u+x \) -print0)

if [[ "$fail" -ne 0 ]]; then
  echo "macdeploy.sh: bundle still links absolute paths (see above)" >&2
  exit 1
fi

echo "==> done: $BUNDLE"

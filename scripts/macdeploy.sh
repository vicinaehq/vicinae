#!/usr/bin/env bash
# Assemble Vicinae.app from build outputs. macdeployqt handles Qt frameworks
# and QML modules; dylibbundler handles the remaining non-Qt third-party
# dylibs (glib, harfbuzz, ...) and rewrites their install names.
# Set VICINAE_CODESIGN_IDENTITY to use a real Developer ID identity.
#
# Usage: macdeploy.sh [build-dir]
#   build-dir defaults to ./build
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
INFO_PLIST="$BUILD_DIR/Info.plist"

for f in "$SERVER_BIN" "$CLI_BIN" "$INFO_PLIST"; do
  if [[ ! -f "$f" ]]; then
    echo "macdeploy.sh: missing $f (build first)" >&2
    exit 1
  fi
done

for tool in macdeployqt dylibbundler; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "macdeploy.sh: $tool not on PATH (brew install qt dylibbundler)" >&2
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
chmod +w "$BUNDLE/Contents/MacOS/Vicinae" "$BUNDLE/Contents/MacOS/vicinae-cli"

echo "==> macdeployqt"
macdeployqt "$BUNDLE" -qmldir="$SRC_DIR/src/server/src/qml" -verbose=2

echo "==> dylibbundler"
dyl_args=(-of -b -cd
  -d "$BUNDLE/Contents/Frameworks"
  -p "@executable_path/../Frameworks/"
  -s "$BUNDLE/Contents/Frameworks"
  -x "$BUNDLE/Contents/MacOS/Vicinae"
  -x "$BUNDLE/Contents/MacOS/vicinae-cli")
while IFS= read -r -d '' p; do
  dyl_args+=(-x "$p")
done < <(find "$BUNDLE/Contents/PlugIns" -name "*.dylib" -print0)
# Also feed loose dylibs in Frameworks/ so dylibbundler rewrites any
# absolute LC_ID_DYLIB macdeployqt left in place (e.g. brotlicommon
# pulled in transitively via @rpath).
while IFS= read -r -d '' p; do
  dyl_args+=(-x "$p")
done < <(find "$BUNDLE/Contents/Frameworks" -maxdepth 1 -name "*.dylib" -print0)
dylibbundler "${dyl_args[@]}"

echo "==> signing bundle"
codesign --force --deep --sign "$SIGN_IDENTITY" "$BUNDLE" 2>&1 | tail -3

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

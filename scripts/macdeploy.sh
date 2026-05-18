#!/usr/bin/env bash
# Assemble Vicinae.app from build outputs, populate Qt frameworks, sign.
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
    echo "macdeploy.sh: missing $f (run 'make mac-bundle' first)" >&2
    exit 1
  fi
done

if ! command -v macdeployqt >/dev/null 2>&1; then
  echo "macdeploy.sh: macdeployqt not on PATH (brew install qt)" >&2
  exit 1
fi

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
macdeployqt "$BUNDLE" -qmldir="$SRC_DIR/src/server/src/qml" "-codesign=$SIGN_IDENTITY" -verbose=2

echo "==> rewriting absolute LC_ID_DYLIB on copied dylibs"
shopt -s nullglob
rewritten=()
for f in "$BUNDLE/Contents/Frameworks/"*.dylib; do
  id_path="$(otool -D "$f" 2>/dev/null | tail -n +2 | head -1)"
  case "$id_path" in
    /opt/homebrew/*|/usr/local/*|/opt/local/*)
      chmod u+w "$f"
      install_name_tool -id "@executable_path/../Frameworks/$(basename "$f")" "$f"
      chmod u-w "$f"
      rewritten+=("$f")
      ;;
  esac
done
shopt -u nullglob

echo "==> stripping absolute LC_RPATH entries"
while IFS= read -r -d '' f; do
  if ! file -b "$f" 2>/dev/null | grep -q "Mach-O"; then continue; fi
  touched=0
  while IFS= read -r rpath; do
    case "$rpath" in
      ""|@*) ;;
      *)
        chmod u+w "$f"
        install_name_tool -delete_rpath "$rpath" "$f" 2>/dev/null || true
        touched=1
        ;;
    esac
  done < <(otool -l "$f" 2>/dev/null |
    awk '/LC_RPATH/{getline;getline;sub(/ \(offset [0-9]+\)/,"");sub(/^[[:space:]]*path[[:space:]]*/,"");print}')
  if [[ "$touched" -eq 1 ]]; then
    rewritten+=("$f")
    chmod u-w "$f" 2>/dev/null || true
  fi
done < <(find "$BUNDLE/Contents" -type f \
           \( -name "*.dylib" -o -name "*.so" -o -perm -u+x \) -print0)

echo "==> re-signing files modified by install_name_tool"
if [[ "${#rewritten[@]}" -gt 0 ]]; then
  printf '%s\n' "${rewritten[@]}" | sort -u | while IFS= read -r f; do
    codesign --force --sign "$SIGN_IDENTITY" "$f" 2>/dev/null || true
  done
fi
codesign --force --sign "$SIGN_IDENTITY" "$BUNDLE" 2>&1 | tail -3

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
  echo "  macdeployqt missed a non-Qt dep. Add the lib's source dir to" >&2
  echo "  macdeployqt's -libpath, or copy the lib in manually." >&2
  exit 1
fi

echo "==> done: $BUNDLE"

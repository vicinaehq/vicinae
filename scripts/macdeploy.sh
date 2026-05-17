#!/usr/bin/env bash
# Make a Vicinae.app bundle portable.
#
# 1. macdeployqt copies Qt frameworks + their transitive Homebrew deps
#    into Contents/Frameworks/ and rewrites consumer load commands to
#    @executable_path/../Frameworks/.
# 2. macdeployqt is known to leave copied dylibs' own install names
#    (LC_ID_DYLIB) pointing at their original absolute Homebrew paths,
#    which breaks codesign --verify. We rewrite those here.
# 3. Audit: scan every Mach-O in the bundle and fail if any load
#    command still resolves to an absolute Homebrew path.
#
# We deliberately do NOT use dylibbundler: it owns the Frameworks dir
# (the -od flag erases it on every run) and does not compose with
# macdeployqt's output. macdeployqt + install_name_tool fixup is the
# pattern used by Tiled, Krita, and Conan's fix_apple_shared_install_name.
# Codesigning is out of scope here.
#
# Usage: macdeploy.sh [bundle-path]
#   bundle-path defaults to build/bin/Vicinae.app
set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "macdeploy.sh: macOS only" >&2
  exit 1
fi

BUNDLE="${1:-build/bin/Vicinae.app}"
if [[ ! -d "$BUNDLE" ]]; then
  echo "macdeploy.sh: bundle not found at $BUNDLE (run 'make mac' first)" >&2
  exit 1
fi
BUNDLE="$(cd "$BUNDLE" && pwd)"

if ! command -v macdeployqt >/dev/null 2>&1; then
  echo "macdeploy.sh: macdeployqt not on PATH (brew install qt)" >&2
  exit 1
fi

QML_DIR="$(cd "$(dirname "$0")/.." && pwd)/src/server/src/qml"

echo "==> macdeployqt"
macdeployqt "$BUNDLE" -qmldir="$QML_DIR" -verbose=2

echo "==> rewriting absolute install names"
shopt -s nullglob
for f in "$BUNDLE/Contents/Frameworks/"*.dylib; do
  id_path="$(otool -D "$f" 2>/dev/null | tail -n +2 | head -1)"
  case "$id_path" in
    /opt/homebrew/*|/usr/local/*|/opt/local/*)
      chmod u+w "$f"
      install_name_tool -id "@executable_path/../Frameworks/$(basename "$f")" "$f"
      ;;
  esac
done
shopt -u nullglob

echo "==> stripping absolute LC_RPATH entries"
while IFS= read -r -d '' f; do
  if ! file -b "$f" 2>/dev/null | grep -q "Mach-O"; then continue; fi
  while IFS= read -r rpath; do
    case "$rpath" in
      ""|@*) ;;
      *) chmod u+w "$f"; install_name_tool -delete_rpath "$rpath" "$f" 2>/dev/null || true ;;
    esac
  done < <(otool -l "$f" 2>/dev/null |
    awk '/LC_RPATH/{getline;getline;sub(/ \(offset [0-9]+\)/,"");sub(/^[[:space:]]*path[[:space:]]*/,"");print}')
done < <(find "$BUNDLE/Contents" -type f \
           \( -name "*.dylib" -o -name "*.so" -o -perm -u+x \) -print0)

echo "==> ad-hoc resign"
# install_name_tool invalidates macdeployqt's ad-hoc signature; without
# this step macOS AMFI silently kills the binary on launch (exit 0, no
# output). Proper Developer ID signing for distribution is a separate
# concern.
codesign --force --deep --sign - "$BUNDLE" 2>&1 | tail -3

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
  echo "  — macdeployqt missed a non-Qt dep. Add the lib's source dir to" >&2
  echo "    macdeployqt's -libpath, or copy the lib in manually." >&2
  exit 1
fi

echo "==> done: $BUNDLE"

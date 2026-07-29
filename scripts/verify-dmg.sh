#!/usr/bin/env bash
# Verify a distributable dmg the way an end user's machine would.
#
# Usage: verify-dmg.sh [path-to-dmg]
#   path-to-dmg defaults to build/Vicinae.dmg
set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "verify-dmg.sh: macOS only" >&2
  exit 1
fi

DMG="${1:-build/Vicinae.dmg}"
if [[ ! -f "$DMG" ]]; then
  echo "verify-dmg.sh: $DMG not found" >&2
  exit 1
fi

WORK="$(mktemp -d)"
MNT="$WORK/mnt"
cleanup() {
  hdiutil detach "$MNT" -quiet 2>/dev/null || true
  rm -rf "$WORK"
}
trap cleanup EXIT

echo "==> notarization staple"
xcrun stapler validate "$DMG"

echo "==> gatekeeper (dmg)"
spctl -a -t open --context context:primary-signature -vv "$DMG"

echo "==> gatekeeper (quarantined app)"
cp "$DMG" "$WORK/download.dmg"
xattr -w com.apple.quarantine "0081;$(printf '%x' "$(date +%s)");Safari;$(uuidgen)" "$WORK/download.dmg"
hdiutil attach "$WORK/download.dmg" -nobrowse -mountpoint "$MNT" -quiet
APP="$WORK/Vicinae.app"
cp -R "$MNT/Vicinae.app" "$APP"
hdiutil detach "$MNT" -quiet
spctl -a -t exec -vv "$APP"

echo "==> dylib resolution"
FW="$APP/Contents/Frameworks"
fail=0
while IFS= read -r -d '' f; do
  file -b "$f" 2>/dev/null | grep -q "Mach-O" || continue
  while IFS= read -r dep; do
    case "$dep" in
      /usr/lib/*|/System/*|@loader_path/*) ;;
      @rpath/*)
        if [[ ! -e "$FW/${dep#@rpath/}" ]]; then
          echo "  unresolved: $dep (needed by ${f#"$APP"/})" >&2
          fail=1
        fi
        ;;
      @executable_path/../Frameworks/*)
        if [[ ! -e "$FW/${dep#@executable_path/../Frameworks/}" ]]; then
          echo "  unresolved: $dep (needed by ${f#"$APP"/})" >&2
          fail=1
        fi
        ;;
      *)
        echo "  non-bundled: $dep (needed by ${f#"$APP"/})" >&2
        fail=1
        ;;
    esac
  done < <(otool -L "$f" 2>/dev/null | tail -n +2 | awk '{print $1}')
done < <(find "$APP/Contents" -type f -print0)
if [[ "$fail" -ne 0 ]]; then
  echo "verify-dmg.sh: bundle has unresolved dependencies" >&2
  exit 1
fi

echo "==> launch check"
xattr -dr com.apple.quarantine "$APP"
"$APP/Contents/MacOS/Vicinae" &
APP_PID=$!
status=0
for _ in 1 2 3 4 5; do
  kill -0 "$APP_PID" 2>/dev/null || break
  sleep 1
done
if kill -0 "$APP_PID" 2>/dev/null; then
  kill "$APP_PID"
  wait "$APP_PID" 2>/dev/null || true
else
  wait "$APP_PID" || status=$?
fi
if [[ "$status" -ne 0 ]]; then
  echo "verify-dmg.sh: app exited with status $status" >&2
  exit 1
fi

echo "==> ok: $DMG"

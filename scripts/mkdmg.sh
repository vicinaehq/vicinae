#!/usr/bin/env bash
# Pack build/bin/Vicinae.app into Vicinae.dmg. The output name must not change:
# the auto updater looks for a release asset named exactly Vicinae.dmg.
#
# Usage: mkdmg.sh [build-dir]
#   build-dir defaults to ./build
set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "mkdmg.sh: macOS only" >&2
  exit 1
fi

BUILD_DIR="${1:-build}"
if [[ ! -d "$BUILD_DIR" ]]; then
  echo "mkdmg.sh: build dir not found at $BUILD_DIR" >&2
  exit 1
fi
BUILD_DIR="$(cd "$BUILD_DIR" && pwd)"
BUNDLE="$BUILD_DIR/bin/Vicinae.app"
STAGING="$BUILD_DIR/dmg-staging"
DMG="$BUILD_DIR/Vicinae.dmg"

if [[ ! -d "$BUNDLE" ]]; then
  echo "mkdmg.sh: $BUNDLE not found (run 'make mac-bundle' first)" >&2
  exit 1
fi

echo "==> staging"
rm -rf "$STAGING" "$DMG"
mkdir -p "$STAGING"
ditto "$BUNDLE" "$STAGING/Vicinae.app"
ln -s /Applications "$STAGING/Applications"

echo "==> creating $DMG"
hdiutil create -volname Vicinae -srcfolder "$STAGING" -ov -format UDZO "$DMG"
rm -rf "$STAGING"

if [[ -n "${VICINAE_CODESIGN_IDENTITY:-}" && "${VICINAE_CODESIGN_IDENTITY}" != "-" ]]; then
  echo "==> signing dmg"
  codesign --force --timestamp --sign "$VICINAE_CODESIGN_IDENTITY" "$DMG"
fi

if [[ -n "${VICINAE_NOTARY_PROFILE:-}" ]]; then
  echo "==> notarizing"
  xcrun notarytool submit "$DMG" --keychain-profile "$VICINAE_NOTARY_PROFILE" --wait
  xcrun stapler staple "$DMG"
fi

echo "==> done: $DMG"

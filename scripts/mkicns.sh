#!/usr/bin/env bash
# Build a macOS .icns from a 1024x1024 source PNG.
# Usage: mkicns.sh <input.png> <output.icns>
set -euo pipefail

INPUT="${1:?input png required}"
OUTPUT="${2:?output icns required}"

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "mkicns.sh: macOS only (needs iconutil/sips)" >&2
  exit 1
fi

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT
ICONSET="$WORKDIR/icon.iconset"
mkdir -p "$ICONSET"

declare -a SIZES=(
  "16:icon_16x16.png"
  "32:icon_16x16@2x.png"
  "32:icon_32x32.png"
  "64:icon_32x32@2x.png"
  "128:icon_128x128.png"
  "256:icon_128x128@2x.png"
  "256:icon_256x256.png"
  "512:icon_256x256@2x.png"
  "512:icon_512x512.png"
  "1024:icon_512x512@2x.png"
)

for entry in "${SIZES[@]}"; do
  size="${entry%%:*}"
  name="${entry##*:}"
  sips -z "$size" "$size" "$INPUT" --out "$ICONSET/$name" >/dev/null
done

iconutil -c icns -o "$OUTPUT" "$ICONSET"

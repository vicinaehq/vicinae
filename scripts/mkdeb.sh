#!/usr/bin/env bash
#
# Builds debian packages, assumes binaries have already been built.

set -euo pipefail

VERSION="${VERSION:-""}"
if [[ -z "$VERSION" ]]; then
  echo "Error: Missing env var VERSION" >&2
  exit 1
fi

VERSION="${VERSION/v/}"

BUILD_DIR="$(pwd)/build"
INSTALL_DIR="$(pwd)/install"
if [[ ! -e "$INSTALL_DIR" ]]; then
  echo "Error: INSTALL_DIR \"$INSTALL_DIR\" could not be found" \
  "(was vicinae built?)" >&2
  exit 1
elif [[ ! -d "$BUILD_DIR" ]]; then
  echo "Error: BUILD_DIR \"$BUILD_DIR\" could not be found" \
  "(was vicinae built?)" >&2
  exit 1
fi

if ! command -v nfpm &>/dev/null; then
  echo "Error: nfpm is missing, please install it:" >&2
  echo "       https://nfpm.goreleaser.com/docs/install/" >&2
  exit 1
fi

exec nfpm package -p deb
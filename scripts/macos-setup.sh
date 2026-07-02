#!/usr/bin/env bash
# Install Vicinae build + deploy dependencies via Homebrew.
set -euo pipefail

[[ "$(uname -s)" == "Darwin" ]] || { echo "You need to be on a Mac to run this script" >&2; exit 1; }
command -v brew >/dev/null || { echo "install Homebrew first: https://brew.sh" >&2; exit 1; }

brew install \
  cmake \
  ninja \
  pkg-config \
  clang-format \
  qt \
  openssl@3 \
  qtkeychain \
  libqalculate \
  minizip \
  dylibbundler \
  ccache	\
  extra-cmake-modules

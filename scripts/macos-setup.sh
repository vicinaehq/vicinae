#!/usr/bin/env bash
# Install Vicinae build + deploy dependencies.
# Qt comes from the official binaries (via aqtinstall) rather than homebrew
set -euo pipefail

[[ "$(uname -s)" == "Darwin" ]] || { echo "You need to be on a Mac to run this script" >&2; exit 1; }
command -v brew >/dev/null || { echo "install Homebrew first: https://brew.sh" >&2; exit 1; }

QT_VERSION="${QT_VERSION:-6.11.1}"
QT_DIR="$HOME/Qt/$QT_VERSION/macos"

brew install \
  cmake \
  ninja \
  pkg-config \
  clang-format \
  pipx \
  libqalculate \
  dylibbundler \
  ccache	\
  extra-cmake-modules

if [[ ! -d "$QT_DIR" ]]; then
  echo "==> installing official Qt $QT_VERSION to ~/Qt"
  pipx run --spec aqtinstall aqt install-qt mac desktop "$QT_VERSION" clang_64 -m qtshadertools -O "$HOME/Qt"
fi

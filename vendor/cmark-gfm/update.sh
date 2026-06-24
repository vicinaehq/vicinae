#!/usr/bin/env bash
#
# Re-vendors cmark-gfm from upstream at a pinned tag and re-applies our local
# modifications. Run from anywhere; it operates on its own directory.
#
#   ./update.sh [tag]
#
# Local modifications kept on top of the pristine upstream tree:
#   - CMakeLists.txt: cmake_minimum_required bumped to 3.16 (drops the need for
#     CMAKE_POLICY_VERSION_MINIMUM) and the test/api_test/man/fuzz subdirectories
#     are removed (we only build the static libraries).
#
# Everything else lives in cmake/CMark.cmake (option overrides, install
# suppression, include-dir workaround); no source edits are required.

set -euo pipefail

REPO="https://github.com/github/cmark-gfm"
TAG="${1:-0.29.0.gfm.13}"

DEST="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

echo "Cloning $REPO @ $TAG"
git clone --depth 1 --branch "$TAG" "$REPO" "$TMP/cmark-gfm"
rm -rf "$TMP/cmark-gfm/.git"

# Strip everything not needed to build the static libraries.
( cd "$TMP/cmark-gfm" && rm -rf \
	test api_test fuzz bench data tools wrappers man .github \
	.travis.yml appveyor.yml Makefile Makefile.nmake nmake.bat \
	toolchain-mingw32.cmake suppressions benchmarks.md why-cmark-and-not-x.md \
	.editorconfig .gitignore )

# Re-apply local CMake edits.
sed -i.bak 's/^cmake_minimum_required(VERSION 3\.0)/cmake_minimum_required(VERSION 3.16)/' \
	"$TMP/cmark-gfm/CMakeLists.txt"
rm -f "$TMP/cmark-gfm/CMakeLists.txt.bak"

# Drop the test/api_test/man/fuzz subdirectory hookups from the root CMakeLists.
python3 - "$TMP/cmark-gfm/CMakeLists.txt" <<'PY'
import re, sys
p = sys.argv[1]
s = open(p).read()
s = s.replace(
"""add_subdirectory(src)
add_subdirectory(extensions)
if(CMARK_TESTS AND (CMARK_SHARED OR CMARK_STATIC))
  add_subdirectory(api_test)
endif()
add_subdirectory(man)
if(CMARK_TESTS)
  enable_testing()
  add_subdirectory(test testdir)
endif()
if(CMARK_FUZZ_QUADRATIC)
  add_subdirectory(fuzz)
endif()
""",
"""add_subdirectory(src)
add_subdirectory(extensions)
""")
open(p, "w").write(s)
PY

# Swap the freshly vendored tree into place, preserving this script.
find "$DEST" -mindepth 1 -maxdepth 1 ! -name update.sh -exec rm -rf {} +
cp -R "$TMP/cmark-gfm/." "$DEST/"

echo "Vendored cmark-gfm @ $TAG into $DEST"
echo "Review the diff and rebuild."

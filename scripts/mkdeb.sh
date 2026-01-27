#!/bin/bash
#
# Vicinae DEB package build script
#
# This script builds a .deb package for Vicinae.
# It copies the debian directory to the project root, updates the changelog
# with the current version from git tags, builds the package, and cleans up.
#
# Usage: ./scripts/mkdeb.sh [--no-clean]
#
# Options:
#   --no-clean    Do not remove the debian/ directory after build
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
DEBIAN_SRC="$SCRIPT_DIR/packaging/debian"
DEBIAN_DST="$PROJECT_ROOT/debian"

# Parse arguments
NO_CLEAN=false
for arg in "$@"; do
    case $arg in
        --no-clean)
            NO_CLEAN=true
            shift
            ;;
        *)
            echo "Unknown option: $arg"
            echo "Usage: $0 [--no-clean]"
            exit 1
            ;;
    esac
done

die() {
    echo "Error: $1" >&2
    exit 1
}

info() {
    echo "==> $1"
}

cleanup() {
    if [ "$NO_CLEAN" = false ]; then
        info "Cleaning up temporary debian directory..."
        rm -rf "$DEBIAN_DST"
    else
        info "Keeping debian directory (--no-clean specified)"
    fi
}

# Set up cleanup trap
trap cleanup EXIT

# Check for required tools
for cmd in dpkg-buildpackage dch lintian; do
    command -v "$cmd" >/dev/null 2>&1 || die "$cmd is not installed. Install with: sudo apt install devscripts debhelper lintian"
done

cd "$PROJECT_ROOT"

# Get version from git tag
info "Determining version from git tags..."
if git describe --tags --abbrev=0 >/dev/null 2>&1; then
    VERSION=$(git describe --tags --abbrev=0 | sed 's/^v//')
else
    # Fallback to reading from CMakeLists.txt or use a default
    VERSION=$(grep -oP 'project\([^)]*VERSION\s+\K[0-9.]+' CMakeLists.txt 2>/dev/null || echo "0.0.0")
    echo "Warning: No git tag found, using version $VERSION"
fi

info "Building version: $VERSION"

# Check if debian directory already exists in project root
if [ -d "$DEBIAN_DST" ]; then
    die "debian/ directory already exists in project root. Please remove it first."
fi

# Copy debian directory to project root
info "Copying debian packaging files..."
cp -r "$DEBIAN_SRC" "$DEBIAN_DST"

# Update changelog with current version and date
info "Updating debian/changelog..."
cat > "$DEBIAN_DST/changelog" << EOF
vicinae ($VERSION) unstable; urgency=medium

  * Release version $VERSION

 -- Vicinae Developers <dev@vicinae.org>  $(date -R)
EOF

# Build the package
info "Building .deb package..."
dpkg-buildpackage -us -uc -b -d

# Move .deb file to project root
info "Moving .deb file to project root..."
mv "../vicinae_${VERSION}_"*.deb "$PROJECT_ROOT/" 2>/dev/null || true

# Also move any other build artifacts
mv ../vicinae_*.buildinfo "$PROJECT_ROOT/" 2>/dev/null || true
mv ../vicinae_*.changes "$PROJECT_ROOT/" 2>/dev/null || true

# Find the built .deb file
DEB_FILE=$(ls -1 "$PROJECT_ROOT"/vicinae_*.deb 2>/dev/null | head -n1)

if [ -z "$DEB_FILE" ]; then
    die "Failed to find built .deb file"
fi

# Run lintian (warnings don't fail the build)
info "Running lintian checks..."
lintian --no-tag-display-limit "$DEB_FILE" || true

info "Build completed successfully!"
info "Package: $DEB_FILE"

# Display package info
echo ""
echo "Package information:"
dpkg-deb -I "$DEB_FILE"

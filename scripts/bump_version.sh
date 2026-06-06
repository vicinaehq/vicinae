#! /bin/bash

set -euo pipefail

# Atomically bump the project version:
#   1. write the new tag + the commit it is based on into the manifest
#   2. commit the manifest change
#   3. tag *that* commit
#
# The tag must land on the commit that carries the updated manifest, otherwise
# anyone checking out the tag gets a manifest pointing at the previous release.

bump_version() {
    local version_type=${1:-patch}
    local manifest=${2:-./manifest.yaml}

    if ! git diff --cached --quiet; then
        echo "refusing to bump: the index has staged changes, commit or unstage them first" >&2
        exit 1
    fi

    # Use the highest version tag across the whole repo, not just tags reachable
    # from HEAD: bump commits are tagged but never merged back into the working
    # branch, so `git describe` would miss the most recent release and we'd
    # recompute a version that already exists.
    local current_tag
    current_tag=$(git tag -l 'v*' --sort=-v:refname | head -n1)
    current_tag=${current_tag:-v0.0.0}

    local current_version=${current_tag#v}
    IFS='.' read -ra VERSION_PARTS <<< "$current_version"
    local major=${VERSION_PARTS[0]:-0}
    local minor=${VERSION_PARTS[1]:-0}
    local patch=${VERSION_PARTS[2]:-0}

    case $version_type in
        major) major=$((major + 1)); minor=0; patch=0 ;;
        minor) minor=$((minor + 1)); patch=0 ;;
        patch) patch=$((patch + 1)) ;;
        *) echo "unknown version type: ${version_type} (expected major|minor|patch)" >&2; exit 1 ;;
    esac

    local new_version="v$major.$minor.$patch"

    # The release is built from the current HEAD: record it before we create the
    # bump commit so the manifest references the actual code commit.
    local rev short_rev
    rev=$(git rev-parse HEAD)
    short_rev=$(git rev-parse --short HEAD)

    yq -i ".release.tag = \"${new_version}\" | .release.rev = \"${rev}\" | .release.short_rev = \"${short_rev}\"" "$manifest"

    git add "$manifest"
    git commit -m "chore: bump to ${new_version}"
    git tag "${new_version}"

    echo "bumped to ${new_version} (tag on $(git rev-parse --short HEAD))"
}

bump_version "$@"

#! /bin/bash

bump_version() {
    local version_type=${1:-patch}
    local current_tag=$(git describe --tags --abbrev=0 2>/dev/null || echo "v0.0.0")
    
    current_version=${current_tag#v}
    IFS='.' read -ra VERSION_PARTS <<< "$current_version"
    major=${VERSION_PARTS[0]:-0}
    minor=${VERSION_PARTS[1]:-0}
    patch=${VERSION_PARTS[2]:-0}
    
    case $version_type in
        major) ((major++)); minor=0; patch=0 ;;
        minor) ((minor++)); patch=0 ;;
        patch) ((patch++)) ;;
    esac
    
    new_version="v$major.$minor.$patch"
	git tag "${new_version}"
}

bump_version $1

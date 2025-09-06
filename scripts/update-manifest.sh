#! /bin/sh

tag="$(git describe --tags --abbrev=0)"
short_rev="$(git rev-parse --short ${tag})"
rev=$(git rev-parse ${tag})

yq -i ".release.tag = \"${tag}\" | .release.rev = \"${rev}\" | .release.short_rev = \"${short_rev}\"" $1

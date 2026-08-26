#!/usr/bin/env bash
# Recompute the fetchNpmDeps hashes in nix/vicinae.nix from the npm lockfiles,
# using prefetch-npm-deps inside a nixos/nix docker container
#
# Usage:
#   scripts/update-nix-npm-hashes.sh           # update nix/vicinae.nix in place
#   scripts/update-nix-npm-hashes.sh --check   # only verify, exit 1 on mismatch (what CI does)

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
NIX_FILE="$REPO_ROOT/nix/vicinae.nix"
NIX_STORE_VOLUME="vicinae-prefetch-nix-store"

CHECK_ONLY=0
[[ "${1:-}" == "--check" ]] && CHECK_ONLY=1

red() { printf '\e[31m%s\e[0m\n' "$*" >&2; }
green() { printf '\e[32m%s\e[0m\n' "$*"; }

prefetch() {
  # The named volumes keep the container's nix store and eval cache across
  # runs so nixpkgs and prefetch-npm-deps are only downloaded the first time.
  docker run --rm \
    -v "$NIX_STORE_VOLUME":/nix \
    -v "$NIX_STORE_VOLUME-cache":/root/.cache/nix \
    -v "$1":/package-lock.json:ro \
    nixos/nix \
    nix run --extra-experimental-features 'nix-command flakes' \
    nixpkgs#prefetch-npm-deps -- /package-lock.json
}

current_hash() {
  awk -v attr="$1" '
    $0 ~ attr " = fetchNpmDeps" { inblock = 1 }
    inblock && match($0, /sha256-[^"]*/) { print substr($0, RSTART, RLENGTH); exit }
  ' "$NIX_FILE"
}

replace_hash() {
  local attr="$1" new_hash="$2" tmp
  tmp=$(mktemp)
  awk -v attr="$attr" -v hash="$new_hash" '
    $0 ~ attr " = fetchNpmDeps" { inblock = 1 }
    inblock && /sha256-/ { sub(/sha256-[^"]*/, hash); inblock = 0 }
    { print }
  ' "$NIX_FILE" >"$tmp"
  mv "$tmp" "$NIX_FILE"
}

STATUS=0

process() {
  local attr="$1" pkg_dir="$2"
  local old_hash new_hash

  old_hash=$(current_hash "$attr")
  [[ -n "$old_hash" ]] || { red "error: could not find hash for $attr in $NIX_FILE"; exit 1; }

  echo "computing hash for $pkg_dir..." >&2
  new_hash=$(prefetch "$REPO_ROOT/$pkg_dir/package-lock.json")
  [[ "$new_hash" == sha256-* ]] || { red "error: unexpected prefetch output for $pkg_dir: $new_hash"; exit 1; }

  if [[ "$old_hash" == "$new_hash" ]]; then
    green "$attr: up to date ($new_hash)"
  elif [[ "$CHECK_ONLY" == 1 ]]; then
    red "$attr: hash mismatch, expected '$new_hash' (found '$old_hash')"
    STATUS=1
  else
    replace_hash "$attr" "$new_hash"
    green "$attr: updated $old_hash -> $new_hash"
  fi
}

process apiDeps src/typescript/api
process extensionManagerDeps src/typescript/extension-manager

exit "$STATUS"

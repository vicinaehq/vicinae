#!/usr/bin/env bash
set -euo pipefail

# Simple helper to build an SRPM from vicinae.spec and trigger a COPR build.
# Assumes:
#   * you are in the vicinae repo root
#   * `copr-cli` is installed and configured (has access to the vicinae project)
#   * rpm-build tooling is installed (rpmbuild, spectool, etc.)

PROJECT_NAME="vicinae"           # COPR project name
SPEC_FILE="vicinae.spec"
ROOT_DIR="$(pwd)"
SRPM_DIR="${ROOT_DIR}/.srpm"

mkdir -p "${SRPM_DIR}"

echo "[copr-build] Fetching remote sources defined in ${SPEC_FILE}..."
spectool -g -R "${SPEC_FILE}" --define "_sourcedir ${ROOT_DIR}"

echo "[copr-build] Building source RPM..."
rpmbuild -bs "${SPEC_FILE}" \
  --define "_sourcedir ${ROOT_DIR}" \
  --define "_specdir ${ROOT_DIR}" \
  --define "_srcrpmdir ${SRPM_DIR}"

LATEST_SRPM="$(ls -t "${SRPM_DIR}"/vicinae-*.src.rpm | head -n 1)"
if [[ -z "${LATEST_SRPM:-}" ]]; then
  echo "[copr-build] ERROR: No SRPM found in ${SRPM_DIR}" >&2
  exit 1
fi

echo "[copr-build] Triggering COPR build in project '${PROJECT_NAME}' with SRPM: ${LATEST_SRPM}";
copr build "${PROJECT_NAME}" "${LATEST_SRPM}" "$@"

echo "[copr-build] Submitted build. Use 'copr list-builds ${PROJECT_NAME}' or 'copr status <build_id>' to watch it."

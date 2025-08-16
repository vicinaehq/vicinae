#! /bin/bash
# This will run a github action runner locally and connect it to GitHub
# using the provided VICINAE_GH_RUNNER_TOKEN

function fatal() {
	echo $1
	exit 1
}

[ -n "$VICINAE_GH_RUNNER_TOKEN" ] || fatal "VICINAE_GH_RUNNER_TOKEN is not set."

RUNNER_TOKEN=${VICINAE_GH_RUNNER_TOKEN} docker compose up --force-recreate --build -d

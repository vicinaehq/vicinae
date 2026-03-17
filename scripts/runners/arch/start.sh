#!/bin/bash

cd /home/runner

# Only configure if not already registered
if [ ! -f ".runner" ]; then
    RUNNER_ALLOW_RUNASROOT=true ./config.sh --url ${REPO_URL} --token ${RUNNER_TOKEN} --name ${RUNNER_NAME} --labels ${LABELS} --unattended --replace
fi

# Start runner
RUNNER_ALLOW_RUNASROOT=true ./run.sh

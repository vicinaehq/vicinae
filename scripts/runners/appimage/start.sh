#!/bin/bash

cd /home/runner

# Configure runner
RUNNER_ALLOW_RUNASROOT=true ./config.sh --replace --url ${REPO_URL} --token ${RUNNER_TOKEN} --name ${RUNNER_NAME} --unattended

# Start runner
RUNNER_ALLOW_RUNASROOT=true ./run.sh

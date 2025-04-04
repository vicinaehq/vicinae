#!/bin/bash
cd module
mv core.js core.mjs && mv duplex.js duplex.mjs && mv helpers.js helpers.mjs
# Unlike Ubuntu, OS X requires the extension to be explicitly specified
# https://myshittycode.com/2014/07/24/os-x-sed-extra-characters-at-the-end-of-l-command-error/
if [[ "$OSTYPE" == "darwin"* ]]; then
  sed -i '' 's/\.js/\.mjs/g' duplex.mjs core.mjs
else
  sed -i 's/\.js/\.mjs/g' duplex.mjs core.mjs
fi
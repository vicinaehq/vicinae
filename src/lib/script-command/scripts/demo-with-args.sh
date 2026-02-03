#!/bin/bash -c hello

# Required parameters:
# @raycast.schemaVersion 1
# @raycast.title Search Flights
# @raycast.mode silent

# Optional parameters:
# @raycast.icon 🛩
# @raycast.packageName Web Searches
# @raycast.argument1 { "type": "text", "placeholder": "from city", "percentEncoded": true }
# @raycast.argument2 { "type": "text", "placeholder": "to city", "optional": true, "percentEncoded": true }

open "https://www.google.com/search?q=flights%20from%20$1%20to%20$2"

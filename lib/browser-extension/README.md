# Vicinae browser integration

This subdirectory contains:

- native bridge between browser extensions and vicinae daemon
- chromium browser extension

# Manual install

If for any reason you don't want to use the vicinae extension from the store:

1) Install the native host manifest for your browser. This will be done automatically if you use the cmake install rule.
2) Load the unpacked extension in your browser. i.e the `chrome` folder if you are using a chromium based browser.
3) Replace the extension id in the manifest with the one of your local extension (as displayed by your browser).

# Architecture schema

If that's any helpful, here is a very simple schema of how communication between vicinae and the browser works:

![](./assets/architecture.png)

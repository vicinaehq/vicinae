# Vicinae browser integration

This subdirectory contains:

- native bridge between browser extensions and vicinae daemon
- chromium/firefox browser extension. Currently the same codebase is used for both, but eventually we may have to split them.

For now the extension is not published to the official stores, this is a work in progress.

# Manual install

If you can't install the extension from the official stores, you can install the browser extension manually.

## Install native host manifest

The browser extension uses [Native messaging](https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging) to communicate with the vicinae daemon.

You need to install the corresponding native host manifest at one of the expected locations.

The recommended way of installing the manifests is to use the provided CMake install rule to install vicinae. This rule will automatically install both manifests in the correct system locations (root access required).

Installing the manifests in system directories is **strongly recommended** because it makes them automatically discoverable by any chromium/firefox based browser.

## Load the browser extension

### Chromium bases

You can load the `./chrome/` directory as any unpacked extension. 

Then modify the native host manifest (likely at `/etc/chromium/native-messaging-hosts/com.vicinae.vicinae.json`) and change the origin in the `allowed_origins` array to use the local extension ID that was automatically generated upon loading the unpacked extension.

Once that is done, reloading the extension should automatically establish a connection with vicinae if the vicinae server is running.

### Firefox bases

You need the [web-ext](`https://github.com/mozilla/web-ext`) CLI tool to be installed.

Then make the extension bundle by running `make firefox`.

And then you can import it from `./build/firefox-vicinae.zip`.

# Architecture schema

If that's any helpful, here is a very simple schema of how communication between vicinae and the browser works:

![](./assets/architecture.png)

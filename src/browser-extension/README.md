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

Installing the manifests in system directories is **strongly recommended** because it makes them automatically discoverable by any chromium/firefox based browser.

### Using CMake

The recommended way of installing the manifests is to use the provided CMake install rule to install vicinae. This rule will automatically install both manifests in the correct system locations (root access required).

### Manual native host manifest install

Copy the right manifest template from `./native-host/` and change the placeholder values.

For firefox, you only need to replace `@NATIVE_HOST_BIN` with the absolute path to the `vicinae-browser-link` executable (e.g: `/usr/local/libexec/vicinae/vicinae-browser-link`). Then copy the manifest at `/usr/lib/mozilla/native-messaging-hosts/com.vicinae.vicinae.json`.

For chromium you will also need to change the allowed origins, more on that below. The manifest needs to be copied at `/etc/chromium/native-messaging-hosts/com.vicinae.vicinae.json`.

## Load the browser extension

### Chromium bases

You can load the `./chrome/` directory as any unpacked extension. 

Then modify the native host manifest (likely at `/etc/chromium/native-messaging-hosts/com.vicinae.vicinae.json`) and change the origin in the `allowed_origins` array to use the local extension ID that was automatically generated upon loading the unpacked extension.

Once that is done, reloading the extension should automatically establish a connection with vicinae if the vicinae server is running.

### Firefox bases

#### Prerequisites

  * install the [web-ext](https://github.com/mozilla/web-ext) CLI tool.
  * register and authenticate a [Mozilla Add-on (AMO) account](https://addons.mozilla.org/en-US/firefox/#login) and generate your API keys in the [Mozilla Add-on Developer Hub](https://addons.mozilla.org/en-US/developers/addon/api/key/)
  * install and configure general prerequisites like git and make

#### Instructions

Copy the right manifest template from `./native-host/` and change the placeholder values.

For firefox, you only need to replace `@NATIVE_HOST_BIN` with the absolute path to the `vicinae-browser-link` executable (e.g: `/usr/local/libexec/vicinae/vicinae-browser-link` or `/usr/local/libexec/vicinae/vicinae-browser-link`). Then copy the manifest at `/usr/lib/mozilla/native-messaging-hosts/com.vicinae.vicinae.json`.

For example:

```
#!/bin/bash

AMO_EMAIL_ADDRESS="<yourAMO@email>"
WEB_EXT_API_KEY="<api cred:JWT issuer>" # user:[num:num] "user:12345678:123"
WEB_EXT_API_SECRET="<api cred:JWT secret>" # string  "123abc789abc............................................345abc90" 
WEB_EXT_CHANNEL="unlisted" # to build and distribute locally as opposed to the store with "listed"

git clone https://github.com/vicinaehq/vicinae
cd ./vicinae/src/browser-extension
sudo make firefox

cat > /lib/mozilla/native-messaging-hosts/com.vicinae.vicinae.json <<EOF
{
  "name": "com.vicinae.vicinae",
  "description": "Vicinae Native Messaging Host",
  "path": "/usr/libexec/vicinae/vicinae-browser-link",
  "type": "stdio",
  "allowed_extensions": [
    "$AMO_EMAIL_ADDRESS"
  ]
}
EOF

cd ./build
unzip firefox-vicinae.zip
cd ./firefox
sed 's|"id": "vicinae@vicinae\.com",|"id": "vicinae@vicinae\.com",\n\t\t"data_collection_permissions": \{\n\t\t\t"required": \["none"\],\n\t\t\t"optional": \[\]\n\t\t}|' -i ./manifest.json
web-ext sign
```
At this point, if you've done everything correctly, the terminal will churn for around 8 minutes while the automated checks run, and maybe a human does a quick approval.
Once that's resolved a .xpi file will have populated in the firefox/web-ext-artifacts/ folder, and you import that into firefox by

    \* opening Firefox and opening the extensions menu, or navigate to 'about:addons'
    \* clicking the gear icon on the top right, selecting 'Install Add-on From File...'
    \* selecting the .xpi file

    And upon restarting Firefox you should be good to go!

# Architecture schema


If that's any helpful, here is a very simple schema of how communication between vicinae and the browser works:

![](./assets/architecture.png)

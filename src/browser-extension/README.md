# Vicinae browser integration

This subdirectory contains:

- native bridge between browser extensions and vicinae daemon
- chromium/firefox browser extension. Currently the same codebase is used for both, but eventually we may have to split them.

For now the extension is not published to the official stores, this is a work in progress.

## Native host manifest

The browser extension uses [Native messaging](https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging) to communicate with the vicinae daemon.

The vicinae server writes the required per-user manifests automatically at startup for every browser it detects, so there is nothing to set up. Packagers can opt out with the `AUTO_INSTALL_BROWSER_MANIFESTS` CMake option.

## Development

### Chromium

Load the `./chrome/` directory as an unpacked extension, then edit `allowed_origins` in the installed manifest (e.g `~/.config/chromium/NativeMessagingHosts/com.vicinae.vicinae.json`) to use the extension ID chromium generated for you. Make the file read-only (`chmod -w`) so vicinae does not rewrite it at startup, then reload the extension.

### Firefox

Firefox only loads signed extensions, so you need the [web-ext](https://github.com/mozilla/web-ext) CLI and [AMO API credentials](https://addons.mozilla.org/en-US/developers/addon/api/key/):

```
cd src/browser-extension
make firefox
cd build/firefox
web-ext sign --channel unlisted
```

The signed `.xpi` lands in `web-ext-artifacts/`; install it via `about:addons` > gear icon > "Install Add-on From File...". If you sign under a custom extension ID, update `allowed_extensions` in the manifest (`~/.mozilla/native-messaging-hosts/com.vicinae.vicinae.json`) the same way as for chromium.

# Architecture schema

If that's any helpful, here is a very simple schema of how communication between vicinae and the browser works:

![](./assets/architecture.png)

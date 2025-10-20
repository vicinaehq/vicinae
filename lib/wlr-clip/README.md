# wlr-clip

A library for monitoring Wayland clipboard events using the [wlr-data-control](https://wayland.app/protocols/wlr-data-control-unstable-v1) protocol.

## How it works

This library is linked into the main vicinae binary. When clipboard monitoring is needed, vicinae spawns itself with the `wlr-clip` argument. The spawned process monitors clipboard events and sends filtered clipboard data back to the parent process via stdout using protobuf.

Runs as a separate process because the wlr-data-control offer receiving can cause issues with Qt's event loop.

## What it does

- Monitors clipboard and primary selection changes on Wayland compositors with wlr-data-control support
- Filters out legacy X11 clipboard types (STRING, UTF8_STRING, etc.)
- Limits image types to avoid duplicates
- Encodes clipboard data as protobuf messages for the parent process

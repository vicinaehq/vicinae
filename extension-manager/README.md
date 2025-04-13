The extension manager is a node process that manages extension bundles and handle everything about them.

It's meant to be spawned by the omnicast daemon directly, which will communicate with it using a well defined message scheme across standard file descriptors.

- Standard output is used for the message scheme
- Standard error is used for debug messages

This package is intended to provide a Raycast compatible entrypoint to the vicinae API (`@vicinae/api`).
It is only meant to be bundled as part of the full javascript bundle and is not published on npm, unlike the vicinae API.

This is used to provide raycast extension compatibility. When a Raycast extension imports something from `@raycast/api`, we route it
to this package instead at runtime.

As of now we mostly re-export stuff that belongs to the `@vicinae/api` package as many signatures are the exact same.

As we continue developping the vicinae SDK, we don't want to limit ourselves to what Raycast offers so using this package as a bridge
solves that problem. No matter what the `@vicinae/api` exports, we can expose a Raycast-compatible form.

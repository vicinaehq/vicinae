# Extension manager bundle lifetime

`ExtensionManager::start()` copies the embedded `:bin/extension-manager` resource to `Omnicast::dataDir() / "extension-manager.js"` and passes that path to Node. Pooled production workers and development workers are created with `new Worker(__filename)`, so they load this same file. The data directory has the same persistent lifetime as the managed Node runtime under `dataDir()/node`.

`vicinae-server-tests` is Linux-only and does not link `ExtensionManager`. Exercise the built application instead (`make dev`, or `build/bin/vicinae server` on Windows via CMake). Run `make format` after code changes and keep formatting edits inside the files this check touches.

Use a disposable non-root profile. `HOME` contains a space so the deployed path does too. Point `VICINAE_NODE_BIN` at an existing Node binary, or reuse an already installed managed runtime under the profile's data directory.

```sh
export HOME="$(mktemp -d)/Vicinae Profile"
unset XDG_DATA_HOME XDG_CONFIG_HOME XDG_STATE_HOME XDG_CACHE_HOME XDG_RUNTIME_DIR
export TMPDIR="$(mktemp -d)"
export VICINAE_NODE_BIN="$(command -v node)"
```

On macOS and Windows the data directory follows `HOME` / `USERPROFILE`. On Linux it follows `XDG_DATA_HOME` when that variable is set, and otherwise `$HOME/.local/share`.

| Platform | Deployed bundle | Previous temporary path |
| --- | --- | --- |
| macOS | `$HOME/.local/share/vicinae/extension-manager.js` | `${TMPDIR:-/tmp}/vicinae/extension-manager.js` |
| Linux | `${XDG_DATA_HOME:-$HOME/.local/share}/vicinae/extension-manager.js` | `${XDG_RUNTIME_DIR:-/tmp}/vicinae/extension-manager.js` |
| Windows | `%LOCALAPPDATA%\vicinae\data\extension-manager.js` | `%TEMP%\vicinae\extension-manager.js` |

The manager PID file stays at `<runtime directory>/extension-manager.pid`.

## Fixed build

1. Delete the profile data directory, then start the server. Startup creates the data directory and deploys the bundle into it.
2. The log line `Started extension manager <path> with <node>` names the data-directory bundle. The Node child is invoked with that same single argument (`ps` on Unix, process command line on Windows). `cmp` the deployed file with `build/extension-runtime.js`, the file embedded as `:bin/extension-manager`.
3. Create a dummy `extension-manager.js` at the previous temporary path, then remove only that dummy. Sockets, PID files, and the rest of the runtime directory stay in place.
4. Launch production extensions until the worker pool refills (each production launch creates a replacement worker from `__filename`). Launch a development extension with `vici develop` in `extra/extension-boilerplate`, including the no-view command and, once enabled, the `desktop-notif-interval` background command. Workers start. The PID in `extension-manager.pid` stays alive. Logs contain no `MODULE_NOT_FOUND`, no `Extension manager crashed`, no `Extension manager is not running` toast, and no `[ExtensionIntervalScheduler] Extension manager not running, deferring`.
5. Stop the server. Overwrite the data-directory bundle with a short stale marker and start again. The deployed file matches `build/extension-runtime.js` again.
6. Stop the server. Write the stale marker again and clear the owner write bit (`chmod u-w` on Unix; read-only attribute on Windows). Start again. Startup restores owner read and write permission, replaces the file, and the contents match the embedded bundle.
7. Clear owner write permission on the data directory and start. `start()` logs `Failed to deploy extension manager bundle` and returns before launching Node, so Node is not started on a missing or stale script. Restore the directory mode afterward.

## Unmodified baseline

On a build that still deploys into the runtime directory: launch one extension, delete only that deployed `extension-manager.js`, then launch extensions until the pool must create a new worker. The manager logs `MODULE_NOT_FOUND` and exits (`Extension manager crashed. Extensions will not work`). Later launches report that the manager is not running, and interval commands are deferred. Remove only the script. Sockets, PID files, and the rest of the runtime directory stay in place.

## Other platforms

Repeat a normal launch and restart on Linux and on Windows when those hosts are available. On Windows, confirm a live manager is killed before the bundle is overwritten (`Killed existing extension manager instance`) and that a read-only stale bundle is replaced. When a platform is unavailable, record that the check was not run.

# Custom frontends: cooperative search and launch

Status: **proposal for review**, not an approved upstream direction.
Scope: **documentation only**. This document ships no backend, CLI, QML, or extension-runtime change.

## Purpose and recommendation

A custom launcher frontend needs three things Vicinae already owns: a ranked catalog of commands and
applications, an indexed file search, and a way to run the selected entrypoint — including extension
commands, whose views are rendered by Vicinae itself.

The recommendation is a **cooperative** boundary:

- the custom frontend (for example a Quickshell shell) asks Vicinae for ranked results and file matches,
  and owns all presentation and interaction;
- Vicinae keeps ownership of the catalog, the ranking, the extension runtime, and every native view;
- when a result needs a real UI, the frontend asks Vicinae to activate it and Vicinae opens its own window.

Search and launch are external; anything that renders is native.

This keeps the integration small for two reasons. The ranking behavior is not trivially portable: it spans
title, unlocalized title, subtitle, alias, and keyword matching, a match-quality gate, frecency, disabled-item
filtering, and alias priority (`RootItemManager::search`,
[`root-item-manager.cpp:162`](../../src/server/src/services/root-item-manager/root-item-manager.cpp)).
And extension views are host-owned objects pushed onto a native view stack
(`ExtUIService`, [`ui-service.hpp`](../../src/server/src/extension/api/ui-service.hpp)), so a frontend
cannot usefully re-implement them from witness data.

The whole integration described in *Available today* works against the current release. *Proposed ranked root
search* adds one read-only query so a frontend does not have to duplicate ranking.

Not in scope: a headless or GUI-free daemon, a second HTTP/WebSocket API, an external extension renderer, or
replacing the frontend. The QML in this document is a documentation snippet, not a tracked file.

## Available today

### Path of a request

An external caller drives the generated Figura client
([`ipc-client.hpp`](../../src/cli/src/ipc-client.hpp), generated from [`figura/ipc.fig`](../../figura/ipc.fig)),
which writes to a local socket served by `IpcCommandServer`/`IpcService`
([`ipc-command-server.cpp`](../../src/server/src/ipc/ipc-command-server.cpp)). Those handlers call services
owned by the running Vicinae process ([`server.cpp`](../../src/server/src/server.cpp)).

The CLI is a separate process that connects to an **already running** server; it does not start one.

### Operations you can rely on today

**`vicinae cmd list --json`** — `Ipc/listCommands`

- Enumerates `RootItemManager::allItems()` and sorts by ID.
- The RPC returns `{"commands": [{"id": ..., "name": ...}]}` (`ListCommandsResponse`); the CLI prints the bare
  array `[{"id": ..., "name": ...}]`. A parser must not assume one wrapper for the other.
- This is a catalog dump, not search: it is ordered by ID, carries no scores, is not filtered by enabled
  state, and includes applications and every other root provider — not only extension commands.
- The JSON is pretty-printed, so parse the whole output as one document, not line by line.
- `id` is a serialized `EntrypointId` in `provider:entrypoint` form
  ([`entrypoint.hpp`](../../src/server/src/common/entrypoint.hpp)).

**`vicinae cmd launch ID [ARGS...]`** — `Ipc/launchCommand`

- Accepts an entrypoint ID as serialized by `cmd list`.
- Validates positional arguments against the manifest: too many arguments, an unknown dropdown value, or a
  missing required argument produce an error message that includes the expected usage.
- Activates through `NavigationController::activateEntrypoint`, which registers the visit and applies the
  entrypoint's primary action.
- The CLI forwards a working directory (defaulting to the current directory) and passes `--query` as fallback
  text. Fallback text is not a generic extension-view search endpoint.

**`vicinae fs query QUERY --json --limit N [--category CATEGORY]`** — `Ipc/fsQuery`

- Delegates to `FileService::queryAsync`; each result carries `path`, `score`, `category`, and an optional
  `mimeType`.
- The CLI rejects queries shorter than three characters.
- Baseline output is pretty-printed: collect the whole stdout and parse it as one document, not line by line.
- `--limit` defaults to 100. `score` is the indexer's rank and is only comparable inside a single result set.

**`Ipc/ping`** reports version and PID. The CLI `ping` prints `Pinged successfully.` — a human message, not
JSON.

### What "a running server" means

`startServer` constructs the `QGuiApplication`, the service registry, navigation and its controllers,
`LauncherWindow`, and onboarding regardless of `--open`; `--open` only decides whether the main window is
shown at startup ([`server.cpp:660`](../../src/server/src/server.cpp)). A hidden Vicinae window is therefore not
a headless server: the shared native navigation stack, views, and toasts exist and can be changed by an
activation.

### Transport

Payloads are a native-endian 32-bit byte length followed by a JSON document, with `Service/method` method
names and a string-valued `error` field. It is not newline-delimited JSON, and it has not been established as
standards-compliant JSON-RPC 2.0.

Socket discovery lives in [`common.cpp:94`](../../src/lib/common/src/common.cpp): `$TMPDIR/vicinae/`
on macOS and `$XDG_RUNTIME_DIR/vicinae/` on Linux, each falling back to `/tmp/vicinae`, with `vicinae.sock`
as the leaf; Windows uses a `vicinae-<user>` name. The comment at the top of `figura/ipc.fig` describes only
the Linux case. Prefer the CLI's own discovery over hard-coded paths.

### Recommendation for the integration

Use **CLI-mediated JSON** for the first Quickshell integration, backed by the existing Figura/local-IPC
implementation. No second listener, persistent stdio bridge, duplicated JavaScript fuzzy matcher,
Quickshell-specific C++ ABI plugin, or private view-host object is needed.

That recommendation is **conditional**: serialize queries — one in flight at a time — until reply routing is
connection-qualified. Overlapping CLI processes share the same server-side routing state, and that is measured,
not hypothesized: two concurrent queries left one process waiting indefinitely (*Concurrency and reply
routing* below). A client that spawns a process per keystroke without serializing will lose replies.

One-shot CLI processes are a deliberate simplicity tradeoff: each query pays process startup, and the
serialization requirement caps how much of that cost can be hidden. Interactive latency has not been measured
here, and nothing below should be read as a production-readiness claim.

## Proposed ranked root search

**Proposed, not implemented.** Current source has no `rootQuery` operation.

The desired behavior already exists. `RootItemManager::search`
([`root-item-manager.cpp:162-202`](../../src/server/src/services/root-item-manager/root-item-manager.cpp),
declared in [`root-item-manager.hpp:340`](../../src/server/src/services/root-item-manager/root-item-manager.hpp))
matches title, unlocalized title, subtitle, alias, and keywords, gates on match quality, adds frecency, skips
disabled entries, supports alias priority, and stable-sorts the output. The native UI reaches it through the
UI-bound `RootSearchModel` ([`root-search-model.cpp`](../../src/server/src/builtins/root/root-search-model.cpp)),
which is the wrong seam for an API: it also owns the update/news/favorites/fallback sections and its own file
search. Do not introduce a second search service.

### Contract

Additive schema in `figura/ipc.fig`:

```text
struct RootQueryParams {
  limit: int;
};

struct RootResult {
  id: string;
  name: string;
  subtitle: string;
};

// Addition to service Ipc:
fn rootQuery(q: string, params: RootQueryParams) => RootResult[];
```

Corresponding CLI surface:

```sh
vicinae cmd query QUERY --limit 20 --json
```

### Semantics

- CLI `--limit` defaults to `20`. The RPC requires an integer limit in `[1, 100]`; outside that range fail with
  `Root query limit must be between 1 and 100`. Validate before searching and truncate only after ranking.
- Map `id` from the serialized `EntrypointId`, `name` from `RootItem::title()`, and `subtitle` from
  `RootItem::subtitle()`. An empty subtitle is allowed. Keep the existing catalog's `id`/`name` terminology;
  subtitle exists to separate similarly named extension commands.
- For an empty `q`, call the manager with `{.includeFavorites = false, .prioritizeAliased = false}`; for a
  non-empty `q`, use the default options. This mirrors the native Results/Suggestions section
  ([`root-search-model.cpp:136-142`](../../src/server/src/builtins/root/root-search-model.cpp)). Do not
  trim or normalize the query differently.
- Disabled entries are excluded, non-empty queries may include favorites, and no match returns `[]`. Return
  the manager's ordering without a client-side re-sort.
- This is not the entire root screen: the separate Favorites section, calculator, links, news, updates, files,
  and fallback sections are not included. Files stay a separate query and result list, and their score is not
  comparable to root ranking.
- Run and project the results synchronously on the server/main thread. `ScoredItem` holds references into the
  index, and `updateIndex()` clears that storage; copy the selected strings into owning DTOs before returning
  and never retain index references across asynchronous work.
- Do not expose raw fuzzy scores, QObject pointers, internal view models, or `icon://` URLs as portable assets.
  `ImageURL::url()` ([`url.cpp:78`](../../src/server/src/ui/image/url.cpp)) builds a Vicinae-specific
  scheme, so the reference example below is text-only rather than pretending those icons resolve.
- Add no sessions, result subscriptions, favorites mutation API, argument-form schema, or extension-render
  protocol to this contract.

### Concurrency and reply routing

These caveats apply to **every** client of the socket, including a shell that merely shells out to the CLI —
not only to a hand-rolled raw-IPC client. The bounded CLI verbs above stay convenient, but each invocation
opens its own connection while the server-side routing state is shared.

- Request IDs are per connection, not global: the generated client starts at 1 (`int m_id = 1` in
  [`glaze-qt.hpp`](../../src/lib/figura/src/codegen/glaze-qt.hpp)), and `IpcTransport::bindReply` records
  `pendingReplies[id] = conn` in one map keyed by that integer alone
  ([`ipc-command-server.hpp`](../../src/server/src/ipc/ipc-command-server.hpp)). When two connections each have
  an unfinished asynchronous request under the same id, the second bind overwrites the first: one reply can be
  written to the wrong peer while the original caller waits for a reply that never arrives.
- This is observed, not theoretical. Against the backend described under *Evidence*, two simultaneous
  `fs query doc --json --limit 5` processes produced one result in ~0.1 s and one process that never returned;
  ten simultaneous processes produced exactly one result and nine hangs, while the same query run sequentially
  returned in ~0.1 s every time and the server stayed healthy afterwards. Cause attribution comes from the
  source above, not from instrumentation, but the symptom matches it exactly: replies are lost, not delayed.
- The generated router returns silently on a malformed outer message or an unknown method, and it ignores
  parameter-deserialization errors by dispatching with default-initialized values
  ([`glaze-qt.hpp`](../../src/lib/figura/src/codegen/glaze-qt.hpp)). Do not expect a "method not found"
  reply from an older server, do not invent a capability handshake, and do not fall back to JavaScript fuzzy
  filtering when a call fails.
- Frame bounds and local socket permissions have not been audited here; verify them before treating the
  internal transport as a supported public API.

These are proposal findings and acceptance gates for a future implementation, not fixes to ship with this
document.

### Illustrative example only

**Illustrative proposed data, not observed output.** Nothing in the current release answers this request.

```json
{"jsonrpc":"2.0","id":1,"method":"Ipc/rootQuery","params":{"q":"example","params":{"limit":20}}}
```

```json
{"jsonrpc":"2.0","id":1,"result":[{"id":"applications:example","name":"Example","subtitle":""}]}
```

### Implementation seam

This document does not edit them, but a future change would touch four places:

- the schema in [`figura/ipc.fig`](../../figura/ipc.fig);
- an `IpcService::rootQuery` handler in the existing IPC server, delegating to `RootItemManager::search`;
- a generated-client wrapper method in [`ipc-client.hpp`](../../src/cli/src/ipc-client.hpp);
- a `query` child of the existing CLI `CommandCommand` in [`cli.cpp`](../../src/cli/src/cli.cpp).

Generation already consumes the schema for the CLI client ([`src/cli/CMakeLists.txt:11`](../../src/cli/CMakeLists.txt));
do not add a handwritten generated header.

### Acceptance criteria for the addition

Implementation criteria for a future PR — not test results for this one:

- an enabled fixture item matches while an otherwise matching disabled item is omitted;
- an alias-prefix match keeps priority over a higher-score non-alias match;
- an empty query omits favorites from the Suggestions result;
- limit 1 returns at most the highest-ranked match;
- limits 0 and 101 fail with the specified error;
- an unrelated query returns `[]`;
- querying does not change the active native view.

### Not provided by this contract

- the Favorites, calculator, links, news, updates, file, and fallback sections of the native root screen;
- raw fuzzy scores, ranking internals, icons, or any `icon://` asset;
- sessions, result subscriptions, favorites/alias mutation, argument-form schemas, or extension rendering;
- connection-qualified reply routing, so concurrent clients are not safe by default today;
- a second transport (HTTP, WebSocket, or persistent stdio) and a GUI-free daemon.

Each exclusion is explained above where it applies.

## Extension and platform boundaries

### Activation

Activate returned root IDs through the existing `cmd launch` path, including application entries, so that the
entrypoint's primary action, argument validation, and visit registration are the same ones native selection
uses. Do not automatically substitute `Ipc/launchApp`: it has different focus, launch, and ranking-feedback
semantics.

Search must not change native selection or navigation. Explicit activation may: `activateEntrypoint` pops the
shared native stack, and when Vicinae was closed and the target is a pushed view rather than the root search
itself, it also shows the native window
([`navigation-controller.cpp:601`](../../src/server/src/navigation-controller.cpp)). There are no isolated
frontend sessions — the native and external frontends share one navigation stack.

### Activation success is not completion

- Missing required arguments produce the existing validation errors; required extension preferences open the
  native setup view (`MissingPreferenceViewHost`).
- A successful response means *accepted*, not loaded, rendered, or completed. `NavigationController::launch`
  reports a stopped extension manager through a native toast
  ([`navigation-controller.cpp:674`](../../src/server/src/navigation-controller.cpp)), and the eventual
  extension load or crash outcome is not returned by this IPC call.
- Do not add retries, and do not treat an accepted activation as completion.

### Why extensions work but external rendering does not

Extension discovery and execution are feasible: providers index manifest-backed commands without rendering
them, and `ExtensionCommandRuntime::load`
([`extension-command-runtime.cpp`](../../src/server/src/extension/extension-command-runtime.cpp)) starts
the runtime, pushes initial native views for view commands, and routes failures into native view and toast
handlers.

External rendering is a different problem. `UI.render`, `UI.pushView`, and `UI.popView` in
[`figura/tsapi.fig`](../../figura/tsapi.fig) are operations for the trusted host and runtime, not a portable
wire format; `ExtUIService` owns the `NavigationController` relationship and the per-view render callbacks.
Lists, grids, forms, details, actions, view lifetimes, images, and native host effects would each need a
separately designed renderer and session contract. Forwarding render JSON to another process is not that
contract. Even no-view commands can need the runtime, preferences, and native host effects.

### Files differ by platform

`FileService` selects `SpotlightFileIndexer` on macOS and `FileIndexer` on Linux
([`file-service.cpp`](../../src/server/src/services/files-service/file-service.cpp)). `FileIndexer`
currently returns an empty vector both when the indexer is not running and when a query fails
([`file-indexer.cpp:252-262`](../../src/server/src/services/files-service/file-indexer/file-indexer.cpp)),
so on Linux an empty file result does **not** prove that indexing is healthy. Do not promise identical file
matching across platforms, and this proposal adds no file-opening operation.

### Trust model

The trust model is one trusted local desktop user: a local socket reachable by that user's processes. It is not
a network boundary and not a sandbox authorization boundary.

## Evaluate from Quickshell

### Existing operations

Runner prerequisites: a running Vicinae on the matching revision, and `vicinae` on `PATH`.

```sh
vicinae version
vicinae ping
vicinae cmd list --json
vicinae fs query BASENAME --json --limit 5
```

Use at least three characters from a file you know is indexed in place of `BASENAME`. Pick a command ID from
**your own** catalog output rather than copying an illustrative ID from this document.

### Proposed operation, not available today

This block is the proposal, not the current release:

```sh
vicinae cmd query QUERY --limit 20 --json
```

### A minimal catalog probe

This is a documentation snippet, not a tracked QML file and not a launcher. It only reads the catalog: it runs
no shell, starts no Vicinae, launches nothing automatically, and does not modify an existing shell
configuration.

The snippet assumes a `vicinae` command on `PATH`, as in the runner block above. Adjust `command[0]` if your
installation exposes the CLI under another name — the macOS app bundle ships it as `vicinae-cli`.

```qml
import Quickshell
import Quickshell.Io

ShellRoot {
  Process {
    id: catalog
    command: ["vicinae", "cmd", "list", "--json"]
    running: true

    stdout: StdioCollector {
      id: catalogOut
      onStreamFinished: {
        try {
          const entries = JSON.parse(catalogOut.text)
          if (!Array.isArray(entries)) {
            console.error(`vicinae catalog: expected a JSON array, got ${typeof entries}`)
            return
          }
          console.log(`vicinae catalog: ${entries.length} entries`)
        } catch (e) {
          console.error(`vicinae catalog: failed to parse JSON (${e})`)
        }
      }
    }

    stderr: StdioCollector {
      id: catalogErr
      onStreamFinished: {
        if (catalogErr.text.length > 0) console.error(`vicinae catalog stderr: ${catalogErr.text}`)
      }
    }

    onExited: {
      const crashed = Number(exitStatus) !== 0
      if (exitCode !== 0 || crashed) {
        console.error(`vicinae catalog: failed (code ${exitCode}, status ${exitStatus})`)
      }
    }
  }
}
```

Notes on the snippet: stdout is collected whole and parsed once at `streamFinished`; the array shape is checked
before use; JSON and shape failures are logged instead of being replaced with `[]`; stderr is separate.

The `exited(exitCode, exitStatus)` signal is handled so a nonzero **or crashed** child is reported, and a
printed row count is never taken as process success. `exitStatus` carries Qt's `QProcess::ExitStatus`
(`NormalExit`, `CrashExit`); Quickshell v0.3.0 does not expose that enum under a QML name — its own `Process`
class declares no such `Q_ENUM` and the type reference documents the parameter as untyped — so the check
compares against the normal-exit value and logs both raw numbers, keeping any crash visible even when its exit
code alone would look clean.

References (Quickshell v0.3.0, inspected):

- https://quickshell.org/docs/v0.3.0/types/Quickshell/ShellRoot/
- https://quickshell.org/docs/v0.3.0/types/Quickshell.Io/Process/
- https://quickshell.org/docs/v0.3.0/types/Quickshell.Io/StdioCollector/
- https://quickshell.org/docs/v0.3.0/guide/introduction/ — `qs --path` / `-p` for an isolated configuration

### For a subsequent interactive client

Once a query operation exists, keep **at most one query in flight** and debounce on the presentation side
(~100 ms). A monotonically increasing query generation tells you which correctly routed reply is stale; it
cannot detect a reply delivered to the wrong process, so generation bookkeeping is not a substitute for
serializing calls. Ending a one-shot CLI process or stopping a watcher cancels local work only; it does not
cancel work inside Vicinae. That client is not implemented here.

### Handoff checks on Linux

**Not executed by this proposal's author.** Prerequisites: the collaborator's normal graphical session, a
running matching Vicinae build, a functioning configured Linux file index for the positive file test, and
Quickshell v0.3.0. From the checkout, `export PATH="$PWD/build/bin:$PATH"` if using a local build; otherwise use
the installed `vicinae`.

1. Save the snippet above as `shell.qml` in a disposable directory and run `qs --path /absolute/path/to/that/directory`.
   Expect a logged entry count from valid JSON, no failure line, and a clean child exit, no native window
   opened by the example, and no parse or process errors in the Quickshell logs. Stop the probe normally; leave
   the existing shell alone.
2. Run `vicinae fs query BASENAME --json --limit 5` with at least three characters from a known indexed,
   non-sensitive file. Expect that path in the results; the nonsensical-query case should return `[]`. If no
   known indexed file matches, record indexing setup as unverified instead of declaring the API healthy.
3. Pick an already trusted, installed view command without required arguments from `vicinae cmd list --json`
   and run `vicinae cmd launch 'COPIED_ID'`. Expect Vicinae to open or own the view, or its required-preference
   screen. This deliberately changes native UI state, so it stays a collaborator-run check.
4. Launch several `vicinae fs query BASENAME --json --limit 5` processes at once and require that every one
   returns its own result set and exits, with none hanging. This already fails on the authoring host, so treat
   it as confirmation of platform-specific behavior rather than a discovery; a single-process run proves
   nothing here.
5. Record the Vicinae revision, Quickshell/Qt versions, session or compositor, commands, outputs, and whether
   the native view handoff behaved as documented.

Until they are run, all five are **not executed**.

## Evidence and review questions

### Observed on macOS

Inspected checkout: `4e7af129ca89fbb6574c8409a4d32ad468c7161d` (clean `main`). The installed CLI reported
`v0.29.2`, commit `4e7af129c`, matching the checkout prefix, built as a release DMG. These read-only probes
were actually run against the local backend:

| Input | Observed |
|---|---|
| `version` | `Version v0.29.2 (commit 4e7af129c)`, with a release DMG build line |
| `ping` | exit 0, `Pinged successfully.` |
| `cmd list --json` | exit 0, parsed JSON array of 286 entries, all with string `id` and `name` |
| `fs query vicinae-proposal-no-match-8b239c41 --json --limit 5` | exit 0, `[]` |
| `fs query ab --json` | exit 1, `Query should be at least 3 characters long` plus the help hint |
| `fs query --help` | positional query, `--limit`, `--category`, `--json` confirmed |
| two simultaneous `fs query doc --json --limit 5` | one returned in ~0.1 s, one never returned |
| ten simultaneous `fs query doc --json --limit 5` | exactly one returned, nine hung |
| the same query run sequentially, three times | ~0.12 s, ~0.10 s, ~0.08 s; a later run ~0.09 s |

The entry count is local evidence, not a contract constant. The single-process probes cover catalog
enumeration and the negative and argument-error paths only: no activation, no ranked query, and no positive
index was run at this revision. The concurrency rows are separate — see *Concurrency and reply routing* — and
say nothing about single-client latency beyond the ~0.1 s above.

### Not executed

No new API, positive indexing scenario, command activation, extension rendering, Quickshell runtime, or Linux
file index was exercised. `qs` was not present in `PATH` on the authoring host. Source inspection and a
negative file query are not evidence for any of those, and macOS results do not establish Linux behavior.

### Review questions

1. Is the cooperative boundary — external search and launch, native UI — the useful shape for a custom
   frontend, or does a real frontend need more than ranked roots and file matches?
2. Do the proposed `id` / `name` / `subtitle` fields plus existing `cmd launch` activation suffice for a first
   custom launcher?
3. Does the maintainer accept CLI-first access to this additive Figura operation, with connection-qualified
   reply routing treated as a prerequisite for any client that queries concurrently?

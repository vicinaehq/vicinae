# Server tests

Configure with `BUILD_TESTS=ON` (the development presets enable it), build, then run
`make test`. CTest runs the available library suites and server tests on the current
platform. The Linux and macOS CI jobs use the same command.

To run just the server tests:

```sh
ctest --test-dir build --output-on-failure -R '^server\.'
```

Tests are organized by feature and compiled into `vicinae-server-tests`.
The current `*-macos` sources and executable are enabled only on macOS. They use a
`QGuiApplication` for native event handling, temporary directories, a private
pasteboard, and the synthetic video under `fixtures/`. `macos-fixtures.hpp` contains
the shared fixture helpers. `main.cpp` handles Catch2/Qt startup without initializing
Qt during test discovery.

Add test sources and the production sources they need to this directory's
`CMakeLists.txt`. Catch2 discovers individual server cases automatically with a
`server.` name prefix. Existing library binaries register with CTest beside their
own targets.

Prefer tests for observable behavior and regressions. Keep unrelated services and
the global service registry out of the setup. Use a fake provider when a service's
asynchronous behavior needs one; don't introduce mocks for every dependency.

The automated suite must not depend on the user's Desktop, clipboard, permissions,
or open applications. Live Spotlight discovery, UI interaction, and pasting into
another application are explicit manual checks, not part of the default suite.

# Server tests

Configure with `BUILD_TESTS=ON` (the development presets enable it), build, then run
`make test`. CTest runs the available library suites and server tests on the current
platform. The Linux and macOS CI jobs use the same command.

To select part of the suite:

```sh
ctest --test-dir build --output-on-failure -R '^server\.'
ctest --test-dir build --output-on-failure -L unit
ctest --test-dir build --output-on-failure -L native
```

- `services/`: portable behavior tests in `vicinae-server-tests`, using a
  `QCoreApplication`. These exercise production code without launching the server.
- `native/<platform>/`: platform integration tests in `vicinae-server-native-tests`,
  currently macOS. They use a `QGuiApplication` for native event handling, temporary
  directories, and the synthetic video under `fixtures/`.
- `main.cpp`: shared Catch2/Qt startup. Test discovery does not initialize Qt.

Add test sources and the production sources they need to this directory's
`CMakeLists.txt`. Catch2 discovers individual server cases automatically with a
`server.` name prefix; CTest labels distinguish `unit` and `native` tests. Existing library binaries register
with CTest beside their own targets.

Prefer tests for observable behavior and regressions. Keep unrelated services and
the global service registry out of the setup. Use a fake provider when a service's
asynchronous behavior needs one; don't introduce mocks for every dependency.

The automated suite must not depend on the user's Desktop, clipboard, permissions,
or open applications. Live Spotlight discovery, UI interaction, and pasting into
another application are explicit manual checks, not part of the default suite.

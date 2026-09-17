# Launcher viewport regression tests

This standalone Qt Quick Test target exercises the production page, scrolling,
and footer-inset components, plus forms, text previews, and script output, without
starting the launcher's services. The window fixture supplies only the geometry
normally exposed by `LauncherWindow`; the theme fixture supplies fonts and colors.

With the project's Qt installation on `CMAKE_PREFIX_PATH`:

```sh
cmake -S src/server/tests/quick -B build/viewport-tests
cmake --build build/viewport-tests
ctest --test-dir build/viewport-tests --output-on-failure
```

The tests cover both the overlay header and the zero-inset layout, independent
panes, fixed toolbars, nested scrollers, selection bounds, navigation history,
form focus, highlighted text, and script output. QML warnings fail the tests.

C++ library implementing various [Freedesktop](https://specifications.freedesktop.org/) specifications. 

This is in very early stage and is exclusively relied on by the Vicinae launcher. Breaking changes WILL occur without prior notice.

Use in your own project at your own risk.

# Goals

The goal of this library is to provide high-level interfaces to deal with various specifications, such as the desktop entry one.

For now, this library (roughly) implements:
- [mime-apps](https://specifications.freedesktop.org/mime-apps-spec/latest/)~
- [desktop-entry](https://specifications.freedesktop.org/desktop-entry-spec/latest/)

# Build and install

## Build

To build the library:

```bash
make static
# or, if you want a dynamic library:
make shared
```

## Install

```bash
make install
```

## Test

To run the test suite:

```bash
make test
```

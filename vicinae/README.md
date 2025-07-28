# Building

## Prerequisites

- `g++`
- `cmake`
- A linux kernel with `inotify` support
- `qt6`
- `qt-layer-shell` (optional, if you are running a wlroots-based compositor)

## Configuration

Before building you might want to configure the build with `cmake`.
To print the list of available options run:

```bash
cat CMakeLists.txt | grep "option("
```

## Instructions

```bash
cmake -B build
cmake --build build
./build/omnicast daemon

# to open the 
./build/omnicast launcher
```


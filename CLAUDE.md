# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Vicinae (pronounced "vih-SIN-ay") is a high-performance, native launcher for Linux built with C++ and Qt6. It features a unique server-side React/TypeScript extension system (no browser or Electron involved). Inspired by Raycast, it provides a mostly compatible extension API for reusing Raycast extensions with minimal modification.

**Key Technologies:**
- C++23 with Qt6 for the native launcher
- Protocol Buffers for all IPC communication
- React 19 with custom reconciler for server-side rendering
- Node.js worker threads for extension execution
- CMake build system with Ninja generator

## Build Commands

### Standard Builds
```bash
make release          # Release build with optimization
make debug            # Debug build with symbols
make test             # Build and run C++ test suite
```

### Specialized Builds
```bash
make portable         # Build with static libs for cross-distro portability
make appimage         # Create AppImage distribution
make no-ts-ext        # Build without TypeScript extension support
make host-optimized   # Release build with -march=native
```

### Development
```bash
make dev              # Alias for debug build
make extdev           # Debug build + start extension dev server
make clean            # Clean all build artifacts
make format           # Format C++ code with clang-format
```

### Running Tests
The test suite is built with `make test`, which compiles and immediately runs `./build/tests/all_tests`. To run tests individually:
```bash
cmake -G Ninja -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
./build/tests/all_tests
```

## Architecture Overview

### Core Application (C++/Qt6)

**Primary Source:** `vicinae/src/`

The Qt6-based launcher handles all UI rendering, system integration, and service management.

**Key Components:**
- `vicinae/src/main.cpp` - Entry point, delegates to CLI handler
- `vicinae/src/ui/` - 65+ Qt widget components for the launcher UI
- `vicinae/src/services/` - 20+ system services (clipboard, OAuth, file search, window management, etc.)
- `vicinae/src/extension/` - Extension system integration
  - `manager/` - ExtensionManager spawns and manages Node.js extension processes
  - `extension-command-runtime.hpp` - Routes extension requests to appropriate handlers
  - `requests/` - Request routers (UIRequestRouter, AppRequestRouter, ClipboardRequestRouter, etc.)
- `vicinae/src/actions/` - Action handlers (app launching, calculator, color picker, file operations, etc.)
- `vicinae/src/root-search/` - Main launcher search and fuzzy matching
- `vicinae/src/navigation-controller.hpp` - View stack and navigation management
- `vicinae/src/service-registry.cpp` - Central service discovery and registration

### TypeScript Extension System

**Primary Source:** `typescript/`

A Node.js-based system that executes React/TypeScript extensions in worker threads and communicates with Qt via protobuf.

**Structure:**
- `typescript/api/` - The `@vicinae/api` npm package
  - TypeScript SDK for extension developers
  - React components: List, Grid, Form, Detail, Alert, Toast
  - APIs: Clipboard, Application, FileSearch, LocalStorage, OAuth, WindowManagement
  - `bus.ts` - Protobuf-based IPC communication layer
- `typescript/extension-manager/` - Node.js process manager
  - `src/index.ts` - Main manager process, spawns worker threads
  - `src/worker.tsx` - Worker thread that loads and executes extensions
  - `src/reconciler.ts` - Custom React reconciler (renders to Qt instead of DOM)
- `typescript/raycast-api-compat/` - Compatibility shim for Raycast extensions

### Protocol Buffers (IPC Layer)

**Primary Source:** `proto/`

All communication between Qt and Node.js uses protobuf with length-prefixed binary framing.

**Key Proto Files:**
- `ipc.proto` - Low-level message envelope
- `extension.proto` - Extension request/response/event types
- `manager.proto` - Extension manager commands (load/unload)
- `ui.proto` - UI rendering protocol (163 lines)
- `application.proto`, `clipboard.proto`, `storage.proto`, `file-search.proto`, `oauth.proto`, `wm.proto` - Domain-specific APIs

**Protobuf Code Generation:**

For TypeScript:
```bash
cd typescript/api
npm run protogen
```

For C++: CMake handles generation automatically during build.

## Communication Flow

### Extension Lifecycle

1. **Load**: User triggers extension command → Qt ExtensionManager receives command → spawns Node.js process via QProcess → Manager creates worker thread → Worker imports extension module
2. **Render**: Extension renders React component → Custom reconciler builds Instance tree → Serialized to JSON → Sent via protobuf to Qt → UIRequestRouter creates Qt widgets → Display in launcher
3. **Interact**: User input in Qt UI → Event sent to extension → Component handler executes → Re-render cycle
4. **API Calls**: Extension calls API (e.g., `Clipboard.readText()`) → `bus.turboRequest()` sends protobuf message → Qt request router handles → Response sent back → Promise resolves in TypeScript
5. **Unload**: Kill worker thread → Cleanup support directory

### IPC Message Flow

```
[Extension Component]
    ↓ (React reconciler)
[Instance Tree JSON]
    ↓ (bus.ts protobuf serialization)
[ExtensionCommandRuntime in Qt]
    ↓ (routes to appropriate handler)
[Request Router] (UIRequestRouter, AppRequestRouter, etc.)
    ↓ (executes Qt/C++ operation)
[Protobuf Response]
    ↓
[Extension receives result]
```

## TypeScript Extension Development

### Building Extensions

Extensions are TypeScript/React modules that export a default component or function. The SDK is in `typescript/api/`.

**Build TypeScript API:**
```bash
cd typescript/api
npm install
npm run build
```

**Build Extension Manager:**
```bash
cd typescript/extension-manager
npm install
npm run build
```

**Development Mode:**
```bash
make extdev  # Starts extension dev server with hot reload
```

### Extension Structure

Extensions use standard React patterns. Example:

```typescript
import { List, showToast } from "@vicinae/api";

export default function Command() {
  const [items, setItems] = useState([]);

  useEffect(() => {
    fetchData().then(setItems);
  }, []);

  return (
    <List>
      {items.map(item => (
        <List.Item
          key={item.id}
          title={item.title}
          actions={
            <ActionPanel>
              <Action title="Copy" onAction={() => Clipboard.copy(item.text)} />
            </ActionPanel>
          }
        />
      ))}
    </List>
  );
}
```

Components are rendered server-side (Node.js) and translated to Qt widgets. No DOM or browser APIs are available.

## Code Organization

### Directory Structure

```
vicinae/
├── vicinae/            # C++ application source
│   ├── src/
│   │   ├── ui/         # Qt widgets and UI components
│   │   ├── services/   # System service implementations
│   │   ├── extension/  # Extension system integration
│   │   ├── actions/    # Command action handlers
│   │   └── extensions/ # Built-in extensions (calculator, clipboard, raycast, etc.)
│   └── include/        # C++ headers
├── typescript/         # TypeScript extension system
│   ├── api/            # @vicinae/api SDK for extension developers
│   ├── extension-manager/  # Node.js worker process manager
│   └── raycast-api-compat/ # Raycast compatibility layer
├── proto/              # Protocol buffer definitions
├── lib/                # Vendored libraries (xdgpp)
├── cmake/              # CMake modules and utilities
├── scripts/            # Build and packaging scripts
├── tests/              # C++ test suite
├── extra/              # Assets (themes, desktop files, icons)
└── nix/                # Nix package definitions
```

### Built-in Extensions

Located in `vicinae/src/extensions/`:
- `calculator/` - Multi-backend calculator (Qalculate!, bc, etc.)
- `clipboard/` - Clipboard history
- `developer/` - Dev tools (color picker, hash generators, UUID)
- `file/` - File browser and search
- `raycast/` - Raycast extension store integration
- `theme/` - Theme selection and management
- `wm/` - Window manager integration

## Important Implementation Details

### Request Router Pattern

Each API domain has a dedicated router class in `vicinae/src/extension/requests/`:

- **UIRequestRouter** - Handles UI rendering (deserializes JSON component trees, creates Qt widgets)
- **AppRequestRouter** - Application launching and queries
- **ClipboardRequestRouter** - Clipboard operations
- **StorageRequestRouter** - Persistent key-value storage
- **FileSearchRequestRouter** - File searching with fuzzy matching
- **WindowManagementRouter** - Window focus, bounds, workspace queries

To add a new API: Define protobuf messages → Create request router → Register in ExtensionCommandRuntime.

### Custom React Reconciler

`typescript/extension-manager/src/reconciler.ts` implements a custom React fiber reconciler:

- **Not DOM-based**: Creates plain "Instance" objects instead of DOM nodes
- **Tree serialization**: Instance tree serialized to JSON and sent to Qt
- **Qt widget mapping**: Qt dynamically creates widgets based on component types
- **State management**: React state updates trigger re-render → diff → send updates to Qt

### Service Registry

`vicinae/src/service-registry.cpp` provides a central service locator. Services are registered at startup and accessible throughout the application. Extensions query services via request routers.

### XDG Base Directory

Vicinae follows XDG standards for config and data directories:
- Config: `~/.config/vicinae/` or `$XDG_CONFIG_HOME/vicinae/`
- Data: `~/.local/share/vicinae/` or `$XDG_DATA_HOME/vicinae/`
- Cache: `~/.cache/vicinae/` or `$XDG_CACHE_HOME/vicinae/`

Implemented via vendored `lib/xdgpp` library.

## Dependencies

### System Libraries (Can Use System or Build from Source)
- **Qt6** (Core, Widgets, Network, DBus) - Required
- **OpenSSL** - Required for network security
- **Protocol Buffers** (libprotobuf, protoc) - IPC serialization
- **Abseil** (libabsl) - C++ utilities, protobuf dependency
- **cmark-gfm** - GitHub-flavored Markdown rendering
- **minizip** - ZIP file handling (for Raycast extensions)
- **layer-shell-qt** - Wayland layer shell support (optional)
- **qt-keychain** - Secure credential storage
- **rapidfuzz-cpp** - Fuzzy matching for search
- **libqalculate** - Qalculate! calculator backend (optional)

### Build Tools
- CMake 3.16+
- Ninja (recommended) or Make
- C++23-capable compiler (GCC 13+, Clang 16+)
- Node.js 18+ (for TypeScript extensions)
- protoc (Protocol Buffers compiler)

### CMake Options

Control which dependencies are built from source vs system libraries:
- `USE_SYSTEM_PROTOBUF` (default: ON)
- `USE_SYSTEM_ABSEIL` (default: ON)
- `USE_SYSTEM_CMARK_GFM` (default: ON)
- `USE_SYSTEM_MINIZIP` (default: ON)
- `USE_SYSTEM_LAYER_SHELL` (default: ON)
- `USE_SYSTEM_QT_KEYCHAIN` (default: ON)
- `USE_SYSTEM_RAPIDFUZZ` (default: ON)

For portable builds, set these to OFF to statically link bundled versions.

### Performance Options
- `LTO` - Enable Link Time Optimization (slower build, better performance)
- `IGNORE_CCACHE` - Disable ccache even if installed
- `USE_PRECOMPILED_HEADERS` (default: ON) - Speed up compilation

### Platform Options
- `WAYLAND_LAYER_SHELL` (default: ON) - Layer shell protocol support
- `WLR_DATA_CONTROL` (default: ON) - Wayland clipboard server
- `TYPESCRIPT_EXTENSIONS` (default: ON) - Enable React/TS extensions
- `LIBQALCULATE_BACKEND` (default: ON) - Qalculate! calculator support

## Testing

### C++ Tests

Located in `tests/`. Built with CMake when `BUILD_TESTS=ON`.

```bash
make test  # Builds and runs all tests
```

Individual test files are in `tests/` and test the core C++ functionality (not extensions).

### Extension Testing

Extensions are tested manually via:
```bash
make extdev  # Launch dev mode with hot reload
```

Extension API uses Zod for runtime schema validation in development mode.

## Working with Protobuf

### Modifying Proto Definitions

1. Edit `.proto` files in `proto/`
2. Regenerate C++ code: CMake handles automatically on next build
3. Regenerate TypeScript code:
   ```bash
   cd typescript/api && npm run protogen
   cd typescript/extension-manager && npm run protogen
   ```
4. Update request routers in `vicinae/src/extension/requests/` if message structure changed
5. Update TypeScript API in `typescript/api/src/api/` if new APIs added

### Protobuf Message Framing

All IPC uses length-prefixed binary framing:
- uint32 (big-endian) message length
- Protobuf-encoded message bytes

Implemented in C++ Bus class and TypeScript bus.ts.

## Debugging

### C++ Debugging

Build in debug mode:
```bash
make debug
```

Run under gdb:
```bash
gdb ./build/vicinae/vicinae
```

Debug symbols are preserved in debug builds. Use `NOSTRIP=ON` to keep symbols in release builds.

### Extension Debugging

Extensions run in Node.js worker threads. Debug output:
```bash
make extdev  # Dev mode prints extension console.log output
```

Check extension manager process:
```bash
ps aux | grep extension-manager
```

Extension errors are caught by ErrorBoundary and sent back to Qt as crash events.

### Logging

Qt application logs to stdout/stderr. Extension manager also logs to stdout.

## Packaging

### AppImage

```bash
make appimage  # Creates AppImage in build/
```

Uses `scripts/mkappimage.sh` to bundle dependencies and create portable binary.

### System Install

```bash
cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/usr -B build
cmake --build build
sudo cmake --install build
```

Installs:
- Binary: `/usr/bin/vicinae`
- Themes: `/usr/share/vicinae/themes/`
- Desktop files: `/usr/share/applications/`
- Icon: `/usr/share/icons/hicolor/512x512/apps/`
- Systemd unit: `/usr/lib/systemd/user/vicinae.service`

## Version Management

Versions are managed via git tags and CMake:

```bash
make bump-patch  # Increment patch version
make bump-minor  # Increment minor version
```

Scripts in `scripts/` handle version bumping and manifest updates.

## Notes on Breaking Changes

Until version 1.0, expect breaking changes between releases. Configuration may need to be reset between versions. Watch changelogs for migration notes.

## Raycast Compatibility

The Raycast compatibility layer (`vicinae/src/extensions/raycast/` and `typescript/raycast-api-compat/`) allows loading Raycast extensions. Many Raycast APIs are implemented, but some may be missing or have Linux-specific limitations. Extensions are installed from the official Raycast store via one-click install.

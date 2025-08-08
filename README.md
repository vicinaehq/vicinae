<p align="center">
  <img
    width="500"
    src=".github/assets/vicinae-banner.png"
    alt="Vicinae text logo"
  />
</p>

**Vicinae** (pronounced _"vih-SIN-ay"_) is a high-performance, native launcher for Linux — built with C++ and Qt.

It includes a set of built-in modules, and extensions can be developed quickly using fully server-side React/TypeScript — with no browser or Electron involved.

Inspired by the popular [Raycast launcher](https://www.raycast.com/), Vicinae provides a mostly compatible extension API, allowing reuse of many existing Raycast extensions with minimal modification.

Vicinae is designed for developers and power users who want fast, keyboard-first access to common system actions — without unnecessary overhead.

---

## ✏️ Features

> ⚠️ **Note:** Some features may vary depending on your desktop environment.  
> If something isn’t supported yet, contributions are fully welcome — many integrations are beginner-friendly, and helpful guides are available in the [docs](https://docs.vicinae.com).

Vicinae currently runs best on **wlroots-based compositors**, such as **Hyprland** and **Sway**.

- Start and retrieve information about installed applications
- File indexing with full-text search across millions of files — available via the file search module or directly from root search
- Smart emoji picker with support for custom indexing keywords
- Calculator module with unit and currency conversion, plus auto-updating history
- Clipboard history tracker with full-text search across all copied content
- Dynamic links — quickly create shortcuts to open anything
- Direct window manager integration (e.g. copy content directly to the focused window)
- Built-in theming system with light and dark palettes  
  → Custom themes can be added via config (see docs)
- Raycast compatibility module  
  → Includes access to the official Raycast extension store, with one-click installs directly from within the launcher  
  → Many extensions may not work yet due to missing APIs or general Linux incompatibilities (improvements in progress)

---

## 🔽 Installation

### Runtime dependencies

These must be installed regardless of your installation method:

- `qtbase`
- `libQt6Svg` (should be separate from `qtbase`)
- `libcmark-gfm`
- `libprotobuf`
- `libqalculate`
- `libqtkeychain`
- `libminizip`
- `libLayerShellQtInterface` *(optional — only if your compositor supports the `wlr-layer-shell` protocol)*
- `nodejs >= 18` *(optional — required only if you want to run or develop third-party extensions using React/TypeScript. `npm` is **not** required at runtime.)*

---

### Install from repository

Vicinae is a new project and is not yet packaged for most distributions.  
As packaging efforts progress, supported distros will be listed here.

---

### Install from latest release

You can fetch the latest release archive, which contains:
- The compiled Vicinae binary
- A `.desktop` file (required for URL scheme support)
- A few optional themes

---

### 🛠️ Build from source

#### Build requirements

> ⚠️ **Note:** GCC 15 has a [known bug with `std::expected`](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=119714), which is used in the codebase.  
> If using GCC, make sure you're running an older or patched version.

- A C++23-capable compiler (GCC is recommended; Clang should also work)
- `cmake`
- `RapidFuzz`
- `libprotoc` — the Protocol Buffers compiler (different from the runtime `libprotobuf`)
- `npm` (any version bundled with Node.js ≥ 18 should work)

#### Build

```sh
cmake -G Ninja -B build
cmake --build build
sudo cmake --install build
```

## Usage

Run `vicinae server` to start the Vicinae daemon.

Then, you can toggle the window by calling:

```sh
vicinae
```

Typically, you'd run `vicinae server` once when starting a new desktop session, and bind your preferred keyboard shortcut to `vicinae`.

> 🧠 On first launch, Vicinae will begin a full file indexing job in the background.
> This may keep your CPU busy for a few minutes, depending on how many files are in your home directory.

## 📚 Documentation

For more details on configuration, extension development, and contributing, visit [https://docs.vicinae.com](docs.vicinae.com).

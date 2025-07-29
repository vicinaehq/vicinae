<p align="center">
  <img
    width="600"
    src=".github/assets/vicinae-banner.png"
    alt="Vicinae text logo"
  />
</p>

**Vicinae** (pronounced _"vih-SIN-ay"_) is a high-performance, native launcher for Linux — built with C++ and Qt.

It includes a growing set of built-in modules, and extensions can be developed quickly using fully server-side React/TypeScript — with no browser or Electron involved.

Inspired by **Raycast**, Vicinae provides a mostly compatible extension API, allowing reuse of many existing Raycast extensions with minimal modification.

Vicinae is designed for developers and power users who want fast, keyboard-first access to common system actions — without unnecessary overhead.

---

## Features

> ⚠️ **Note:** Some features may vary depending on your desktop environment.  
> If something isn’t supported yet, contributions are fully welcome — many integrations are beginner-friendly, and helpful guides are available in the docs.

Vicinae currently runs best on **wlroots-based compositors**, such as **Hyprland** and **Sway**.

- Start and retrieve information about installed applications
- File indexing with full-text search across millions of files — available via the file search module or directly from root search
- Smart emoji picker with support for custom indexing keywords
- Calculator module with unit and currency conversion, plus auto-updating history
- Clipboard history tracker with full-text search over all copied content
- Dynamic links — quickly create shortcuts to open anything
- Direct window manager integration (e.g. copy content directly to the focused window)
- Built-in theming system with light and dark palettes  
  → Custom themes can be added via config (see docs)
- Raycast compatibility module  
  → Includes access to the official Raycast extension store, with one-click installs directly from within the launcher  
  → Many extensions may not work yet due to missing APIs or general Linux incompatibilities (improvements in progress)


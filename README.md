<p align="center">
  <img
    width="500"
    src=".github/assets/vicinae-banner.png"
    alt="Vicinae text logo"
  />
</p>

[![Discord](https://discord.com/api/guilds/1407488619765174273/widget.png?style=shield)](https://discord.gg/rP4ecD42p7)

**Vicinae** (pronounced _"vih-SIN-ay"_) is a high-performance, native launcher for Linux — built with C++ and Qt.

It includes a set of built-in modules, and extensions can be developed quickly using fully server-side React/TypeScript — with no browser or Electron involved.

Inspired by the popular [Raycast launcher](https://www.raycast.com/), Vicinae provides a mostly compatible extension API, allowing reuse of many existing Raycast extensions with minimal modification.

Vicinae is designed for developers and power users who want fast, keyboard-first access to common system actions — without unnecessary overhead.

<p align="center">
  <img
    src=".github/assets/demo.gif"
    alt="Vicinae text logo"
  />
</p>

---

## ✏️ Features

> ⚠️ **Note:** Some features may vary depending on your desktop environment.  
> If something isn’t supported yet, contributions are fully welcome — many integrations are beginner-friendly, and helpful guides are available in the [docs](https://docs.vicinae.com).

Vicinae currently runs best on **wlroots-based compositors**, such as **Hyprland** and **Sway**.

- Start and retrieve information about installed applications
- File indexing with full-text search across millions of files — available via the file search module or directly from root search
- Smart emoji picker with support for custom indexing keywords
- Calculator module with unit and currency conversion, plus auto-updating history
- Encrypted clipboard history tracker with full-text search across all copied content
- Shortcuts — quickly open anything using dynamic links
- Direct window manager integration (e.g. copy content directly to the focused window)
- Built-in theming system with light and dark palettes  
  → Custom themes can be added via config (see docs)
- Raycast compatibility module  
  → Includes access to the official Raycast extension store, with one-click installs directly from within the launcher  
  → Many extensions may not work yet due to missing APIs or general Linux incompatibilities (improvements in progress)

---

## 📚 Documentation

For more details on installation, usage, configuration, extension development, and contributing, visit [docs.vicinae.com](https://docs.vicinae.com).

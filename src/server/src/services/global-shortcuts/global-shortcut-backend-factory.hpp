#pragma once
#include <memory>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

/**
 * Builds the best global shortcut backend for the current platform, or the dummy fallback (which
 * reports `isSupported() == false`) when none is viable. X11 (XGrabKey) / macOS / Windows backends
 * slot in here; Wayland is intentionally unsupported for now.
 */
std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend();

#pragma once
#include "services/window-material/window-material-backend.hpp"

// Defined in qml/macos-chrome-attached.mm. Installs (enabled) or removes a
// behind-window NSVisualEffectView used for native popover/menu vibrancy.
void macosApplyWindowVibrancy(QWindow *window, bool enabled, int cornerRadius);

class MacOSWindowMaterialBackend : public WindowMaterialBackend {
public:
  bool isSupported() const override { return true; }

  bool apply(QWindow *win, const Params &params) override {
    macosApplyWindowVibrancy(win, true, params.radius);
    return true;
  }

  bool clear(QWindow *win) override {
    macosApplyWindowVibrancy(win, false, 0);
    return true;
  }
};

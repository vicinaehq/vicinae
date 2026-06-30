#pragma once
#include <qwindow.h>
#include "common/types.hpp"
#include "services/window-material/null-window-material-backend.hpp"
#include "services/window-material/window-material-backend.hpp"
#ifdef Q_OS_LINUX
#include "internal/wayland/globals.hpp"
#include "services/window-material/ext-background-effect-v1-manager.hpp"
#include "services/window-material/kde-background-effect-manager.hpp"
#endif

class WindowMaterialManager : NonCopyable {
public:
  WindowMaterialManager() : m_backend(createBackend()) {}

  /**
   * Apply the platform window material (Wayland blur / macOS vibrancy) behind the window.
   */
  bool apply(QWindow *win, const WindowMaterialBackend::Params &params) {
    return m_backend->apply(win, params);
  }

  bool clear(QWindow *win) { return m_backend->clear(win); }

  bool isSupported() const { return m_backend->isSupported(); }

private:
  static std::unique_ptr<WindowMaterialBackend> createBackend() {
#ifdef Q_OS_LINUX
    if (auto manager = Wayland::Globals::extBackgroundEffectManager()) {
      return std::make_unique<ExtBackgroundEffectV1Manager>(manager);
    }
    if (auto blur = Wayland::Globals::kwinBlur()) {
      return std::make_unique<KDE::BackgroundEffectManager>(blur);
    }
#endif
    return std::make_unique<NullWindowMaterialBackend>();
  }

  std::unique_ptr<WindowMaterialBackend> m_backend;
};

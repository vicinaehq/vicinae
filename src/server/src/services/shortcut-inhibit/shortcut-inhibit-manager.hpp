#pragma once
#include <memory>
#include <qwindow.h>
#include "common/types.hpp"
#include "services/shortcut-inhibit/abstract-shortcut-inhibit-manager.hpp"
#ifdef Q_OS_LINUX
#include "internal/wayland/globals.hpp"
#include "services/shortcut-inhibit/wayland-shortcut-inhibit-manager.hpp"
#endif

class ShortcutInhibitManager : NonCopyable {
public:
  ShortcutInhibitManager() : m_manager(createManager()) {}

  bool isSupported() const { return m_manager->isSupported(); }
  bool inhibit(QWindow *win) { return m_manager->inhibit(win); }
  bool release(QWindow *win) { return m_manager->release(win); }

private:
  static std::unique_ptr<AbstractShortcutInhibitManager> createManager() {
#ifdef Q_OS_LINUX
    if (auto *manager = Wayland::Globals::shortcutInhibit()) {
      return std::make_unique<WaylandShortcutInhibitManager>(manager);
    }
#endif
    return std::make_unique<DummyShortcutInhibitManager>();
  }

  std::unique_ptr<AbstractShortcutInhibitManager> m_manager;
};

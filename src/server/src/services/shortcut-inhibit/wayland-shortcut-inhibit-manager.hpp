#pragma once
#include <qwindow.h>
#include <unordered_map>
#include "keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h"
#include "services/shortcut-inhibit/abstract-shortcut-inhibit-manager.hpp"

class WaylandShortcutInhibitManager : public AbstractShortcutInhibitManager {
public:
  explicit WaylandShortcutInhibitManager(zwp_keyboard_shortcuts_inhibit_manager_v1 *manager);
  ~WaylandShortcutInhibitManager() override;

  bool isSupported() const override { return true; }
  bool inhibit(QWindow *win) override;
  bool release(QWindow *win) override;

protected:
  bool eventFilter(QObject *sender, QEvent *event) override;

private:
  zwp_keyboard_shortcuts_inhibit_manager_v1 *m_manager = nullptr;
  std::unordered_map<QWindow *, zwp_keyboard_shortcuts_inhibitor_v1 *> m_inhibitors;
};

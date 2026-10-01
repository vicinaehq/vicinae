#pragma once
#include <memory>
#include <qwindow.h>
#include <unordered_map>
#include <QtWaylandClient/QWaylandClientExtension>
#include "qwayland-keyboard-shortcuts-inhibit-unstable-v1.h"
#include "services/shortcut-inhibit/abstract-shortcut-inhibit-manager.hpp"

class KeyboardShortcutsInhibitV1 : public QWaylandClientExtensionTemplate<KeyboardShortcutsInhibitV1>,
                                   public QtWayland::zwp_keyboard_shortcuts_inhibit_manager_v1 {
  Q_OBJECT

public:
  KeyboardShortcutsInhibitV1();
};

class WaylandShortcutInhibitManager : public AbstractShortcutInhibitManager {
public:
  bool isSupported() const override { return m_manager.isActive(); }
  bool inhibit(QWindow *win) override;
  bool release(QWindow *win) override;

protected:
  bool eventFilter(QObject *sender, QEvent *event) override;

private:
  struct Inhibitor : QtWayland::zwp_keyboard_shortcuts_inhibitor_v1 {
    using zwp_keyboard_shortcuts_inhibitor_v1::zwp_keyboard_shortcuts_inhibitor_v1;
    ~Inhibitor() override {
      if (isInitialized()) destroy();
    }
  };

  KeyboardShortcutsInhibitV1 m_manager;
  std::unordered_map<QWindow *, std::unique_ptr<Inhibitor>> m_inhibitors;
};

#pragma once
#include "lib/keyboard/keybind-manager.hpp"
#include <QObject>
#include <qtmetamacros.h>

class KeybindBridge : public QObject {
  Q_OBJECT

signals:
  void keybindsChanged();

public:
  Q_PROPERTY(QString toggleActionPanel READ toggleActionPanel NOTIFY keybindsChanged)
  Q_PROPERTY(QString openSearchAccessory READ openSearchAccessory NOTIFY keybindsChanged)
  explicit KeybindBridge(QObject *parent = nullptr) : QObject(parent) {
    connect(KeybindManager::instance(), &KeybindManager::keybindChanged, this, &KeybindBridge::keybindsChanged);
  }

  QString toggleActionPanel() const { return resolve(Keybind::ToggleActionPanel); }
  QString openSearchAccessory() const { return resolve(Keybind::OpenSearchAccessorySelector); }

private:
  static QString resolve(Keybind bind) {
    auto s = KeybindManager::instance()->resolve(bind);
    if (!s.isValid()) return {};
    return s.toDisplayString();
  }
};

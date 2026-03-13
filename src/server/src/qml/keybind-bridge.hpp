#pragma once
#include <QObject>
#include <QVariantList>
#include <qtmetamacros.h>
#include "lib/keyboard/keybind-manager.hpp"

class KeybindBridge : public QObject {
  Q_OBJECT

signals:
  void keybindsChanged();

public:
  Q_PROPERTY(QString toggleActionPanelSequence READ toggleActionPanelSequence NOTIFY keybindsChanged)
  Q_PROPERTY(QVariantList toggleActionPanelTokens READ toggleActionPanelTokens NOTIFY keybindsChanged)
  Q_PROPERTY(QString openSearchAccessorySequence READ openSearchAccessorySequence NOTIFY keybindsChanged)
  Q_PROPERTY(QVariantList openSearchAccessoryTokens READ openSearchAccessoryTokens NOTIFY keybindsChanged)
  explicit KeybindBridge(QObject *parent = nullptr) : QObject(parent) {
    connect(KeybindManager::instance(), &KeybindManager::keybindChanged, this,
            &KeybindBridge::keybindsChanged);
  }

  QString toggleActionPanelSequence() const { return resolveSequence(Keybind::ToggleActionPanel); }
  QVariantList toggleActionPanelTokens() const { return resolveTokens(Keybind::ToggleActionPanel); }
  QString openSearchAccessorySequence() const {
    return resolveSequence(Keybind::OpenSearchAccessorySelector);
  }
  QVariantList openSearchAccessoryTokens() const {
    return resolveTokens(Keybind::OpenSearchAccessorySelector);
  }

private:
  static QString resolveSequence(Keybind bind) {
    auto s = KeybindManager::instance()->resolve(bind);
    if (!s.isValid()) return {};
    return s.toBindingSequence();
  }

  static QVariantList resolveTokens(Keybind bind) {
    auto s = KeybindManager::instance()->resolve(bind);
    if (!s.isValid()) return {};
    return s.toDisplayTokens();
  }
};

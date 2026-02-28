#pragma once
#include "config/config.hpp"
#include "service-registry.hpp"
#include <QObject>

class ConfigBridge : public QObject {
  Q_OBJECT

  Q_PROPERTY(qreal windowOpacity READ windowOpacity NOTIFY changed)
  Q_PROPERTY(int borderWidth READ borderWidth NOTIFY changed)
  Q_PROPERTY(int borderRounding READ borderRounding NOTIFY changed)
  Q_PROPERTY(int windowWidth READ windowWidth NOTIFY changed)
  Q_PROPERTY(int windowHeight READ windowHeight NOTIFY changed)
  Q_PROPERTY(bool emacsMode READ emacsMode NOTIFY changed)

signals:
  void changed();

public:
  explicit ConfigBridge(QObject *parent = nullptr) : QObject(parent) {
    connect(ServiceRegistry::instance()->config(), &config::Manager::configChanged, this,
            [this] { emit changed(); });
  }

  qreal windowOpacity() const { return cfg().launcherWindow.opacity; }

  int borderWidth() const {
    auto &csd = cfg().launcherWindow.clientSideDecorations;
    return csd.enabled ? csd.borderWidth : 0;
  }

  int borderRounding() const {
    auto &csd = cfg().launcherWindow.clientSideDecorations;
    return csd.enabled ? csd.rounding : 0;
  }

  int windowWidth() const { return cfg().launcherWindow.size.width; }
  int windowHeight() const { return cfg().launcherWindow.size.height; }
  bool emacsMode() const { return cfg().keybinding == "emacs"; }

private:
  static const config::ConfigValue &cfg() { return ServiceRegistry::instance()->config()->value(); }
};

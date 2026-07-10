#pragma once
#include "capabilities.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include <QColor>
#include <QObject>

class ConfigBridge : public QObject {
  Q_OBJECT

  Q_PROPERTY(qreal windowOpacity READ windowOpacity NOTIFY changed)
  Q_PROPERTY(qreal popupOpacity READ popupOpacity NOTIFY changed)
  Q_PROPERTY(qreal surfaceOpacity READ surfaceOpacity NOTIFY changed)
  Q_PROPERTY(qreal popupSurfaceOpacity READ popupSurfaceOpacity NOTIFY changed)
  Q_PROPERTY(int borderWidth READ borderWidth NOTIFY changed)
  Q_PROPERTY(int borderRounding READ borderRounding NOTIFY changed)
  Q_PROPERTY(int shadowSize READ shadowSize NOTIFY changed)
  Q_PROPERTY(int windowWidth READ windowWidth NOTIFY changed)
  Q_PROPERTY(int windowHeight READ windowHeight NOTIFY changed)
  Q_PROPERTY(bool emacsMode READ emacsMode NOTIFY changed)
  Q_PROPERTY(bool considerPreedit READ considerPreedit NOTIFY changed)
  Q_PROPERTY(bool activateOnSingleClick READ activateOnSingleClick NOTIFY changed)
  Q_PROPERTY(bool blurEnabled READ blurEnabled NOTIFY changed)
  Q_PROPERTY(QString windowMaterial READ windowMaterial NOTIFY changed)

signals:
  void changed();

public:
  explicit ConfigBridge(QObject *parent = nullptr) : QObject(parent) {
    connect(ServiceRegistry::instance()->config(), &config::Manager::configChanged, this,
            [this] { emit changed(); });
  }

  qreal windowOpacity() const {
    return cfg().launcher_window.resolvedOpacity(platform::supports(platform::Capability::LiquidGlass),
                                                platform::supports(platform::Capability::WindowMaterial));
  }

  qreal popupOpacity() const {
    return cfg().launcher_window.resolvedPopupOpacity(
        platform::supports(platform::Capability::LiquidGlass),
        platform::supports(platform::Capability::WindowMaterial));
  }

  qreal surfaceOpacity() const {
    return cfg().launcher_window.resolvedSurfaceOpacity(
        platform::supports(platform::Capability::LiquidGlass),
        platform::supports(platform::Capability::WindowMaterial));
  }

  qreal popupSurfaceOpacity() const {
    return cfg().launcher_window.resolvedPopupSurfaceOpacity(
        platform::supports(platform::Capability::LiquidGlass),
        platform::supports(platform::Capability::WindowMaterial));
  }

  int borderWidth() const {
    auto &csd = cfg().launcher_window.client_side_decorations;
    return csd.enabled ? csd.border_width : 0;
  }

  int borderRounding() const {
    const auto &window = cfg().launcher_window;
    if (platform::supports(platform::Capability::ClientSideDecorations)) {
      return window.client_side_decorations.enabled ? window.effectiveRounding() : 0;
    }
    return window.effectiveRounding();
  }

  int shadowSize() const {
    auto &csd = cfg().launcher_window.client_side_decorations;
    return csd.enabled ? csd.shadow_size : 0;
  }

  int windowWidth() const { return cfg().launcher_window.size.width; }
  int windowHeight() const { return cfg().launcher_window.size.height; }
  bool emacsMode() const { return cfg().keybinding == "emacs"; }
  bool considerPreedit() const { return cfg().consider_preedit; }
  bool activateOnSingleClick() const { return cfg().activate_on_single_click; }
  QString windowMaterial() const {
    return QString::fromStdString(
        cfg().launcher_window.resolvedMaterial(platform::supports(platform::Capability::LiquidGlass),
                                              platform::supports(platform::Capability::WindowMaterial)));
  }
  bool blurEnabled() const { return windowMaterial() != QStringLiteral("none"); }

  Q_INVOKABLE static QColor withAlpha(const QColor &c, qreal alpha) {
    return QColor::fromRgbF(c.redF(), c.greenF(), c.blueF(), alpha);
  }

private:
  static const config::ConfigValue &cfg() { return ServiceRegistry::instance()->config()->value(); }
};

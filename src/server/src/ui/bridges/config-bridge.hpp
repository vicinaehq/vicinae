#pragma once
#include "ui/qml-engine-scope.hpp"
#include "capabilities.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include <QColor>
#include <QObject>
#include <QVariant>

#ifdef Q_OS_WIN
#include "ui/quick/windows-chrome-attached.hpp"
#endif

class ConfigBridge : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Config)
  QML_SINGLETON

public:
  static ConfigBridge *create(QQmlEngine *engine, QJSEngine *) {
    return QmlEngineScope::get<ConfigBridge>(engine);
  }

private:
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
  Q_PROPERTY(bool floatingStatusBar READ floatingStatusBar NOTIFY changed)
  Q_PROPERTY(QString windowMaterial READ windowMaterial NOTIFY changed)
  Q_PROPERTY(QString popupMaterial READ popupMaterial NOTIFY changed)
  Q_PROPERTY(QVariant searchBarDivider READ searchBarDivider NOTIFY changed)
  Q_PROPERTY(QVariant statusBarDivider READ statusBarDivider NOTIFY changed)
  Q_PROPERTY(QVariant listItemHeight READ listItemHeight NOTIFY changed)
  Q_PROPERTY(QVariant listItemRounding READ listItemRounding NOTIFY changed)
  Q_PROPERTY(QVariant listItemInset READ listItemInset NOTIFY changed)

signals:
  void changed();

public:
  // Windows whose surfaces sit on an opaque background (settings) ignore the
  // configured translucency.
  enum SurfaceMode { TranslucentSurfaces, OpaqueSurfaces };

  explicit ConfigBridge(QObject *parent) : ConfigBridge(TranslucentSurfaces, parent) {}

  explicit ConfigBridge(SurfaceMode mode, QObject *parent = nullptr)
      : QObject(parent), m_opaqueSurfaces(mode == OpaqueSurfaces) {
    connect(ServiceRegistry::instance()->config(), &config::Manager::configChanged, this,
            [this] { emit changed(); });
  }

  qreal windowOpacity() const {
    if (m_opaqueSurfaces) return 1.0;
    return cfg().launcherWindow.resolvedOpacity(platform::supports(platform::Capability::LiquidGlass),
                                                platform::supports(platform::Capability::WindowMaterial));
  }

  qreal popupOpacity() const {
    if (m_opaqueSurfaces) return 1.0;
    return cfg().launcherWindow.resolvedPopupOpacity(
        platform::supports(platform::Capability::LiquidGlass),
        platform::supports(platform::Capability::WindowMaterial));
  }

  qreal surfaceOpacity() const {
    if (m_opaqueSurfaces) return 1.0;
    return cfg().launcherWindow.resolvedSurfaceOpacity(
        platform::supports(platform::Capability::LiquidGlass),
        platform::supports(platform::Capability::WindowMaterial));
  }

  qreal popupSurfaceOpacity() const {
    if (m_opaqueSurfaces) return 1.0;
    return cfg().launcherWindow.resolvedPopupSurfaceOpacity(
        platform::supports(platform::Capability::LiquidGlass),
        platform::supports(platform::Capability::WindowMaterial));
  }

  int borderWidth() const {
    auto &csd = cfg().launcherWindow.clientSideDecorations;
    return csd.enabled ? csd.borderWidth : 0;
  }

  int borderRounding() const {
#ifdef Q_OS_WIN
    if (!platform::supports(platform::Capability::CustomWindowRounding)) {
      return WindowsWindowAttached::nativeCornerRadius();
    }
#endif
    const auto &window = cfg().launcherWindow;
    if (platform::supports(platform::Capability::ClientSideDecorations)) {
      return window.clientSideDecorations.enabled ? window.effectiveRounding() : 0;
    }
    return window.effectiveRounding();
  }

  int shadowSize() const {
    auto &csd = cfg().launcherWindow.clientSideDecorations;
    return csd.enabled ? csd.shadowSize : 0;
  }

  int windowWidth() const { return cfg().launcherWindow.size.width; }
  int windowHeight() const { return cfg().launcherWindow.size.height; }
  bool emacsMode() const { return KeyBindingService::getMode(cfg().keybinding) == KeyBindingMode::Emacs; }
  bool considerPreedit() const { return cfg().considerPreedit; }
  bool activateOnSingleClick() const { return cfg().activateOnSingleClick; }
  QString windowMaterial() const {
    return QString::fromStdString(
        cfg().launcherWindow.resolvedMaterial(platform::supports(platform::Capability::LiquidGlass),
                                              platform::supports(platform::Capability::WindowMaterial)));
  }
  bool blurEnabled() const { return windowMaterial() != QStringLiteral("none"); }
  QString popupMaterial() const {
    return QString::fromStdString(
        cfg().launcherWindow.resolvedPopupMaterial(platform::supports(platform::Capability::LiquidGlass),
                                                   platform::supports(platform::Capability::WindowMaterial)));
  }
  bool floatingStatusBar() const { return cfg().launcherWindow.floatingStatusBar; }

  // Layout overrides are undefined in QML when unset, so that the platform appearance keeps its own metrics.
  QVariant searchBarDivider() const { return optional(cfg().launcherWindow.searchBar.divider); }
  QVariant statusBarDivider() const { return optional(cfg().launcherWindow.statusBar.divider); }
  QVariant listItemHeight() const { return optional(cfg().launcherWindow.list.itemHeight); }
  QVariant listItemRounding() const { return optional(cfg().launcherWindow.list.itemRounding); }
  QVariant listItemInset() const { return optional(cfg().launcherWindow.list.itemInset); }

  Q_INVOKABLE static QColor withAlpha(const QColor &c, qreal alpha) {
    return QColor::fromRgbF(c.redF(), c.greenF(), c.blueF(), alpha);
  }

private:
  static const config::ConfigValue &cfg() { return ServiceRegistry::instance()->config()->value(); }

  template <typename T> static QVariant optional(const std::optional<T> &value) {
    return value ? QVariant::fromValue(*value) : QVariant{};
  }

  const bool m_opaqueSurfaces;
};

#pragma once
#include <QObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <memory>
#include <vector>
#include "bar/bar-host.hpp"
#include "common/context.hpp"

class QScreen;
class QQmlContext;
class QQuickWindow;
class BarScreen;
class ThemeBridge;
class ConfigBridge;
class ImageSource;
class PlatformBridge;

/**
 * Owns the bar QML engine and one layer-shell window per connected screen.
 */
class BarController : public QObject {
  Q_OBJECT

public:
  explicit BarController(ApplicationContext &ctx, QObject *parent = nullptr);
  ~BarController() override;

private:
  struct ScreenBar {
    QScreen *screen = nullptr;
    QQmlContext *context = nullptr;
    BarScreen *bridge = nullptr;
    QQuickWindow *window = nullptr;
  };

  void addScreen(QScreen *screen);
  void removeScreen(QScreen *screen);
  void destroy(ScreenBar &bar);

  ApplicationContext &m_ctx;
  QQmlEngine m_engine;
  BarHost m_host;
  std::unique_ptr<QQmlComponent> m_component;
  std::vector<ScreenBar> m_bars;
};

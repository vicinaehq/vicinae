#include "bar/bar-controller.hpp"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickWindow>
#include <QScreen>
#include <algorithm>
#include "bar/bar-screen.hpp"
#include "config-bridge.hpp"
#include "image-source.hpp"
#include "navigation-controller.hpp"
#include "platform-bridge.hpp"
#include "style-bridge.hpp"
#include "theme-bridge.hpp"

BarController::BarController(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_host(ctx, this) {
  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Nav"), ctx.navigation.get());
  rootCtx->setContextProperty(QStringLiteral("Theme"), new ThemeBridge(this));
  rootCtx->setContextProperty(QStringLiteral("Config"), new ConfigBridge(this));
  rootCtx->setContextProperty(QStringLiteral("Platform"), new PlatformBridge(this));
  rootCtx->setContextProperty(QStringLiteral("Style"), new StyleBridge(this));
  rootCtx->setContextProperty(QStringLiteral("Img"), new ImageSource(this));
  rootCtx->setContextProperty(QStringLiteral("bar"), &m_host);

  m_component =
      std::make_unique<QQmlComponent>(&m_engine, QUrl(QStringLiteral("qrc:/Vicinae/BarWindow.qml")));
  if (m_component->isError()) {
    for (const auto &error : m_component->errors()) {
      qWarning() << "bar:" << error.toString();
    }
    return;
  }

  for (auto *screen : QGuiApplication::screens()) {
    addScreen(screen);
  }

  connect(qApp, &QGuiApplication::screenAdded, this, &BarController::addScreen);
  connect(qApp, &QGuiApplication::screenRemoved, this, &BarController::removeScreen);
}

BarController::~BarController() {
  for (auto &bar : m_bars) {
    destroy(bar);
  }
}

void BarController::addScreen(QScreen *screen) {
  if (std::ranges::any_of(m_bars, [&](auto &&bar) { return bar.screen == screen; })) return;

  ScreenBar bar{.screen = screen};
  bar.context = new QQmlContext(m_engine.rootContext(), this);
  bar.bridge = new BarScreen(m_host, screen->name(), bar.context);
  bar.context->setContextProperty(QStringLiteral("barScreen"), bar.bridge);

  auto *object = m_component->create(bar.context);
  bar.window = qobject_cast<QQuickWindow *>(object);

  if (!bar.window) {
    for (const auto &error : m_component->errors()) {
      qWarning() << "bar:" << error.toString();
    }
    if (object) object->deleteLater();
    bar.context->deleteLater();
    return;
  }

  bar.window->setScreen(screen);
  bar.window->show();
  m_bars.emplace_back(bar);
}

void BarController::removeScreen(QScreen *screen) {
  auto it = std::ranges::find_if(m_bars, [&](auto &&bar) { return bar.screen == screen; });
  if (it == m_bars.end()) return;
  destroy(*it);
  m_bars.erase(it);
}

void BarController::destroy(ScreenBar &bar) {
  if (bar.window) {
    bar.window->hide();
    bar.window->deleteLater();
  }
  if (bar.context) bar.context->deleteLater();
  bar = {};
}

#include "qml-settings-window.hpp"
#include "qml-async-image-provider.hpp"
#include "qml-config-bridge.hpp"
#include "qml-extension-settings-model.hpp"
#include "qml-general-settings-model.hpp"
#include "qml-image-source.hpp"
#include "qml-keybind-settings-model.hpp"
#include "qml-theme-bridge.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "services/app-service/app-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "vicinae.hpp"
#include "version.h"
#include <QQmlContext>
#include <QQuickWindow>

QmlSettingsWindow::QmlSettingsWindow(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx) {}

void QmlSettingsWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;

  m_themeBridge = new QmlThemeBridge(this);
  m_configBridge = new QmlConfigBridge(this);
  m_imgSource = new QmlImageSource(this);
  m_generalModel = new QmlGeneralSettingsModel(this);
  m_keybindModel = new QmlKeybindSettingsModel(this);
  m_extensionModel = new QmlExtensionSettingsModel(this);

  m_engine.addImageProvider(QStringLiteral("vicinae"), new QmlAsyncImageProvider());

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);
  rootCtx->setContextProperty(QStringLiteral("settings"), this);

  m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/SettingsWindow.qml")));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

  if (m_window) {
    connect(m_window, &QQuickWindow::visibleChanged, this, [this](bool visible) {
      if (!visible) m_ctx.settings->closeWindow();
    });
    connect(m_window, &QQuickWindow::widthChanged, this, &QmlSettingsWindow::updateBlur);
    connect(m_window, &QQuickWindow::heightChanged, this, &QmlSettingsWindow::updateBlur);
  }

  connect(m_ctx.services->config(), &config::Manager::configChanged, this, &QmlSettingsWindow::updateBlur);
}

void QmlSettingsWindow::setCurrentTab(int tab) {
  if (m_currentTab != tab) {
    m_currentTab = tab;
    emit currentTabChanged();
  }
}

QString QmlSettingsWindow::version() const { return QStringLiteral(VICINAE_GIT_TAG); }
QString QmlSettingsWindow::commitHash() const { return QStringLiteral(VICINAE_GIT_COMMIT_HASH); }
QString QmlSettingsWindow::buildInfo() const { return QStringLiteral(BUILD_INFO); }
QString QmlSettingsWindow::headline() const { return Omnicast::HEADLINE; }

void QmlSettingsWindow::openUrl(const QString &url) { m_ctx.services->appDb()->openTarget(url); }

void QmlSettingsWindow::close() {
  if (m_window) m_window->hide();
}

void QmlSettingsWindow::show() {
  ensureInitialized();
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
  updateBlur();
}

void QmlSettingsWindow::hide() {
  if (m_window) m_window->hide();
}

void QmlSettingsWindow::openTab(const QString &tabId) {
  ensureInitialized();
  static const std::array<std::pair<const char *, int>, 4> tabs = {{
      {"general", 0},
      {"extensions", 1},
      {"keybinds", 2},
      {"about", 3},
  }};
  for (const auto &[id, idx] : tabs) {
    if (tabId == id) {
      setCurrentTab(idx);
      return;
    }
  }
}

void QmlSettingsWindow::selectExtension(const QString &entrypointId) {
  ensureInitialized();
  m_extensionModel->selectByEntrypointId(entrypointId);
}

void QmlSettingsWindow::updateBlur() {
  if (!m_window) return;
  auto &cfg = m_ctx.services->config()->value();
  auto *bgEffect = m_ctx.services->backgroundEffectManager();
  if (!bgEffect->supportsBlur()) return;

  if (cfg.launcherWindow.blur.enabled) {
    QRect region(0, 0, m_window->width(), m_window->height());
    bgEffect->setBlur(m_window, {.radius = 10, .region = region});
  } else {
    bgEffect->clearBlur(m_window);
  }
}

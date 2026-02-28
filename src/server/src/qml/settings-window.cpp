#include "settings-window.hpp"
#include "async-image-provider.hpp"
#include "config-bridge.hpp"
#include "extension-settings-model.hpp"
#include "general-settings-model.hpp"
#include "image-source.hpp"
#include "keybind-settings-model.hpp"
#include "theme-bridge.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "services/app-service/app-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "vicinae.hpp"
#include "version.h"
#include <QQmlContext>
#include <QQuickWindow>

SettingsWindow::SettingsWindow(ApplicationContext &ctx, QObject *parent) : QObject(parent), m_ctx(ctx) {}

void SettingsWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;

  m_themeBridge = new ThemeBridge(this);
  m_configBridge = new ConfigBridge(this);
  m_imgSource = new ImageSource(this);
  m_generalModel = new GeneralSettingsModel(this);
  m_keybindModel = new KeybindSettingsModel(this);
  m_extensionModel = new ExtensionSettingsModel(this);

  m_engine.addImageProvider(QStringLiteral("vicinae"), new AsyncImageProvider());

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
    connect(m_window, &QQuickWindow::widthChanged, this, &SettingsWindow::updateBlur);
    connect(m_window, &QQuickWindow::heightChanged, this, &SettingsWindow::updateBlur);
  }

  connect(m_ctx.services->config(), &config::Manager::configChanged, this, &SettingsWindow::updateBlur);
}

void SettingsWindow::setCurrentTab(int tab) {
  if (m_currentTab != tab) {
    m_currentTab = tab;
    emit currentTabChanged();
  }
}

QString SettingsWindow::version() const { return QStringLiteral(VICINAE_GIT_TAG); }
QString SettingsWindow::commitHash() const { return QStringLiteral(VICINAE_GIT_COMMIT_HASH); }
QString SettingsWindow::buildInfo() const { return QStringLiteral(BUILD_INFO); }
QString SettingsWindow::headline() const { return Omnicast::HEADLINE; }

void SettingsWindow::openUrl(const QString &url) { m_ctx.services->appDb()->openTarget(url); }

void SettingsWindow::close() {
  if (m_window) m_window->hide();
}

void SettingsWindow::show() {
  ensureInitialized();
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
  updateBlur();
}

void SettingsWindow::hide() {
  if (m_window) m_window->hide();
}

void SettingsWindow::openTab(const QString &tabId) {
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

void SettingsWindow::selectExtension(const QString &entrypointId) {
  ensureInitialized();
  m_extensionModel->selectByEntrypointId(entrypointId);
}

void SettingsWindow::updateBlur() {
  if (!m_window) return;
  auto &cfg = m_ctx.services->config()->value();
  auto *bgEffect = m_ctx.services->backgroundEffectManager();
  if (!bgEffect->supportsBlur()) return;

  if (cfg.launcherWindow.blur.enabled) {
    QRect const region(0, 0, m_window->width(), m_window->height());
    bgEffect->setBlur(m_window, {.radius = 10, .region = region});
  } else {
    bgEffect->clearBlur(m_window);
  }
}

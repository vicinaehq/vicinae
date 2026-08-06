#include "settings-window.hpp"
#include "common/entrypoint.hpp"
#include "config-bridge.hpp"
#include "extension-settings-model.hpp"
#include "general-settings-model.hpp"
#include "image-source.hpp"
#include "keyboard-bridge.hpp"
#include "global-shortcut-bridge.hpp"
#include "platform-bridge.hpp"
#include "keybind-settings-model.hpp"
#include "settings-sidebar-model.hpp"
#include "theme-bridge.hpp"
#include "view-utils.hpp"
#include "config/config.hpp"
#include "extension/extension.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/app-service/app-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings-controller/settings-controller.hpp"
#include "vicinae.hpp"
#include "generated/version.h"
#include "fuzzy/fuzzy-searchable.hpp"
#include <QQmlContext>
#include <QQuickWindow>
#ifdef Q_OS_MACOS
#include "macos-chrome-attached.hpp"
#endif

SettingsWindow::SettingsWindow(ApplicationContext &ctx, QObject *parent) : QObject(parent), m_ctx(ctx) {}

void SettingsWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;

  m_themeBridge = new ThemeBridge(this);
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
  m_configBridge = new ConfigBridge(ConfigBridge::OpaqueSurfaces, this);
#else
  m_configBridge = new ConfigBridge(this);
#endif
  m_imgSource = new ImageSource(this);
  m_keyboardBridge = new KeyboardBridge(this);
  m_globalShortcutBridge = new GlobalShortcutBridge(this);
  m_platformBridge = new PlatformBridge(this);
  m_generalModel = new GeneralSettingsModel(this);
  m_keybindModel = new KeybindSettingsModel(this);
  m_extensionModel = new ExtensionSettingsModel(this);
  m_sidebarModel = new SettingsSidebarModel(m_extensionModel, this);

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);
  rootCtx->setContextProperty(QStringLiteral("Keyboard"), m_keyboardBridge);
  rootCtx->setContextProperty(QStringLiteral("GlobalShortcuts"), m_globalShortcutBridge);
  rootCtx->setContextProperty(QStringLiteral("Platform"), m_platformBridge);
  rootCtx->setContextProperty(QStringLiteral("settings"), this);
  rootCtx->setContextProperty(QStringLiteral("FileChooser"),
                              ServiceRegistry::instance()->fileChooserService());

  m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/SettingsWindow.qml")));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

  if (m_window) {
    connect(m_window, &QQuickWindow::visibleChanged, this, [this](bool visible) {
      if (!visible) m_ctx.settings->closeWindow();
    });
  }
}

void SettingsWindow::setCurrentPage(const QString &page) {
  if (m_currentPage != page) {
    m_currentPage = page;
    emit currentPageChanged();
  }
}

void SettingsWindow::setPendingCommandId(const QString &id) {
  if (m_pendingCommandId != id) {
    m_pendingCommandId = id;
    emit pendingCommandIdChanged();
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

void SettingsWindow::requestDefaultFocus() { emit defaultFocusRequested(); }

void SettingsWindow::show() {
  ensureInitialized();
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
#ifdef Q_OS_MACOS
  macosActivateApp();
#endif
}

void SettingsWindow::hide() {
  if (m_window) m_window->hide();
}

void SettingsWindow::openTab(const QString &tabId) {
  ensureInitialized();
  if (tabId == "keybinds" || tabId == "shortcuts") {
    setCurrentPage(QStringLiteral("keybindings"));
  } else if (tabId == "extensions") {
    setCurrentPage(QStringLiteral("general"));
  } else {
    setCurrentPage(tabId);
  }
}

void SettingsWindow::selectExtension(const QString &entrypointId) {
  ensureInitialized();
  auto providerId = QString::fromStdString(EntrypointId::fromSerialized(entrypointId.toStdString()).provider);
  if (providerId.isEmpty()) return;
  m_extensionModel->selectProviderById(providerId);
  // Page first so the surviving (new) page handles the pending command.
  setCurrentPage(providerId);
  setPendingCommandId(entrypointId);
}

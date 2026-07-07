#include "onboarding-window.hpp"
#include "config-bridge.hpp"
#include "general-settings-model.hpp"
#include "global-shortcut-bridge.hpp"
#include "image-source.hpp"
#include "keyboard-bridge.hpp"
#include "platform-bridge.hpp"
#include "theme-bridge.hpp"
#include "services/app-service/app-service.hpp"
#include "vicinae.hpp"
#include <QDateTime>
#include <QDebug>
#include <QQmlContext>
#include <QQuickWindow>
#include <filesystem>
#include <glaze/glaze.hpp>

#ifdef Q_OS_MACOS
#include "macos-chrome-attached.hpp"
#include "services/autostart/macos-login-item.hpp"
#include "services/permissions/macos-permission-service.hpp"
#endif

struct OnboardingState {
  int version = 0;
  std::string completedAt;
};

namespace {

constexpr int ONBOARDING_VERSION = 1;

std::filesystem::path stateFilePath() { return Omnicast::stateDir() / "onboarding.json"; }

int completedVersion() {
  OnboardingState state;
  std::string buf;
  if (glz::read_file_json(state, stateFilePath().c_str(), buf)) return 0;
  return state.version;
}

} // namespace

OnboardingWindow::OnboardingWindow(ApplicationContext &ctx, QObject *parent) : QObject(parent), m_ctx(ctx) {}

bool OnboardingWindow::shouldShow() {
#if defined(Q_OS_MACOS) && defined(ENABLE_ONBOARDING)
  return completedVersion() < ONBOARDING_VERSION;
#else
  return false;
#endif
}

bool OnboardingWindow::loginItemSupported() const {
#ifdef Q_OS_MACOS
  return vicinae::macos::isLoginItemSupported();
#else
  return false;
#endif
}

void OnboardingWindow::setLoginItemEnabled(bool enabled) {
#ifdef Q_OS_MACOS
  vicinae::macos::setLoginItemEnabled(enabled);
  const bool effective = vicinae::macos::isLoginItemEnabled();
  if (effective != m_loginItemEnabled) {
    m_loginItemEnabled = effective;
    emit loginItemEnabledChanged();
  }
#else
  Q_UNUSED(enabled);
#endif
}

void OnboardingWindow::finish() {
  markCompleted();
  if (m_window) m_window->hide();
}

void OnboardingWindow::openUrl(const QString &url) { m_ctx.services->appDb()->openTarget(url); }

void OnboardingWindow::show() {
  ensureInitialized();
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
#ifdef Q_OS_MACOS
  macosActivateApp();
#endif
}

void OnboardingWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;

  m_themeBridge = new ThemeBridge(this);
  m_configBridge = new ConfigBridge(this);
  m_imgSource = new ImageSource(this);
  m_keyboardBridge = new KeyboardBridge(this);
  m_globalShortcutBridge = new GlobalShortcutBridge(this);
  m_platformBridge = new PlatformBridge(this);
  m_generalModel = new GeneralSettingsModel(this);

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);
  rootCtx->setContextProperty(QStringLiteral("Keyboard"), m_keyboardBridge);
  rootCtx->setContextProperty(QStringLiteral("GlobalShortcuts"), m_globalShortcutBridge);
  rootCtx->setContextProperty(QStringLiteral("Platform"), m_platformBridge);
  rootCtx->setContextProperty(QStringLiteral("onboarding"), this);

#ifdef Q_OS_MACOS
  m_loginItemEnabled = vicinae::macos::isLoginItemEnabled();
  m_permissions = new MacosPermissionService(this);
  rootCtx->setContextProperty(QStringLiteral("Permissions"), m_permissions);
#endif

  m_engine.load(QUrl(
#ifdef Q_OS_MACOS
      QStringLiteral("qrc:/Vicinae/OnboardingWindowMacOS.qml")
#else
      QStringLiteral("qrc:/Vicinae/OnboardingWindow.qml")
#endif
          ));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

#ifdef Q_OS_MACOS
  if (m_window) {
    connect(m_window, &QQuickWindow::visibleChanged, m_permissions,
            [this](bool visible) { m_permissions->setWatching(visible); });
    m_permissions->setWatching(m_window->isVisible());
  }
#endif
}

void OnboardingWindow::markCompleted() {
  if (m_completed) return;
  m_completed = true;

  const OnboardingState state{
      .version = ONBOARDING_VERSION,
      .completedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString(),
  };

  std::string buf;
  if (auto const error = glz::write_file_json(state, stateFilePath().c_str(), buf)) {
    qWarning() << "Failed to write onboarding state file" << stateFilePath().c_str();
  }
}

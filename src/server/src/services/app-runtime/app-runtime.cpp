#include "app-runtime.hpp"
#include "vicinae.hpp"

#ifdef Q_OS_MACOS
#include "services/app-runtime/macos/mac-app-runtime.hpp"
#elif defined(Q_OS_WIN)
#include "services/app-runtime/windows/win-app-runtime.hpp"
#else
#include "services/app-runtime/linux/linux-app-runtime.hpp"
#endif

namespace {

bool isLauncherName(const QString &name) { return name.contains(Omnicast::APP_ID, Qt::CaseInsensitive); }

} // namespace

AppRuntime::AppRuntime(WindowManager &wm, AppService &appService)
    : m_provider(createProvider(wm, appService)) {
  connect(m_provider.get(), &AbstractAppRuntime::runningAppsChanged, this, &AppRuntime::runningAppsChanged);
  connect(m_provider.get(), &AbstractAppRuntime::frontmostAppChanged, this, [this]() {
    rememberPasteTarget();
    emit frontmostAppChanged();
  });
  rememberPasteTarget();
}

void AppRuntime::rememberPasteTarget() {
  if (auto app = m_provider->frontmostApp(); app && !isLauncherName(app->displayName())) {
    m_pasteTargetApp = app;
    m_pasteTargetName = app->displayName();
    return;
  }

  const auto name = m_provider->frontmostAppDisplayName();
  if (name.isEmpty() || isLauncherName(name)) return;
  m_pasteTargetName = name;
}

void AppRuntime::capturePasteTarget() { rememberPasteTarget(); }

QString AppRuntime::pasteTargetName() const {
  if (auto app = pasteTargetApp()) return app->displayName();
  const auto current = m_provider->frontmostAppDisplayName();
  if (!current.isEmpty() && !isLauncherName(current)) return current;
  return m_pasteTargetName;
}

std::shared_ptr<AbstractApplication> AppRuntime::pasteTargetApp() const {
  if (auto app = m_provider->frontmostApp(); app && !isLauncherName(app->displayName())) return app;
  return m_pasteTargetApp;
}

bool AppRuntime::isRunning(const AbstractApplication &app) const { return m_provider->isRunning(app); }

std::shared_ptr<AbstractApplication> AppRuntime::frontmostApp() const { return m_provider->frontmostApp(); }

bool AppRuntime::activate(const AbstractApplication &app) const { return m_provider->activate(app); }

bool AppRuntime::quit(const AbstractApplication &app) const { return m_provider->quit(app); }

bool AppRuntime::forceQuit(const AbstractApplication &app) const { return m_provider->forceQuit(app); }

std::unique_ptr<AbstractAppRuntime> AppRuntime::createProvider(WindowManager &wm, AppService &appService) {
#ifdef Q_OS_MACOS
  (void)wm;
  return std::make_unique<MacAppRuntime>(appService);
#elif defined(Q_OS_WIN)
  return std::make_unique<WindowsAppRuntime>(wm, appService);
#else
  return std::make_unique<LinuxAppRuntime>(wm, appService);
#endif
}

#include "services/tray/tray-service.hpp"
#ifdef Q_OS_MACOS
#include "services/tray/macos/tray-service-macos.hpp"
#elif defined(Q_OS_WIN)
#include "services/tray/windows/tray-service-windows.hpp"
#elif defined(Q_OS_LINUX)
#include "services/tray/linux/tray-service-linux.hpp"
#endif

QString TrayService::toggleLabel() { return tr("Toggle Vicinae"); }

QString TrayService::aboutLabel() { return tr("About Vicinae"); }

QString TrayService::checkForUpdatesLabel() { return tr("Check for Updates…"); }

QString TrayService::updateAvailableLabel(const QString &tag) { return tr("Update Available: %1").arg(tag); }

QString TrayService::settingsLabel() { return tr("Settings…"); }

QString TrayService::preferencesLabel() { return tr("Preferences…"); }

QString TrayService::sponsorLabel() { return tr("Sponsor Vicinae"); }

QString TrayService::discordLabel() { return tr("Join the Discord"); }

QString TrayService::followLabel() { return tr("Follow on X"); }

QString TrayService::quitLabel() { return tr("Quit Vicinae"); }

std::unique_ptr<TrayService> createTrayService() {
#ifdef Q_OS_MACOS
  return std::make_unique<TrayServiceMacOS>();
#elif defined(Q_OS_WIN)
  return std::make_unique<TrayServiceWindows>();
#elif defined(Q_OS_LINUX)
  return std::make_unique<TrayServiceLinux>();
#else
  return nullptr;
#endif
}

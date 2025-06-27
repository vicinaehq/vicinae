#include "app-settings-detail.hpp"
#include "service-registry.hpp"
#include <qlogging.h>

void AppSettingsDetail::setupUI() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  QJsonObject preferences = manager->getProviderPreferenceValues("apps");

  for (const auto &obj : preferences.value("paths").toArray()) {
    qCritical() << "PATH" << obj.toString();
  }
}

AppSettingsDetail::AppSettingsDetail() { setupUI(); }

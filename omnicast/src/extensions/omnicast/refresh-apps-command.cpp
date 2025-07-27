#include "extensions/omnicast/refresh-apps-command.hpp"
#include "command.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/toast.hpp"

RefreshAppsCommandContext::RefreshAppsCommandContext(const std::shared_ptr<AbstractCmd> &command)
    : CommandContext(command) {}

void RefreshAppsCommandContext::load(const LaunchProps &props) {
  auto appDb = ServiceRegistry::instance()->appDb();
  auto toast = context()->services->toastService();

  if (appDb->scanSync()) {
    toast->setToast("Apps successfully refreshed");
  } else {
    toast->setToast("Failed to refresh apps", ToastPriority::Danger);
  }
}

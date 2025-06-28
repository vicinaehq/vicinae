#include "extensions/omnicast/refresh-apps-command.hpp"
#include "command.hpp"
#include "service-registry.hpp"
#include "ui/toast.hpp"

RefreshAppsCommandContext::RefreshAppsCommandContext(const std::shared_ptr<AbstractCmd> &command)
    : CommandContext(command) {}

void RefreshAppsCommandContext::load(const LaunchProps &props) {
  auto appDb = ServiceRegistry::instance()->appDb();
  auto ui = ServiceRegistry::instance()->UI();

  if (appDb->scanSync()) {
    ui->setToast("Apps successfully refreshed");
  } else {
    ui->setToast("Failed to refresh apps", ToastPriority::Danger);
  }
}

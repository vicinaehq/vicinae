#include "extensions/vicinae/refresh-apps-command.hpp"
#include "command-controller.hpp"
#include "common.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/toast/toast.hpp"

void RefreshAppsCommand::execute(CommandController &controller) const {
  auto ctx = controller.context();
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  if (appDb->scanSync()) {
    toast->setToast(tr("Apps successfully refreshed"));
  } else {
    toast->setToast(tr("Failed to refresh apps"), ToastStyle::Danger);
  }
}

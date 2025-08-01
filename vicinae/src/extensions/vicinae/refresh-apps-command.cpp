#include "extensions/vicinae/refresh-apps-command.hpp"
#include "common.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/toast/toast.hpp"

void RefreshAppsCommand::execute(ApplicationContext *ctx) const {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  if (appDb->scanSync()) {
    toast->setToast("Apps successfully refreshed");
  } else {
    toast->setToast("Failed to refresh apps", ToastPriority::Danger);
  }
}

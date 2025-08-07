#include "extensions/vicinae/open-documentation-command.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/toast/toast.hpp"
#include <qlogging.h>

static const char *DOC_URL = "https://docs.vicinae.com";

void OpenDocumentationCommand::execute(ApplicationContext *ctx) const {
  auto appDb = ctx->services->appDb();

  if (auto browser = appDb->webBrowser()) {
    appDb->launch(*browser, {DOC_URL});
    ctx->navigation->showHud("Opened in browser");
    return;
  }

  ctx->services->toastService()->setToast("No browser to open the link", ToastPriority::Danger);
}

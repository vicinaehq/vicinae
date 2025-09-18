#include "extensions/vicinae/open-documentation-command.hpp"
#include <qlogging.h>

static const char *DOC_URL = "https://docs.vicinae.com";

void OpenDocumentationCommand::execute(CommandController *ctx) const {
  auto appDb = ctx->services->appDb();

  if (auto browser = appDb->webBrowser()) {
    appDb->launch(*browser, {DOC_URL});
    ctx->navigation->showHud("Opened in browser");
    return;
  }

  ctx->services->toastService()->setToast("No browser to open the link", ToastPriority::Danger);
}

#include "extensions/vicinae/open-documentation-command.hpp"
#include "command.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/toast/toast.hpp"
#include <qlogging.h>

static const char *DOC_URL = "https://docs.vicinae.com";

OpenDocumentationCommand::OpenDocumentationCommand(const std::shared_ptr<AbstractCmd> &cmd)
    : CommandContext(cmd) {}

void OpenDocumentationCommand::load(const LaunchProps &props) {
  auto appDb = context()->services->appDb();

  if (auto browser = appDb->webBrowser()) {
    appDb->launch(*browser, {DOC_URL});
    return;
  }

  context()->services->toastService()->setToast("No browser to open the link", ToastPriority::Danger);
}

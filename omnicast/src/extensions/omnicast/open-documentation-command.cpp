#include "extensions/omnicast/open-documentation-command.hpp"
#include "command.hpp"
#include "service-registry.hpp"
#include "ui/toast.hpp"
#include <qlogging.h>

static const char *DOC_URL = "https://omnicast.sh/docs";

OpenDocumentationCommand::OpenDocumentationCommand(const std::shared_ptr<AbstractCmd> &cmd)
    : CommandContext(cmd) {}

void OpenDocumentationCommand::load(const LaunchProps &props) {
  auto appDb = ServiceRegistry::instance()->appDb();
  auto ui = ServiceRegistry::instance()->UI();

  if (auto browser = appDb->webBrowser()) {
    appDb->launch(*browser, {DOC_URL});
    return;
  }

  ui->setToast("No browser to open the link", ToastPriority::Danger);
}

#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"

void OpenAppAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  if (!appDb->launch(*application.get(), args)) {
    toast->setToast("Failed to start app", ToastStyle::Danger);
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenAppAction::OpenAppAction(const std::shared_ptr<AbstractApplication> &app, const QString &title,
                             const std::vector<QString> args)
    : AbstractAction(title, app->iconUrl()), application(app), args(args) {}

void OpenRawProgramAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  if (!appDb->launchRaw(m_prog, m_args)) {
    toast->failure("Failed to start app");
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenRawProgramAction::OpenRawProgramAction(const QString &prog, const std::vector<QString> args)
    : m_prog(prog), m_args(args) {}

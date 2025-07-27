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
    qDebug() << "Failed to launch app";
    toast->setToast("Failed to start app", ToastPriority::Danger);
    return;
  }

  ctx->navigation->closeWindow();
}

OpenAppAction::OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                             const std::vector<QString> args)
    : AbstractAction(title, app->iconUrl()), application(app), args(args) {}

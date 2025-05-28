#include "actions/app/app-actions.hpp"
#include "service-registry.hpp"

void OpenAppAction::execute() {
  auto appDb = ServiceRegistry::instance()->appDb();
  auto ui = ServiceRegistry::instance()->UI();

  if (!appDb->launch(*application.get(), args)) {
    qDebug() << "Failed to launch app";
    ui->setToast("Failed to start app", ToastPriority::Danger);
    return;
  }

  ui->popToRoot();
  ui->closeWindow();
}

OpenAppAction::OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                             const std::vector<QString> args)
    : AbstractAction(title, app->iconUrl()), application(app), args(args) {}

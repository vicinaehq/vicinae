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

ActionPanelView *OpenWithAppAction::createSubmenu() const {
  auto panel = new ActionPanelStaticListView;
  auto appDb = ServiceRegistry::instance()->appDb();
  auto filterApp = [](const std::shared_ptr<Application> &app) -> bool { return app->displayable(); };
  auto makeAction = [&](const std::shared_ptr<Application> &app) {
    return new OpenAppAction(app, app->name(), {m_arguments});
  };

  panel->setTitle("Open with...");
  auto actions = appDb->list() | std::views::filter(filterApp) | std::views::transform(makeAction);
  std::ranges::for_each(actions, [&](AbstractAction *action) { panel->addAction(action); });

  return panel;
}

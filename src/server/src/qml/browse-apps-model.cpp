#include "browse-apps-model.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/wm/window-actions.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

QString BrowseAppsModel::displayTitle(const AppPtr &app) const { return app->displayName(); }

QString BrowseAppsModel::displaySubtitle(const AppPtr &app) const { return app->description(); }

QString BrowseAppsModel::displayIconSource(const AppPtr &app) const {
  return imageSourceFor(app->iconUrl());
}

QVariantList BrowseAppsModel::displayAccessory(const AppPtr &app) const {
  if (!app->displayable()) return qml::textAccessory(QStringLiteral("Hidden"));
  return {};
}

std::unique_ptr<ActionPanelState> BrowseAppsModel::buildActionPanel(const AppPtr &app) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto appDb = ctx()->services->appDb();

  panel->setTitle(app->displayName());

  auto *mainSection = panel->createSection();
  auto *utils = panel->createSection();

  auto activeWindows = ctx()->services->windowManager()->findAppWindows(*app);
  if (!activeWindows.empty()) {
    mainSection->addAction(new FocusWindowAction(activeWindows.front()));
  }

  auto *open = new OpenAppAction(app, "Open Application", {});
  open->setClearSearch(true);
  mainSection->addAction(open);

  auto actions = app->actions();
  for (int i = 0; i < static_cast<int>(actions.size()); ++i) {
    auto *action = new OpenAppAction(actions[i], actions[i]->displayName(), {});
    if (i < 9)
      action->setShortcut(QString("control+shift+%1").arg(i + 1));
    mainSection->addAction(action);
  }

  if (auto opener = appDb->findDefaultOpener(app->path().c_str())) {
    auto *openLocation = new OpenAppAction(opener, "Open Location", {app->path().c_str()});
    openLocation->setShortcut(Keybind::OpenAction);
    utils->addAction(openLocation);
  }

  auto *copyId = new CopyToClipboardAction(Clipboard::Text(app->id()), "Copy App ID");
  utils->addAction(copyId);

  auto *copyLocation = new CopyToClipboardAction(Clipboard::Text(app->path().c_str()), "Copy App Location");
  utils->addAction(copyLocation);

  return panel;
}

QString BrowseAppsModel::sectionLabel() const {
  return QStringLiteral("Applications ({count})");
}

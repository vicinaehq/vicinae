#include "browse-apps-model.hpp"

#include <utility>
#include "actions/app/app-actions.hpp"
#include "actions/wm/window-actions.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

QString BrowseAppsSection::displayTitle(const AppPtr &app) const { return app->displayName(); }

QString BrowseAppsSection::displaySubtitle(const AppPtr &app) const { return app->description(); }

QString BrowseAppsSection::displayIconSource(const AppPtr &app) const {
  return imageSourceFor(app->iconUrl());
}

QVariantList BrowseAppsSection::displayAccessories(const AppPtr &app) const {
  if (!app->displayable()) return qml::textAccessory(QStringLiteral("Hidden"));
  return {};
}

std::unique_ptr<ActionPanelState> BrowseAppsSection::buildActionPanel(const AppPtr &app) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto appDb = scope().services()->appDb();

  panel->setTitle(app->displayName());

  auto *mainSection = panel->createSection();
  auto *utils = panel->createSection();

  auto activeWindows = scope().services()->windowManager()->findAppWindows(*app);
  if (!activeWindows.empty()) { mainSection->addAction(new FocusWindowAction(activeWindows.front())); }

  auto *open = new OpenAppAction(app, "Open Application", {});
  open->setClearSearch(true);
  mainSection->addAction(open);

  auto actions = app->actions();
  for (int i = 0; std::cmp_less(i, actions.size()); ++i) {
    auto *action = new OpenAppAction(actions[i], actions[i]->displayName(), {});
    if (i < 9) action->setShortcut(QString("control+shift+%1").arg(i + 1));
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

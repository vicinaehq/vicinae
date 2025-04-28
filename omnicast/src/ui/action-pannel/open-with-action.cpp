#include "ui/action-pannel/open-with-action.hpp"
#include "app.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include <qnamespace.h>

std::vector<ActionItem> OpenWithAction::generateItems(const QString &query) const {
  std::vector<ActionItem> items;
  auto appDb = ServiceRegistry::instance()->appDb();

  items.push_back(ActionLabel("Open with..."));

  for (const auto &app : appDb->list()) {
    if (app->name().contains(query, Qt::CaseInsensitive)) {
      items.push_back(std::make_unique<OpenAppAction>(app, app->name(), args));
    }
  }

  return items;
};

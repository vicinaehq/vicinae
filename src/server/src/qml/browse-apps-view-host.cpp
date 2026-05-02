#include "browse-apps-view-host.hpp"
#include "browse-apps-model.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

void BrowseAppsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText("Search apps...");
}

void BrowseAppsViewHost::loadInitialData() { reload(); }

void BrowseAppsViewHost::reload() {
  auto appDb = context()->services->appDb();
  auto preferences = command()->preferenceValues();
  AppListOptions const opts{.sortAlphabetically = preferences.value("sortAlphabetically").toBool()};
  bool const showHidden = preferences.value("showHidden").toBool();

  auto apps = appDb->list(opts);
  std::vector<AppPtr> filtered;
  filtered.reserve(apps.size());

  for (auto &app : apps) {
    if (!showHidden && !app->displayable()) continue;
    filtered.emplace_back(std::move(app));
  }

  m_section.setItems(std::move(filtered));
}

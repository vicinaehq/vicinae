#include "builtins/system/browse-apps-view-host.hpp"
#include "builtins/system/browse-apps-model.hpp"
#include "builtins/system/browse-apps-preferences.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

void BrowseAppsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText(tr("Search apps..."));
}

void BrowseAppsViewHost::loadInitialData() { reload(); }

void BrowseAppsViewHost::reload() {
  auto appDb = context()->services->appDb();
  const auto preferences = command()->preferences<BrowseAppsPreferences>();
  AppListOptions const opts{.sortAlphabetically = preferences.sortAlphabetically};
  bool const showHidden = preferences.showHidden;

  auto apps = appDb->list(opts);
  std::vector<AppPtr> filtered;
  filtered.reserve(apps.size());

  for (auto &app : apps) {
    if (!showHidden && !app->displayable()) continue;
    filtered.emplace_back(std::move(app));
  }

  m_section.setItems(std::move(filtered));
}

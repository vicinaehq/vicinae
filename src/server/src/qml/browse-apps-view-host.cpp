#include "browse-apps-view-host.hpp"
#include "browse-apps-model.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

QUrl BrowseAppsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap BrowseAppsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void BrowseAppsViewHost::initialize() {
  BaseView::initialize();

  m_model = new BrowseAppsModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search apps...");
}

void BrowseAppsViewHost::loadInitialData() { reload(); }

void BrowseAppsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void BrowseAppsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void BrowseAppsViewHost::beforePop() { m_model->beforePop(); }

QObject *BrowseAppsViewHost::listModel() const { return m_model; }

void BrowseAppsViewHost::reload() {
  auto appDb = context()->services->appDb();
  auto preferences = command()->preferenceValues();
  AppListOptions opts{.sortAlphabetically = preferences.value("sortAlphabetically").toBool()};
  bool showHidden = preferences.value("showHidden").toBool();

  auto apps = appDb->list(opts);
  std::vector<AppPtr> filtered;
  filtered.reserve(apps.size());

  for (auto &app : apps) {
    if (!showHidden && !app->displayable()) continue;
    filtered.emplace_back(std::move(app));
  }

  m_model->setItems(std::move(filtered));
}

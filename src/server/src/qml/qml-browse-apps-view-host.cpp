#include "qml-browse-apps-view-host.hpp"
#include "qml-browse-apps-model.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

QUrl QmlBrowseAppsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlBrowseAppsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlBrowseAppsViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlBrowseAppsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search apps...");
}

void QmlBrowseAppsViewHost::loadInitialData() { reload(); }

void QmlBrowseAppsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void QmlBrowseAppsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void QmlBrowseAppsViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlBrowseAppsViewHost::listModel() const { return m_model; }

void QmlBrowseAppsViewHost::reload() {
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

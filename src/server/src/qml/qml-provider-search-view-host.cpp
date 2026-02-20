#include "qml-provider-search-view-host.hpp"
#include "qml-provider-search-model.hpp"
#include "service-registry.hpp"

QmlProviderSearchViewHost::QmlProviderSearchViewHost(const ExtensionRootProvider &provider)
    : m_providerId(provider.uniqueId()), m_displayName(provider.repository()->displayName()),
      m_icon(provider.icon()) {}

QUrl QmlProviderSearchViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlProviderSearchViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlProviderSearchViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlProviderSearchModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText(QString("Search %1").arg(m_displayName));
  setNavigationTitle(m_displayName);
  setNavigationIcon(m_icon);

  auto *manager = context()->services->rootItemManager();
  connect(manager, &RootItemManager::itemsChanged, this, [this]() { refresh(searchText()); });
}

void QmlProviderSearchViewHost::loadInitialData() { refresh({}); }

void QmlProviderSearchViewHost::textChanged(const QString &text) { refresh(text); }

void QmlProviderSearchViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlProviderSearchViewHost::listModel() const { return m_model; }

void QmlProviderSearchViewHost::refresh(const QString &text) {
  auto *manager = context()->services->rootItemManager();
  auto results = manager->search(text, {.providerId = m_providerId.toStdString()});
  m_model->setItems(std::move(results));
}

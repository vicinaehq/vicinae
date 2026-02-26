#include "provider-search-view-host.hpp"
#include "provider-search-model.hpp"
#include "service-registry.hpp"

ProviderSearchViewHost::ProviderSearchViewHost(const ExtensionRootProvider &provider)
    : m_providerId(provider.uniqueId()), m_displayName(provider.repository()->displayName()),
      m_icon(provider.icon()) {}

QUrl ProviderSearchViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap ProviderSearchViewHost::qmlProperties() {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void ProviderSearchViewHost::initialize() {
  BaseView::initialize();

  m_model = new ProviderSearchModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText(QString("Search %1").arg(m_displayName));
  setNavigationTitle(m_displayName);
  setNavigationIcon(m_icon);

  auto *manager = context()->services->rootItemManager();
  connect(manager, &RootItemManager::itemsChanged, this, [this]() { refresh(searchText()); });
}

void ProviderSearchViewHost::loadInitialData() { refresh({}); }

void ProviderSearchViewHost::textChanged(const QString &text) { refresh(text); }

void ProviderSearchViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *ProviderSearchViewHost::listModel() const { return m_model; }

void ProviderSearchViewHost::refresh(const QString &text) {
  auto *manager = context()->services->rootItemManager();
  auto results = manager->search(text, {.providerId = m_providerId.toStdString()});
  m_model->setItems(std::move(results));
}

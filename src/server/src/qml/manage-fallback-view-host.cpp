#include "manage-fallback-view-host.hpp"
#include "manage-fallback-model.hpp"
#include "service-registry.hpp"

QUrl ManageFallbackViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap ManageFallbackViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void ManageFallbackViewHost::initialize() {
  BaseView::initialize();

  m_model = new ManageFallbackModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search commands...");

  auto manager = context()->services->rootItemManager();
  connect(manager, &RootItemManager::fallbackEnabled, this, &ManageFallbackViewHost::reload);
  connect(manager, &RootItemManager::fallbackDisabled, this, &ManageFallbackViewHost::reload);
}

void ManageFallbackViewHost::loadInitialData() { reload(); }

void ManageFallbackViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void ManageFallbackViewHost::onReactivated() { m_model->refreshActionPanel(); }

void ManageFallbackViewHost::beforePop() { m_model->beforePop(); }

QObject *ManageFallbackViewHost::listModel() const { return m_model; }

void ManageFallbackViewHost::reload() {
  auto manager = context()->services->rootItemManager();

  m_model->setFallbackItems(manager->fallbackItems());

  auto results = manager->search("");
  std::vector<RootItemPtr> items;
  for (const auto &scored : results) {
    auto item = scored.item.get().get();
    if (item->isSuitableForFallback())
      items.emplace_back(scored.item.get());
  }

  m_model->setItems(std::move(items));
}

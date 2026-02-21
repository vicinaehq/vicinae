#include "qml-manage-fallback-view-host.hpp"
#include "qml-manage-fallback-model.hpp"
#include "service-registry.hpp"

QUrl QmlManageFallbackViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlManageFallbackViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlManageFallbackViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlManageFallbackModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search commands...");

  auto manager = context()->services->rootItemManager();
  connect(manager, &RootItemManager::fallbackEnabled, this, &QmlManageFallbackViewHost::reload);
  connect(manager, &RootItemManager::fallbackDisabled, this, &QmlManageFallbackViewHost::reload);
}

void QmlManageFallbackViewHost::loadInitialData() { reload(); }

void QmlManageFallbackViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void QmlManageFallbackViewHost::onReactivated() { m_model->refreshActionPanel(); }

void QmlManageFallbackViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlManageFallbackViewHost::listModel() const { return m_model; }

void QmlManageFallbackViewHost::reload() {
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

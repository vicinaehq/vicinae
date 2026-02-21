#include "raycast-store-view-host.hpp"
#include "raycast-store-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <chrono>

RaycastStoreViewHost::RaycastStoreViewHost() {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(std::chrono::milliseconds(200));

  connect(&m_debounce, &QTimer::timeout, this, &RaycastStoreViewHost::handleDebounce);
  connect(&m_listResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
          &RaycastStoreViewHost::handleFinishedPage);
  connect(&m_queryResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
          &RaycastStoreViewHost::handleFinishedQuery);
}

QUrl RaycastStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreListingView.qml"));
}

QVariantMap RaycastStoreViewHost::qmlProperties() const {
  return {{QStringLiteral("host"),
           QVariant::fromValue(const_cast<RaycastStoreViewHost *>(this))}};
}

void RaycastStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new RaycastStoreModel(this);
  m_model->initialize(context());
  m_store = context()->services->raycastStore();

  setSearchPlaceholderText("Browse Raycast extensions");

  connect(context()->services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &RaycastStoreViewHost::refresh);
}

void RaycastStoreViewHost::loadInitialData() { fetchExtensions(); }

void RaycastStoreViewHost::textChanged(const QString &text) {
  if (text.isEmpty()) {
    fetchExtensions();
    return;
  }
  m_debounce.start();
}

void RaycastStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *RaycastStoreViewHost::listModel() const { return m_model; }

void RaycastStoreViewHost::fetchExtensions() {
  setLoading(true);
  m_listResultWatcher.setFuture(m_store->fetchExtensions());
}

void RaycastStoreViewHost::handleFinishedPage() {
  if (!searchText().isEmpty()) return;

  auto result = m_listResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);
  m_model->setEntries(result->m_extensions, context()->services->extensionRegistry(),
                      QStringLiteral("Extensions"));
}

void RaycastStoreViewHost::handleFinishedQuery() {
  if (searchText() != m_lastQueryText) return;

  auto result = m_queryResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to search extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);
  m_model->setEntries(result->m_extensions, context()->services->extensionRegistry(),
                      QStringLiteral("Results"));
}

void RaycastStoreViewHost::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  m_lastQueryText = searchText();
  m_queryResultWatcher.setFuture(m_store->search(m_lastQueryText));
}

void RaycastStoreViewHost::refresh() { textChanged(searchText()); }

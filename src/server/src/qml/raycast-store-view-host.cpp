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
  connect(&m_compatResultWatcher, &QFutureWatcher<Raycast::CompatResult>::finished, this,
          &RaycastStoreViewHost::handleFinishedCompat);
}

QUrl RaycastStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreListingView.qml"));
}

QVariantMap RaycastStoreViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void RaycastStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new RaycastStoreModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();
  m_store = context()->services->raycastStore();

  setSearchPlaceholderText("Browse Raycast extensions");

  connect(context()->services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &RaycastStoreViewHost::refresh);
}

void RaycastStoreViewHost::loadInitialData() {
  fetchExtensions();
  if (!m_compatReady) { m_compatResultWatcher.setFuture(m_store->fetchCompat()); }
}

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
    qWarning() << "[RaycastStore] fetch error:" << result.error();
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  m_pendingPage = std::move(*result);
  tryPopulateModel();
}

void RaycastStoreViewHost::handleFinishedCompat() {
  m_compatReady = true;
  tryPopulateModel();
}

void RaycastStoreViewHost::tryPopulateModel() {
  if (!m_compatReady || !m_pendingPage) return;

  setLoading(false);
  m_model->setEntries(m_pendingPage->extensions, context()->services->extensionRegistry(),
                      m_store->compatMap(), QStringLiteral("Extensions"));
  m_pendingPage.reset();
}

void RaycastStoreViewHost::handleFinishedQuery() {
  if (searchText() != m_lastQueryText) return;

  auto result = m_queryResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to search extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);
  m_model->setEntries(result->extensions, context()->services->extensionRegistry(), m_store->compatMap(),
                      QStringLiteral("Results"));
}

void RaycastStoreViewHost::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  m_lastQueryText = searchText();
  m_queryResultWatcher.setFuture(m_store->search(m_lastQueryText));
}

void RaycastStoreViewHost::refresh() { textChanged(searchText()); }

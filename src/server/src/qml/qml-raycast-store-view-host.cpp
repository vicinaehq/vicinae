#include "qml-raycast-store-view-host.hpp"
#include "qml-raycast-store-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <chrono>

QmlRaycastStoreViewHost::QmlRaycastStoreViewHost() {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(std::chrono::milliseconds(200));

  connect(&m_debounce, &QTimer::timeout, this, &QmlRaycastStoreViewHost::handleDebounce);
  connect(&m_listResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
          &QmlRaycastStoreViewHost::handleFinishedPage);
  connect(&m_queryResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
          &QmlRaycastStoreViewHost::handleFinishedQuery);
}

QUrl QmlRaycastStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreListingView.qml"));
}

QVariantMap QmlRaycastStoreViewHost::qmlProperties() const {
  return {{QStringLiteral("host"),
           QVariant::fromValue(const_cast<QmlRaycastStoreViewHost *>(this))}};
}

void QmlRaycastStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlRaycastStoreModel(this);
  m_model->initialize(context());
  m_store = context()->services->raycastStore();

  setSearchPlaceholderText("Browse Raycast extensions");

  connect(context()->services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &QmlRaycastStoreViewHost::refresh);
}

void QmlRaycastStoreViewHost::loadInitialData() { fetchExtensions(); }

void QmlRaycastStoreViewHost::textChanged(const QString &text) {
  if (text.isEmpty()) {
    fetchExtensions();
    return;
  }
  m_debounce.start();
}

void QmlRaycastStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlRaycastStoreViewHost::listModel() const { return m_model; }

void QmlRaycastStoreViewHost::fetchExtensions() {
  setLoading(true);
  m_listResultWatcher.setFuture(m_store->fetchExtensions());
}

void QmlRaycastStoreViewHost::handleFinishedPage() {
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

void QmlRaycastStoreViewHost::handleFinishedQuery() {
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

void QmlRaycastStoreViewHost::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  m_lastQueryText = searchText();
  m_queryResultWatcher.setFuture(m_store->search(m_lastQueryText));
}

void QmlRaycastStoreViewHost::refresh() { textChanged(searchText()); }

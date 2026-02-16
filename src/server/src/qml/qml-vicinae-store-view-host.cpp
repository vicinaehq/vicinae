#include "qml-vicinae-store-view-host.hpp"
#include "qml-vicinae-store-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <chrono>

QmlVicinaeStoreViewHost::QmlVicinaeStoreViewHost() {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(std::chrono::milliseconds(200));

  connect(&m_debounce, &QTimer::timeout, this, &QmlVicinaeStoreViewHost::handleDebounce);
  connect(&m_listResultWatcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &QmlVicinaeStoreViewHost::handleFinishedPage);
  connect(&m_queryResultWatcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &QmlVicinaeStoreViewHost::handleFinishedQuery);
}

QUrl QmlVicinaeStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreListingView.qml"));
}

QVariantMap QmlVicinaeStoreViewHost::qmlProperties() const {
  return {{QStringLiteral("host"),
           QVariant::fromValue(const_cast<QmlVicinaeStoreViewHost *>(this))}};
}

void QmlVicinaeStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlVicinaeStoreModel(this);
  m_model->initialize(context());
  m_store = context()->services->vicinaeStore();

  setSearchPlaceholderText("Browse Vicinae extensions");

  connect(context()->services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &QmlVicinaeStoreViewHost::refresh);
}

void QmlVicinaeStoreViewHost::loadInitialData() { fetchExtensions(); }

void QmlVicinaeStoreViewHost::textChanged(const QString &text) {
  if (text.isEmpty()) {
    fetchExtensions();
    return;
  }
  m_debounce.start();
}

void QmlVicinaeStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlVicinaeStoreViewHost::listModel() const { return m_model; }

void QmlVicinaeStoreViewHost::fetchExtensions() {
  setLoading(true);
  m_listResultWatcher.setFuture(m_store->fetchExtensions());
}

void QmlVicinaeStoreViewHost::handleFinishedPage() {
  if (!searchText().isEmpty()) return;

  auto result = m_listResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  // Convert QList to std::vector
  std::vector<VicinaeStore::Extension> extensions(result->extensions.begin(),
                                                  result->extensions.end());
  m_model->setEntries(extensions, context()->services->extensionRegistry(),
                      QStringLiteral("Extensions"));
}

void QmlVicinaeStoreViewHost::handleFinishedQuery() {
  if (searchText() != m_lastQueryText) return;

  auto result = m_queryResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to search extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  std::vector<VicinaeStore::Extension> extensions(result->extensions.begin(),
                                                  result->extensions.end());
  m_model->setEntries(extensions, context()->services->extensionRegistry(),
                      QStringLiteral("Results"));
}

void QmlVicinaeStoreViewHost::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  m_lastQueryText = searchText();
  m_queryResultWatcher.setFuture(m_store->search(m_lastQueryText));
}

void QmlVicinaeStoreViewHost::refresh() { textChanged(searchText()); }

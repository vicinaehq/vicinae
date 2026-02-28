#include "vicinae-store-view-host.hpp"
#include "vicinae-store-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <chrono>

VicinaeStoreViewHost::VicinaeStoreViewHost() {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(std::chrono::milliseconds(200));

  connect(&m_debounce, &QTimer::timeout, this, &VicinaeStoreViewHost::handleDebounce);
  connect(&m_listResultWatcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &VicinaeStoreViewHost::handleFinishedPage);
  connect(&m_queryResultWatcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &VicinaeStoreViewHost::handleFinishedQuery);
}

QUrl VicinaeStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreListingView.qml"));
}

QVariantMap VicinaeStoreViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void VicinaeStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new VicinaeStoreModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();
  m_store = context()->services->vicinaeStore();

  setSearchPlaceholderText("Browse Vicinae extensions");

  connect(context()->services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &VicinaeStoreViewHost::refresh);
}

void VicinaeStoreViewHost::loadInitialData() { fetchExtensions(); }

void VicinaeStoreViewHost::textChanged(const QString &text) {
  if (text.isEmpty()) {
    fetchExtensions();
    return;
  }
  m_debounce.start();
}

void VicinaeStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *VicinaeStoreViewHost::listModel() const { return m_model; }

void VicinaeStoreViewHost::fetchExtensions() {
  setLoading(true);
  m_listResultWatcher.setFuture(m_store->fetchExtensions());
}

void VicinaeStoreViewHost::handleFinishedPage() {
  if (!searchText().isEmpty()) return;

  auto result = m_listResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  std::vector<VicinaeStore::Extension> const extensions(result->extensions.begin(), result->extensions.end());
  m_model->setEntries(extensions, context()->services->extensionRegistry(), QStringLiteral("Extensions"));
}

void VicinaeStoreViewHost::handleFinishedQuery() {
  if (searchText() != m_lastQueryText) return;

  auto result = m_queryResultWatcher.result();
  if (!result) {
    context()->services->toastService()->setToast("Failed to search extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  std::vector<VicinaeStore::Extension> const extensions(result->extensions.begin(), result->extensions.end());
  m_model->setEntries(extensions, context()->services->extensionRegistry(), QStringLiteral("Results"));
}

void VicinaeStoreViewHost::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  m_lastQueryText = searchText();
  m_queryResultWatcher.setFuture(m_store->search(m_lastQueryText));
}

void VicinaeStoreViewHost::refresh() { textChanged(searchText()); }

#include "vicinae-store-view-host.hpp"
#include "vicinae-store-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"

VicinaeStoreViewHost::VicinaeStoreViewHost() {
  connect(&m_watcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &VicinaeStoreViewHost::handleFinished);
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

void VicinaeStoreViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void VicinaeStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *VicinaeStoreViewHost::listModel() const { return m_model; }

void VicinaeStoreViewHost::fetchExtensions() {
  setLoading(true);
  m_watcher.setFuture(m_store->fetchAll());
}

void VicinaeStoreViewHost::handleFinished() {
  auto result = m_watcher.result();
  setLoading(false);

  if (!result) {
    qWarning() << "[VicinaeStore] fetch error:" << result.error();
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  m_model->setEntries(result->extensions, context()->services->extensionRegistry(),
                      QStringLiteral("Extensions"));
}

void VicinaeStoreViewHost::refresh() {
  if (auto *cached = m_store->cached()) {
    m_model->setEntries(*cached, context()->services->extensionRegistry(), QStringLiteral("Extensions"));
  } else {
    fetchExtensions();
  }
}

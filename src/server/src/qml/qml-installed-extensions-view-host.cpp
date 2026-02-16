#include "qml-installed-extensions-view-host.hpp"
#include "qml-installed-extensions-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"

QUrl QmlInstalledExtensionsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlInstalledExtensionsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlInstalledExtensionsViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlInstalledExtensionsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search extensions...");

  auto registry = ServiceRegistry::instance()->extensionRegistry();
  connect(registry, &ExtensionRegistry::extensionsChanged, this,
          &QmlInstalledExtensionsViewHost::reload);
}

void QmlInstalledExtensionsViewHost::loadInitialData() { reload(); }

void QmlInstalledExtensionsViewHost::textChanged(const QString &text) {
  m_model->setFilter(text);
}

void QmlInstalledExtensionsViewHost::onReactivated() {
  m_model->refreshActionPanel();
}

void QmlInstalledExtensionsViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlInstalledExtensionsViewHost::listModel() const { return m_model; }

void QmlInstalledExtensionsViewHost::reload() {
  auto registry = ServiceRegistry::instance()->extensionRegistry();
  m_model->setItems(registry->scanAll());
}

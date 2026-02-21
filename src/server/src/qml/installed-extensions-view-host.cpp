#include "installed-extensions-view-host.hpp"
#include "installed-extensions-model.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"

QUrl InstalledExtensionsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap InstalledExtensionsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void InstalledExtensionsViewHost::initialize() {
  BaseView::initialize();

  m_model = new InstalledExtensionsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search extensions...");

  auto registry = ServiceRegistry::instance()->extensionRegistry();
  connect(registry, &ExtensionRegistry::extensionsChanged, this,
          &InstalledExtensionsViewHost::reload);
}

void InstalledExtensionsViewHost::loadInitialData() { reload(); }

void InstalledExtensionsViewHost::textChanged(const QString &text) {
  m_model->setFilter(text);
}

void InstalledExtensionsViewHost::onReactivated() {
  m_model->refreshActionPanel();
}

void InstalledExtensionsViewHost::beforePop() { m_model->beforePop(); }

QObject *InstalledExtensionsViewHost::listModel() const { return m_model; }

void InstalledExtensionsViewHost::reload() {
  auto registry = ServiceRegistry::instance()->extensionRegistry();
  m_model->setItems(registry->scanAll());
}

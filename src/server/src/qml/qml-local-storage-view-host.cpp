#include "qml-local-storage-view-host.hpp"
#include "qml-local-storage-model.hpp"
#include "service-registry.hpp"

QUrl QmlLocalStorageViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlLocalStorageViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlLocalStorageViewHost::initialize() {
  BaseView::initialize();
  m_model = new QmlLocalStorageNamespaceModel(this);
  m_model->initialize(context());
  setSearchPlaceholderText("Search namespaces...");
}

void QmlLocalStorageViewHost::loadInitialData() {
  m_model->setItems(context()->services->localStorage()->namespaces());
}

void QmlLocalStorageViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void QmlLocalStorageViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlLocalStorageViewHost::listModel() const { return m_model; }

QmlLocalStorageItemViewHost::QmlLocalStorageItemViewHost(const QString &ns, std::vector<QString> keys)
    : m_ns(ns), m_keys(std::move(keys)) {}

QUrl QmlLocalStorageItemViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlLocalStorageItemViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlLocalStorageItemViewHost::initialize() {
  BaseView::initialize();
  m_model = new QmlLocalStorageItemModel(this);
  m_model->initialize(context());
  m_model->setNamespace(m_ns);
  setSearchPlaceholderText("Search items...");
}

void QmlLocalStorageItemViewHost::loadInitialData() { m_model->setItems(std::move(m_keys)); }

void QmlLocalStorageItemViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void QmlLocalStorageItemViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlLocalStorageItemViewHost::listModel() const { return m_model; }

#include "local-storage-view-host.hpp"
#include "local-storage-model.hpp"
#include "service-registry.hpp"

QUrl LocalStorageViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap LocalStorageViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void LocalStorageViewHost::initialize() {
  BaseView::initialize();
  m_model = new LocalStorageNamespaceModel(this);
  m_model->initialize(context());
  setSearchPlaceholderText("Search namespaces...");
}

void LocalStorageViewHost::loadInitialData() {
  m_model->setItems(context()->services->localStorage()->namespaces());
}

void LocalStorageViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void LocalStorageViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *LocalStorageViewHost::listModel() const { return m_model; }

LocalStorageItemViewHost::LocalStorageItemViewHost(const QString &ns, std::vector<QString> keys)
    : m_ns(ns), m_keys(std::move(keys)) {}

QUrl LocalStorageItemViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap LocalStorageItemViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void LocalStorageItemViewHost::initialize() {
  BaseView::initialize();
  m_model = new LocalStorageItemModel(this);
  m_model->initialize(context());
  m_model->setNamespace(m_ns);
  setSearchPlaceholderText("Search items...");
}

void LocalStorageItemViewHost::loadInitialData() { m_model->setItems(std::move(m_keys)); }

void LocalStorageItemViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void LocalStorageItemViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *LocalStorageItemViewHost::listModel() const { return m_model; }

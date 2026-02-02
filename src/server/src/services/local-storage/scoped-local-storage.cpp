#include "scoped-local-storage.hpp"
#include <qjsondocument.h>

ScopedLocalStorage::ScopedLocalStorage(LocalStorageService &storage, const QString &ns)
    : m_storage(storage), m_namespace(ns) {}

bool ScopedLocalStorage::removeItem(const QString &key) { return m_storage.removeItem(m_namespace, key); }

bool ScopedLocalStorage::setItem(const QString &key, const QJsonValue &json) {
  return m_storage.setItem(m_namespace, key, json);
}

QJsonValue ScopedLocalStorage::getItem(const QString &key) { return m_storage.getItem(m_namespace, key); }

QJsonDocument ScopedLocalStorage::getItemAsJson(const QString &key) {
  return m_storage.getItemAsJson(m_namespace, key);
}

void ScopedLocalStorage::setItemAsJson(const QString &key, const QJsonDocument &doc) {
  m_storage.setItemAsJson(m_namespace, key, doc);
}

bool ScopedLocalStorage::clear() { return m_storage.clearNamespace(m_namespace); }

QJsonObject ScopedLocalStorage::list() { return m_storage.listNamespaceItems(m_namespace); }

const QString &ScopedLocalStorage::scope() const { return m_namespace; }

#include <qjsondocument.h>
#include <qjsonobject.h>
#include "local-storage-service.hpp"
#include "scoped-local-storage.hpp"
#include "omni-database.hpp"

using ValueType = LocalStorageService::ValueType;

std::pair<QString, ValueType> LocalStorageService::serializeValue(const QJsonValue &value) const {
  if (value.isString()) { return {value.toString(), ValueType::String}; }
  if (value.isDouble()) { return {QString::number(value.toDouble()), ValueType::Number}; }
  if (value.isBool()) { return {value.toBool() ? "1" : "0", ValueType::Boolean}; }

  return {"", ValueType::String};
}

QJsonValue LocalStorageService::deserializeValue(const QString &value, ValueType type) {
  switch (type) {
  case ValueType::String:
    return value;
  case ValueType::Boolean:
    return value == "1";
  case ValueType::Number:
    return value.toDouble();
  }

  return {};
}

QJsonDocument LocalStorageService::getItemAsJson(const QString &namespaceId, const QString &key) {
  QJsonValue const json = getItem(namespaceId, key);

  if (!json.isString()) return {};

  return QJsonDocument::fromJson(json.toString().toUtf8());
}

void LocalStorageService::setItemAsJson(const QString &namespaceId, const QString &key,
                                        const QJsonDocument &json) {
  setItem(namespaceId, key, QString::fromUtf8(json.toJson(QJsonDocument::JsonFormat::Compact)));
}

std::vector<QString> LocalStorageService::namespaces() const {
  std::vector<QString> ss;
  auto stmt = m_omniDb.db().prepare("SELECT DISTINCT(namespace_id) FROM storage_data_item");

  while (stmt.step()) {
    ss.emplace_back(stmt.columnQString(0));
  }

  return ss;
}

QJsonValue LocalStorageService::getItem(const QString &namespaceId, const QString &key) {
  m_getQuery.reset();
  m_getQuery.bind(":namespace_id", namespaceId);
  m_getQuery.bind(":key", key);

  if (!m_getQuery.step()) return {};

  QString const value = m_getQuery.columnQString(0);
  ValueType const valueType = static_cast<ValueType>(m_getQuery.columnInt(1));

  return deserializeValue(value, valueType);
}

bool LocalStorageService::setItem(const QString &namespaceId, const QString &key, const QJsonValue &json) {
  auto [value, valueType] = serializeValue(json);

  m_setItemQuery.reset();
  m_setItemQuery.bind(":namespace_id", namespaceId);
  m_setItemQuery.bind(":key", key);
  m_setItemQuery.bind(":value", value);
  m_setItemQuery.bind(":value_type", static_cast<int>(valueType));

  if (!m_setItemQuery.exec()) {
    qCritical() << "LocalStorageService::setItem: failed to execute query"
                << m_setItemQuery.lastError().c_str();
    return false;
  }

  return true;
}

bool LocalStorageService::removeItem(const QString &namespaceId, const QString &key) {
  m_removeQuery.reset();
  m_removeQuery.bind(":namespace_id", namespaceId);
  m_removeQuery.bind(":key", key);

  if (!m_removeQuery.exec()) {
    qCritical() << "LocalStorageService::removeItem: failed to execute query"
                << m_removeQuery.lastError().c_str();
    return false;
  }

  return m_omniDb.db().changes() != 0;
}

QJsonObject LocalStorageService::listNamespaceItems(const QString &namespaceId) {
  m_listQuery.reset();
  m_listQuery.bind(":namespace_id", namespaceId);

  QJsonObject obj;

  while (m_listQuery.step()) {
    auto key = m_listQuery.columnQString(0);
    auto value = m_listQuery.columnQString(1);
    auto valueType = static_cast<ValueType>(m_listQuery.columnInt(2));
    obj[key] = deserializeValue(value, valueType);
  }

  return obj;
}

bool LocalStorageService::clearNamespace(const QString &namespaceId) {
  m_clearQuery.reset();
  m_clearQuery.bind(":namespace_id", namespaceId);

  if (!m_clearQuery.exec()) {
    qCritical() << "LocalStorageService::clearNamespace: failed to execute query"
                << m_clearQuery.lastError().c_str();
    return false;
  }

  return true;
}

LocalStorageService::LocalStorageService(OmniDatabase &db) : m_omniDb(db) {
  m_clearQuery = db.db().prepare("DELETE FROM storage_data_item WHERE namespace_id = :namespace_id");
  m_listQuery = db.db().prepare(
      "SELECT key, value, value_type FROM storage_data_item WHERE namespace_id = :namespace_id");
  m_removeQuery =
      db.db().prepare("DELETE FROM storage_data_item WHERE namespace_id = :namespace_id AND key = :key");
  m_setItemQuery = db.db().prepare(R"(
    INSERT INTO storage_data_item (namespace_id, key, value, value_type)
    VALUES (:namespace_id, :key, :value, :value_type)
    ON CONFLICT (namespace_id, key) DO UPDATE SET value = :value, value_type = :value_type
  )");
  m_getQuery = db.db().prepare(
      "SELECT value, value_type FROM storage_data_item WHERE namespace_id = :namespace_id AND key = :key");
}

ScopedLocalStorage LocalStorageService::scoped(const QString &scope) {
  return ScopedLocalStorage(*this, scope);
}

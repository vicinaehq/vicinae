#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qsqlquery.h>
#include <qvariant.h>
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
  QJsonValue json = getItem(namespaceId, key);

  if (!json.isString()) return {};

  return QJsonDocument::fromJson(json.toString().toUtf8());
}

void LocalStorageService::setItemAsJson(const QString &namespaceId, const QString &key,
                                        const QJsonDocument &json) {
  setItem(namespaceId, key, QString::fromUtf8(json.toJson(QJsonDocument::JsonFormat::Compact)));
}

std::vector<QString> LocalStorageService::namespaces() const {
  std::vector<QString> ss;
  auto query = db.createQuery();
  query.exec("SELECT DISTINCT(namespace_id) FROM storage_data_item");

  while (query.next()) {
    ss.emplace_back(query.value(0).toString());
  }

  return ss;
}

QJsonValue LocalStorageService::getItem(const QString &namespaceId, const QString &key) {
  m_getQuery.bindValue(":namespace_id", namespaceId);
  m_getQuery.bindValue(":key", key);

  if (!m_getQuery.exec()) {
    qCritical() << "LocalStorageService::getItem: failed to execute query" << m_getQuery.lastError();
    return {};
  }

  if (!m_getQuery.next()) return {};

  QString value = m_getQuery.value(0).toString();
  ValueType valueType = static_cast<ValueType>(m_getQuery.value(1).toUInt());

  return deserializeValue(value, valueType);
}

bool LocalStorageService::setItem(const QString &namespaceId, const QString &key, const QJsonValue &json) {
  auto [value, valueType] = serializeValue(json);

  m_setItemQuery.bindValue(":namespace_id", namespaceId);
  m_setItemQuery.bindValue(":key", key);
  m_setItemQuery.bindValue(":value", value);
  m_setItemQuery.bindValue(":value_type", valueType);

  if (!m_setItemQuery.exec()) {
    qCritical() << "LocalStorageService::setItem: failed to execute query" << m_setItemQuery.lastError();
    return false;
  }

  return true;
}

bool LocalStorageService::removeItem(const QString &namespaceId, const QString &key) {
  m_removeQuery.bindValue(":namespace_id", namespaceId);
  m_removeQuery.bindValue(":key", key);

  if (!m_removeQuery.exec()) {
    qCritical() << "LocalStorageService::removeItem: failed to execute query" << m_removeQuery.lastError();
    return false;
  }

  return m_removeQuery.numRowsAffected() != 0;
}

QJsonObject LocalStorageService::listNamespaceItems(const QString &namespaceId) {
  m_listQuery.bindValue(":namespace_id", namespaceId);

  if (!m_listQuery.exec()) {
    qCritical() << "LocalStorageService::listNamespaceItems: failed to execute query"
                << m_listQuery.lastError();
    return {};
  }

  QJsonObject obj;

  while (m_listQuery.next()) {
    auto key = m_listQuery.value(0).toString();
    auto value = m_listQuery.value(1);

    obj[key] = QJsonValue::fromVariant(value);
  }

  return obj;
}

bool LocalStorageService::clearNamespace(const QString &namespaceId) {
  m_clearQuery.bindValue(":namespace_id", namespaceId);

  if (!m_clearQuery.exec()) {
    qCritical() << "LocalStorageService::clearNamespace: failed to execute query" << m_clearQuery.lastError();
    return false;
  }

  return true;
}

LocalStorageService::LocalStorageService(OmniDatabase &db) : db(db) {
  m_clearQuery = db.createQuery();
  m_listQuery = db.createQuery();
  m_removeQuery = db.createQuery();
  m_setItemQuery = db.createQuery();
  m_getQuery = db.createQuery();

  m_clearQuery.prepare("DELETE FROM storage_data_item WHERE namespace_id = :namespace_id");
  m_listQuery.prepare("SELECT key, value FROM storage_data_item WHERE namespace_id = :namespace_id");
  m_removeQuery.prepare("DELETE FROM storage_data_item WHERE namespace_id = :namespace_id AND key = :key");
  m_setItemQuery.prepare(R"(
		INSERT INTO storage_data_item (namespace_id, key, value, value_type)
		VALUES (:namespace_id, :key, :value, :value_type)
		ON CONFLICT (namespace_id, key) DO UPDATE SET value = :value, value_type = :value_type
	)");
  m_getQuery.prepare(
      "SELECT value, value_type FROM storage_data_item WHERE namespace_id = :namespace_id AND key = :key");
}

ScopedLocalStorage LocalStorageService::scoped(const QString &scope) {
  return ScopedLocalStorage(*this, scope);
}

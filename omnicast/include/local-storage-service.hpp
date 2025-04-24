#pragma once
#include "omni-database.hpp"
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qvariant.h>

class LocalStorageService {
  OmniDatabase &db;
  QSqlQuery m_clearQuery = db.createQuery();
  QSqlQuery m_listQuery = db.createQuery();
  QSqlQuery m_removeQuery = db.createQuery();
  QSqlQuery m_setItemQuery = db.createQuery();
  QSqlQuery m_getQuery = db.createQuery();

  enum ValueType { Number, String, Boolean };

  void createTables() {
    auto query = db.createQuery();

    if (!query.exec(R"(
	  	CREATE TABLE IF NOT EXISTS storage_data_item (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			namespace_id TEXT NOT NULL,
			value_type INT NOT NULL,
			key TEXT NOT NULL,
			value TEXT NOT NULL,
			UNIQUE(namespace_id, key)
		);
	  )")) {
      qCritical() << "Failed to initialize storage table";
    }
  }

  std::pair<QString, ValueType> serializeValue(const QJsonValue &value) {
    if (value.isString()) { return {value.toString(), ValueType::String}; }
    if (value.isDouble()) { return {QString::number(value.toDouble()), ValueType::Number}; }
    if (value.isBool()) { return {value.toBool() ? "1" : "0", ValueType::Boolean}; }

    return {"", ValueType::String};
  }

  QJsonValue deserializeValue(const QString &value, ValueType type) {
    switch (type) {
    case ValueType::String:
      return value;
    case ValueType::Boolean:
      return value == "1";
    case ValueType::Number:
      return value.toDouble();
    }
  }

public:
  LocalStorageService(OmniDatabase &db) : db(db) {
    createTables();
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

  bool clearNamespace(const QString &namespaceId) {
    m_clearQuery.bindValue(":namespace_id", namespaceId);

    if (!m_clearQuery.exec()) {
      qCritical() << "LocalStorageService::clearNamespace: failed to execute query"
                  << m_clearQuery.lastError();
      return false;
    }

    return true;
  }

  QJsonObject listNamespaceItems(const QString &namespaceId) {
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

  bool removeItem(const QString &namespaceId, const QString &key) {
    m_removeQuery.bindValue(":namespace_id", namespaceId);
    m_removeQuery.bindValue(":key", key);

    if (!m_removeQuery.exec()) {
      qCritical() << "LocalStorageService::removeItem: failed to execute query" << m_removeQuery.lastError();
      return false;
    }

    return m_removeQuery.numRowsAffected() != 0;
  }

  bool setItem(const QString &namespaceId, const QString &key, const QJsonValue &json) {
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

  QJsonValue getItem(const QString &namespaceId, const QString &key) {
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
};

#pragma once
#include <qjsondocument.h>
#include <qjsonobject.h>

#include "db/database.hpp"

class OmniDatabase;
class ScopedLocalStorage;

class LocalStorageService {
public:
  enum ValueType { Number, String, Boolean };

  LocalStorageService(OmniDatabase &db);

  bool clearNamespace(const QString &namespaceId);
  QJsonObject listNamespaceItems(const QString &namespaceId);
  std::vector<QString> namespaces() const;
  bool removeItem(const QString &namespaceId, const QString &key);
  bool setItem(const QString &namespaceId, const QString &key, const QJsonValue &json);
  QJsonValue getItem(const QString &namespaceId, const QString &key);
  QJsonDocument getItemAsJson(const QString &namespaceId, const QString &key);
  void setItemAsJson(const QString &namespaceId, const QString &key, const QJsonDocument &json);
  ScopedLocalStorage scoped(const QString &scope);

private:
  OmniDatabase &m_omniDb;
  db::Statement m_clearQuery;
  db::Statement m_listQuery;
  db::Statement m_removeQuery;
  db::Statement m_setItemQuery;
  db::Statement m_getQuery;

  std::pair<QString, ValueType> serializeValue(const QJsonValue &value) const;

  QJsonValue deserializeValue(const QString &value, ValueType type);
};

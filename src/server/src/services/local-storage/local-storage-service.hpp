#pragma once
#include <qjsondocument.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qjsonobject.h>

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
  OmniDatabase &db;
  QSqlQuery m_clearQuery;
  QSqlQuery m_listQuery;
  QSqlQuery m_removeQuery;
  QSqlQuery m_setItemQuery;
  QSqlQuery m_getQuery;

  std::pair<QString, ValueType> serializeValue(const QJsonValue &value) const;

  QJsonValue deserializeValue(const QString &value, ValueType type);
};

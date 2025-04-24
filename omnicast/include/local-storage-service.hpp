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
public:
  enum ValueType { Number, String, Boolean };

private:
  OmniDatabase &db;
  QSqlQuery m_clearQuery = db.createQuery();
  QSqlQuery m_listQuery = db.createQuery();
  QSqlQuery m_removeQuery = db.createQuery();
  QSqlQuery m_setItemQuery = db.createQuery();
  QSqlQuery m_getQuery = db.createQuery();

  void createTables();

  std::pair<QString, ValueType> serializeValue(const QJsonValue &value) const;

  QJsonValue deserializeValue(const QString &value, ValueType type);

public:
  bool clearNamespace(const QString &namespaceId);
  QJsonObject listNamespaceItems(const QString &namespaceId);
  bool removeItem(const QString &namespaceId, const QString &key);
  bool setItem(const QString &namespaceId, const QString &key, const QJsonValue &json);
  QJsonValue getItem(const QString &namespaceId, const QString &key);

  LocalStorageService(OmniDatabase &db);
};

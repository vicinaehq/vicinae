#pragma once
#include "local-storage-service.hpp"
#include <qjsondocument.h>

/**
 * Wrapper around LocalStorage with a predefined namespace.
 */
class ScopedLocalStorage {
public:
  ScopedLocalStorage(LocalStorageService &storage, const QString &ns);

  bool removeItem(const QString &key);
  bool setItem(const QString &key, const QJsonValue &json);
  QJsonValue getItem(const QString &key);
  QJsonDocument getItemAsJson(const QString &key);
  void setItemAsJson(const QString &key, const QJsonDocument &doc);
  bool clear();
  QJsonObject list();

  const QString &scope() const;

private:
  LocalStorageService &m_storage;
  QString m_namespace;
};

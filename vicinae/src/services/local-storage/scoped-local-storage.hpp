#pragma once
#include "local-storage-service.hpp"

/**
 * Wrapper around LocalStorage with a predefined namespace.
 */
class ScopedLocalStorage {
public:
  ScopedLocalStorage(LocalStorageService &storage, const QString &ns);

  bool removeItem(const QString &key);
  bool setItem(const QString &key, const QJsonValue &json);
  QJsonValue getItem(const QString &key);
  QJsonObject getItemAsJson(const QString &key);
  bool clear();
  QJsonObject list();

  const QString &scope() const;

private:
  LocalStorageService &m_storage;
  QString m_namespace;
};

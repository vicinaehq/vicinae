#pragma once
#include "generated/tsapi.hpp"
#include "glaze-qt.hpp"
#include "services/local-storage/local-storage-service.hpp"

class ExtStorageService : public tsapi::AbstractStorage {
  using Result = tsapi::Result<void>;

public:
  ExtStorageService(tsapi::RpcTransport &transport, LocalStorageService &storage, QString namespaceId)
      : AbstractStorage(transport), m_storage(storage), m_namespaceId(std::move(namespaceId)) {}

  tsapi::Result<glz::generic>::Future get(std::string key) override {
    auto value = m_storage.getItem(m_namespaceId, QString::fromStdString(key));
    return tsapi::Result<glz::generic>::ok(qJsonValueToGlazeGeneric(value));
  }

  Result::Future set(std::string key, glz::generic value) override {
    qDebug() << "storage" << key << glazeToQJsonValue(value);
    m_storage.setItem(m_namespaceId, QString::fromStdString(key), glazeToQJsonValue(value));
    return Result::ok();
  }

  Result::Future remove(std::string key) override {
    m_storage.removeItem(m_namespaceId, QString::fromStdString(key));
    return Result::ok();
  }

  Result::Future clear() override {
    m_storage.clearNamespace(m_namespaceId);
    return Result::ok();
  }

  tsapi::Result<glz::generic>::Future list() override {
    auto items = m_storage.listNamespaceItems(m_namespaceId);
    glz::generic result = qJsonObjectToGlazeGeneric(items);
    return tsapi::Result<glz::generic>::ok(std::move(result));
  }

private:
  LocalStorageService &m_storage;
  QString m_namespaceId;
};

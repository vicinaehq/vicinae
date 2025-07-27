#pragma once
#include "proto/storage.pb.h"
#include "proto/extension.pb.h"
#include <QString>

class LocalStorageService;

class StorageRequestRouter {
  LocalStorageService *m_storage = nullptr;
  QString m_namespaceId;

  proto::ext::extension::Response *makeErrorResponse(const QString &errorText) {
    auto res = new proto::ext::extension::Response;
    auto err = new proto::ext::common::ErrorResponse;

    err->set_error_text(errorText.toStdString());
    res->set_allocated_error(err);

    return res;
  }

  proto::ext::storage::GetResponse *handleGetStorage(const proto::ext::storage::GetRequest &req);
  proto::ext::storage::SetResponse *handleSetStorage(const proto::ext::storage::SetRequest &req);
  proto::ext::storage::ClearResponse *handleClearStorage(const proto::ext::storage::ClearRequest &req);
  proto::ext::storage::RemoveResponse *handleRemoveStorage(const proto::ext::storage::RemoveRequest &req);
  proto::ext::storage::ListResponse *handleListStorage(const proto::ext::storage::ListRequest &req);

public:
  StorageRequestRouter(LocalStorageService *storage, const QString &namespaceId)
      : m_storage(storage), m_namespaceId(namespaceId) {}

  proto::ext::extension::Response *route(const proto::ext::storage::Request &req);
};

#include "services/local-storage/local-storage-service.hpp"
#include "storage-request-router.hpp"
#include "utils/utils.hpp"

namespace storage = proto::ext::storage;

proto::ext::storage::GetResponse *
StorageRequestRouter::handleGetStorage(const proto::ext::storage::GetRequest &req) {
  auto res = new proto::ext::storage::GetResponse;
  QJsonValue value = m_storage->getItem(m_namespaceId, QString::fromStdString(req.key()));

  res->set_allocated_value(new google::protobuf::Value(transformJsonValueToProto(value)));

  return res;
}

storage::SetResponse *StorageRequestRouter::handleSetStorage(const storage::SetRequest &req) {
  auto res = new storage::SetResponse;
  auto jsonValue = protoToJsonValue(req.value());

  m_storage->setItem(m_namespaceId, QString::fromStdString(req.key()), jsonValue);

  return res;
}

storage::ClearResponse *StorageRequestRouter::handleClearStorage(const storage::ClearRequest &req) {
  auto res = new storage::ClearResponse;

  m_storage->clearNamespace(m_namespaceId);

  return res;
}

storage::RemoveResponse *StorageRequestRouter::handleRemoveStorage(const storage::RemoveRequest &req) {
  auto res = new storage::RemoveResponse;

  m_storage->removeItem(m_namespaceId, QString::fromStdString(req.key()));

  return res;
}

storage::ListResponse *StorageRequestRouter::handleListStorage(const storage::ListRequest &req) {
  auto res = new storage::ListResponse;
  auto values = res->mutable_values();
  auto jsonValues = m_storage->listNamespaceItems(m_namespaceId);

  for (const auto &key : jsonValues.keys()) {
    auto value = transformJsonValueToProto(jsonValues.value(key));
    values->insert({key.toStdString(), value});
  }

  return res;
}

proto::ext::extension::Response *StorageRequestRouter::route(const storage::Request &req) {
  namespace storage = storage;
  auto storageRes = new storage::Response();

  switch (req.payload_case()) {
  case storage::Request::kGet:
    storageRes->set_allocated_get(handleGetStorage(req.get()));
    break;
  case storage::Request::kSet:
    storageRes->set_allocated_set(handleSetStorage(req.set()));
    break;
  case storage::Request::kRemove:
    storageRes->set_allocated_remove(handleRemoveStorage(req.remove()));
    break;
  case storage::Request::kClear:
    storageRes->set_allocated_clear(handleClearStorage(req.clear()));
    break;
  case storage::Request::kList:
    storageRes->set_allocated_list(handleListStorage(req.list()));
    break;
  default: {
    delete storageRes;
    return makeErrorResponse("Unhandled storage response");
  }
  }

  auto response = new proto::ext::extension::Response;
  auto data = new proto::ext::extension::ResponseData;

  data->set_allocated_storage(storageRes);
  response->set_allocated_data(data);

  return response;
}

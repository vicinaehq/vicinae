#include "json-patch.hpp"
#include <qhash.h>

static QHash<QString, JsonPatch::OperationType> stringToOp{{"add", JsonPatch::OperationType::Add},
                                                           {"remove", JsonPatch::OperationType::Remove},
                                                           {"replace", JsonPatch::OperationType::Replace}};

JsonPatch::OperationType JsonPatch::parseOperationType(const QString &op) {
  return stringToOp.value(op, JsonPatch::OperationType::None);
}

JsonPatch::JsonPatch(const QJsonArray &data) {
  for (const auto &row : data) {
    auto obj = row.toObject();
    auto path = obj.value("path").toString();
    auto op = parseOperationType(obj.value("op").toString());

    keyToOp.insert(path, op);
  }
}

JsonPatch::OperationType JsonPatch::getOperationType(const QString &path) const {
  return keyToOp.value(path, OperationType::None);
}

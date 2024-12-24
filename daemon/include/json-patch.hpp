#pragma once
#include <qhash.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <ranges>

class JsonPatch {
public:
  enum OperationType { None, Add, Replace, Remove };

private:
  QHash<QString, OperationType> keyToOp;

  OperationType parseOperationType(const QString &);

public:
  JsonPatch(const QJsonArray &data);

  OperationType getOperationType(const QString &path) const;

  bool isSubtreeChanged(const QString &path) const {
    for (const auto key : keyToOp.keys()) {
      if (key.size() > path.size() && key.startsWith(path))
        return true;
    }

    return false;
  }
};

#pragma once
#include <QObject>

namespace qt_utils {
inline size_t countQObjectChildren(QObject *obj) {
  size_t count = 0;

  for (const auto &child : obj->children()) {
    count = count + countQObjectChildren(child) + 1;
  }

  return count;
}
}; // namespace qt_utils

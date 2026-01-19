#pragma once
#include <QObject>

struct QObjectDeleter {
  void operator()(QObject *obj) { obj->deleteLater(); }
};

template <typename T = QObject> using QObjectUniquePtr = std::unique_ptr<T, QObjectDeleter>;

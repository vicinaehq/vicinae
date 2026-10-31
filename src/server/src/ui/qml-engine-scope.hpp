#pragma once
#include <QCoreApplication>
#include <QHash>
#include <QJSEngine>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>
#include <type_traits>

// Backs the QML singletons: process-wide bridges come from global<T>(), per-window objects are
// registered by their window before the engine loads and looked up per engine in get<T>().
class QmlEngineScope {
public:
  template <typename T> static void set(QQmlEngine *engine, T *object) {
    map()[engine][&T::staticMetaObject] = object;
  }

  template <typename T> static T *get(QQmlEngine *engine) {
    static_assert(!std::is_default_constructible_v<T>,
                  "Qt would default-construct the singleton instead of calling create()");
    auto *object = static_cast<T *>(map().value(engine).value(&T::staticMetaObject));
    if (object) QJSEngine::setObjectOwnership(object, QJSEngine::CppOwnership);
    return object;
  }

  template <typename T> static T *global() {
    static_assert(!std::is_default_constructible_v<T>,
                  "Qt would default-construct the singleton instead of calling create()");
    static T *object = [] {
      auto *created = new T(QCoreApplication::instance());
      QJSEngine::setObjectOwnership(created, QJSEngine::CppOwnership);
      return created;
    }();
    return object;
  }

private:
  static QHash<QQmlEngine *, QHash<const QMetaObject *, QObject *>> &map() {
    static QHash<QQmlEngine *, QHash<const QMetaObject *, QObject *>> instance;
    return instance;
  }
};

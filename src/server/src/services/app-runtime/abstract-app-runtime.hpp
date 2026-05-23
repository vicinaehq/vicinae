#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include <qobject.h>
#include <qtmetamacros.h>

class AbstractAppRuntime : public QObject {
  Q_OBJECT

public:
  virtual ~AbstractAppRuntime() = default;

  virtual bool isRunning(const AbstractApplication &app) const = 0;

signals:
  void runningAppsChanged();
};

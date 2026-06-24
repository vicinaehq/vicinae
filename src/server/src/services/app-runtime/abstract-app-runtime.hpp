#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>

class AbstractAppRuntime : public QObject {
  Q_OBJECT

public:
  virtual ~AbstractAppRuntime() = default;

  virtual bool isRunning(const AbstractApplication &app) const = 0;
  virtual std::shared_ptr<AbstractApplication> frontmostApp() const = 0;
  virtual bool activate(const AbstractApplication &app) const = 0;
  virtual bool quit(const AbstractApplication &app) const = 0;
  virtual bool forceQuit(const AbstractApplication &app) const = 0;

signals:
  void runningAppsChanged();
  void frontmostAppChanged();
};

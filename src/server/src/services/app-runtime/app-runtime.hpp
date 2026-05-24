#pragma once
#include "abstract-app-runtime.hpp"
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>

class WindowManager;

class AppService;

class AppRuntime : public QObject {
  Q_OBJECT

signals:
  void runningAppsChanged();
  void frontmostAppChanged();

public:
  AppRuntime(WindowManager &wm, AppService &appService);

  bool isRunning(const AbstractApplication &app) const;
  std::shared_ptr<AbstractApplication> frontmostApp() const;
  bool activate(const AbstractApplication &app) const;

private:
  static std::unique_ptr<AbstractAppRuntime> createProvider(WindowManager &wm, AppService &appService);

  std::unique_ptr<AbstractAppRuntime> m_provider;
};

#pragma once
#include "abstract-app-runtime.hpp"
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>

class WindowManager;

class AppRuntime : public QObject {
  Q_OBJECT

signals:
  void runningAppsChanged();

public:
  explicit AppRuntime(WindowManager &wm);

  bool isRunning(const AbstractApplication &app) const;

private:
  static std::unique_ptr<AbstractAppRuntime> createProvider(WindowManager &wm);

  std::unique_ptr<AbstractAppRuntime> m_provider;
};

#pragma once
#include "services/app-runtime/abstract-app-runtime.hpp"

class WindowManager;
class AppService;

class LinuxAppRuntime : public AbstractAppRuntime {
  Q_OBJECT

public:
  LinuxAppRuntime(WindowManager &wm, AppService &appService);

  bool isRunning(const AbstractApplication &app) const override;
  std::shared_ptr<AbstractApplication> frontmostApp() const override;
  bool activate(const AbstractApplication &app) const override;

private:
  WindowManager &m_wm;
  AppService &m_appService;
};

#pragma once
#include "services/app-runtime/abstract-app-runtime.hpp"

class WindowManager;

class LinuxAppRuntime : public AbstractAppRuntime {
  Q_OBJECT

public:
  explicit LinuxAppRuntime(WindowManager &wm);

  bool isRunning(const AbstractApplication &app) const override;

private:
  WindowManager &m_wm;
};

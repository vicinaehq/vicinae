#pragma once
#include "services/app-runtime/abstract-app-runtime.hpp"

#ifdef __OBJC__
@class MacAppRuntimeObserver;
#else
class MacAppRuntimeObserver;
#endif

class MacAppRuntime : public AbstractAppRuntime {
  Q_OBJECT

public:
  MacAppRuntime();
  ~MacAppRuntime() override;

  bool isRunning(const AbstractApplication &app) const override;

private:
  MacAppRuntimeObserver *m_observer;
};

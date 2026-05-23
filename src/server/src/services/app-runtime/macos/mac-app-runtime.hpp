#pragma once
#include "services/app-runtime/abstract-app-runtime.hpp"
#include <unordered_set>

class AppService;

#ifdef __OBJC__
@class MacAppRuntimeObserver;
#else
class MacAppRuntimeObserver;
#endif

class MacAppRuntime : public AbstractAppRuntime {
  Q_OBJECT

public:
  explicit MacAppRuntime(AppService &appService);
  ~MacAppRuntime() override;

  bool isRunning(const AbstractApplication &app) const override;
  std::shared_ptr<AbstractApplication> frontmostApp() const override;
  bool activate(const AbstractApplication &app) const override;

private:
  void refreshRunningCache();

  AppService &m_appService;
  std::unordered_set<QString> m_runningIds;
  MacAppRuntimeObserver *m_observer;
};

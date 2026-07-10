#pragma once
#include "abstract-app-runtime.hpp"

class DummyAppRuntime : public AbstractAppRuntime {
public:
  bool isRunning(const AbstractApplication &app) const override { return false; }
  std::shared_ptr<AbstractApplication> frontmostApp() const override { return nullptr; }
  bool activate(const AbstractApplication &app) const override { return false; }
  bool quit(const AbstractApplication &app) const override { return false; }
  bool forceQuit(const AbstractApplication &app) const override { return false; }
};

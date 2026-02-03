#include "abstract-window-manager.hpp"

class DummyWindowManager : public AbstractWindowManager {
  QString id() const override { return "dummy"; }
  QString displayName() const override { return "Dummy (No window manager available)"; }
  bool isActivatable() const override { return false; }
  void start() override {}
  bool ping() const override { return false; }
};

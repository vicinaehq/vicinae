#include "wm/window-manager.hpp"

class DummyWindowManager : public AbstractWindowManager {
  QString name() const override { return "Dummy (No window manager available)"; }
  bool isActivatable() const override { return false; }
  void start() const override {}
  bool ping() const override { return false; }
};

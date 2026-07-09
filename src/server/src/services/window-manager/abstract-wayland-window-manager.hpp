#pragma once
#include "services/window-manager/abstract-window-manager.hpp"

class AbstractWaylandWindowManager : public AbstractWindowManager {
public:
  std::vector<Screen> listScreensSync() const override;
};

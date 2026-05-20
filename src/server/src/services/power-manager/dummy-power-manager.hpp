#pragma once
#include "services/power-manager/abstract-power-manager.hpp"

class DummyPowerManager : public AbstractPowerManager {
public:
  QString id() const override { return "dummy"; }
};

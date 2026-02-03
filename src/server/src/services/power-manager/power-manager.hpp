#pragma once
#include <memory>
#include "services/power-manager/abstract-power-manager.hpp"
#include "services/power-manager/systemd/systemd-power-manager.hpp"

class PowerManager {
public:
  AbstractPowerManager *provider() const { return m_manager.get(); }
  PowerManager() { m_manager = std::make_unique<SystemdPowerManager>(); }

private:
  std::unique_ptr<AbstractPowerManager> m_manager;
};

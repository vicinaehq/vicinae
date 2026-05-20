#pragma once
#include <memory>
#include "services/power-manager/abstract-power-manager.hpp"
#ifdef Q_OS_LINUX
#include "services/power-manager/systemd/systemd-power-manager.hpp"
#else
#include "services/power-manager/dummy-power-manager.hpp"
#endif

class PowerManager {
public:
  AbstractPowerManager *provider() const { return m_manager.get(); }
  PowerManager() {
#ifdef Q_OS_LINUX
    m_manager = std::make_unique<SystemdPowerManager>();
#else
    m_manager = std::make_unique<DummyPowerManager>();
#endif
  }

private:
  std::unique_ptr<AbstractPowerManager> m_manager;
};

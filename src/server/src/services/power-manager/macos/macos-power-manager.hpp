#pragma once
#include "../abstract-power-manager.hpp"

class MacosPowerManager : public AbstractPowerManager {
public:
  bool powerOff() override;
  bool reboot() override;
  bool sleep() const override;
  bool lock() override;
  bool logout() override;

  bool canPowerOff() const override;
  bool canReboot() const override;
  bool canSleep() const override;
  bool canLock() const override;
  bool canLogOut() const override;

  QString id() const override;
};

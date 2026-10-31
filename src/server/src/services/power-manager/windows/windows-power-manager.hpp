#pragma once
#include "../abstract-power-manager.hpp"

class WindowsPowerManager : public AbstractPowerManager {
public:
  bool powerOff() override;
  bool reboot() override;
  bool sleep() const override;
  bool suspend() override;
  bool hibernate() override;
  bool lock() override;
  bool logout() override;

  bool canPowerOff() const override;
  bool canReboot() const override;
  bool canSleep() const override;
  bool canSuspend() const override;
  bool canHibernate() const override;
  bool canLock() const override;
  bool canLogOut() const override;

  QString id() const override;
};

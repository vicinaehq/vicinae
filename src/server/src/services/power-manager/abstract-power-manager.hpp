#pragma once
#include <QString>

class AbstractPowerManager {
public:
  virtual bool powerOff() { return false; }
  virtual bool reboot() { return false; }
  virtual bool softReboot() { return false; }
  virtual bool sleep() const { return false; }
  virtual bool suspend() { return false; }
  virtual bool hibernate() { return false; }
  virtual bool lock() { return false; }
  virtual bool logout() { return false; }

  virtual bool canPowerOff() const { return false; }
  virtual bool canReboot() const { return false; }
  virtual bool canSoftReboot() const { return false; }
  virtual bool canSuspend() const { return false; }
  virtual bool canHibernate() const { return false; }
  virtual bool canLock() const { return false; }
  virtual bool canSleep() const { return false; }
  virtual bool canLogOut() const { return false; }

  virtual QString id() const = 0;

  virtual ~AbstractPowerManager() = default;
};

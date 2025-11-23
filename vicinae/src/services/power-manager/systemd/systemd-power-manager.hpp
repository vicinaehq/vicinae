#pragma once
#include <qdbusargument.h>
#include <qdbusinterface.h>
#include <qdbusmessage.h>
#include <qdbusmetatype.h>
#include <unistd.h>
#include <vector>
#include "../abstract-power-manager.hpp"

/**
 * Power manager implementation for systems running systemd.
 * We use the systemd-logind DBus API:
 * https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.login1.html
 */
class SystemdPowerManager : public AbstractPowerManager {
public:
  struct Session {
    QString id;
    int uid;
    QString user;
    QString seatId;
    QString path;
  };

  using SessionList = std::vector<Session>;

  bool powerOff() override;
  bool reboot() override;
  bool sleep() const override;
  bool suspend() override;
  bool hibernate() override;
  bool lock() override;
  bool logout() override;
  bool softReboot() override;

  bool canPowerOff() const override;
  bool canSuspend() const override;
  bool canSleep() const override;
  bool canHibernate() const override;
  bool canReboot() const override;
  bool canSoftReboot() const override;
  bool canLock() const override;
  bool canLogOut() const override;

  std::optional<uint32_t> currentSessionId() const;
  QString id() const override;

  QDBusMessage logindCall(const QString &method) const { return createInterface()->call(method); };

  template <typename... Args> QDBusMessage logindCall(const QString &method, Args &&...args) const {
    return createInterface()->call(method, args...);
  }

  SystemdPowerManager();

private:
  std::unique_ptr<QDBusInterface> createInterface() const {
    return std::make_unique<QDBusInterface>(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,
                                            QDBusConnection::systemBus());
  }

  static constexpr const char *DBUS_SERVICE = "org.freedesktop.login1";
  static constexpr const char *DBUS_PATH = "/org/freedesktop/login1";
  static constexpr const char *DBUS_INTERFACE = "org.freedesktop.login1.Manager";

  bool can(const QString &method) const;
  std::optional<uint> getUserSessionId() const;

  std::optional<uint32_t> m_sessionId;
};

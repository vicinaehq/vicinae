#pragma once
#include "../abstract-desktop-notification-client.hpp"
#include <QDBusInterface>
#include <memory>

class FreedesktopNotificationClient : public AbstractDesktopNotificationClient {
public:
  FreedesktopNotificationClient();
  bool send(const Notification &notification) override;

private:
  static constexpr const char *DBUS_SERVICE = "org.freedesktop.Notifications";
  static constexpr const char *DBUS_PATH = "/org/freedesktop/Notifications";
  static constexpr const char *DBUS_INTERFACE = "org.freedesktop.Notifications";

  static uchar mapUrgency(Urgency urgency);

  std::unique_ptr<QDBusInterface> m_iface;
};

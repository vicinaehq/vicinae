#pragma once
#include "../abstract-desktop-notification-client.hpp"

class FreedesktopNotificationClient : public AbstractDesktopNotificationClient {
public:
  bool send(const Notification &notification) override;

private:
  static constexpr const char *DBUS_SERVICE = "org.freedesktop.Notifications";
  static constexpr const char *DBUS_PATH = "/org/freedesktop/Notifications";
  static constexpr const char *DBUS_INTERFACE = "org.freedesktop.Notifications";

  static uchar mapUrgency(Urgency urgency);
};

#pragma once
#include "../abstract-desktop-notification-client.hpp"

class WindowsNotificationClient : public AbstractDesktopNotificationClient {
public:
  // unpackaged apps must declare their AUMID in the registry or the shell silently drops their toasts
  static void registerAppIdentity();

  bool send(const Notification &notification) override;
};

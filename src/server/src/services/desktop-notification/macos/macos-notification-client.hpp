#pragma once
#include "../abstract-desktop-notification-client.hpp"

class MacosNotificationClient : public AbstractDesktopNotificationClient {
public:
  bool send(const Notification &notification) override;
};

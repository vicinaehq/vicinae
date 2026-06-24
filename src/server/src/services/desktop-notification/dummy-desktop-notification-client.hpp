#pragma once
#include "abstract-desktop-notification-client.hpp"

class DummyDesktopNotificationClient : public AbstractDesktopNotificationClient {
public:
  bool send(const Notification &) override { return false; }
};

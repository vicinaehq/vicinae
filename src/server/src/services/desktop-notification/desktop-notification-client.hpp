#pragma once
#include "abstract-desktop-notification-client.hpp"
#include <memory>
#ifdef Q_OS_LINUX
#include "freedesktop/freedesktop-notification-client.hpp"
#else
#include "dummy-desktop-notification-client.hpp"
#endif

class DesktopNotificationClient {
public:
  AbstractDesktopNotificationClient *provider() const { return m_client.get(); }
  DesktopNotificationClient() {
#ifdef Q_OS_LINUX
    m_client = std::make_unique<FreedesktopNotificationClient>();
#else
    m_client = std::make_unique<DummyDesktopNotificationClient>();
#endif
  }

private:
  std::unique_ptr<AbstractDesktopNotificationClient> m_client;
};

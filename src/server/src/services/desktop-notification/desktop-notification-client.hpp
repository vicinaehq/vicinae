#pragma once
#include "abstract-desktop-notification-client.hpp"
#include <memory>
#if defined(Q_OS_LINUX)
#include "freedesktop/freedesktop-notification-client.hpp"
#elif defined(Q_OS_MACOS)
#include "macos/macos-notification-client.hpp"
#elif defined(Q_OS_WIN)
#include "windows/windows-notification-client.hpp"
#else
#include "dummy-desktop-notification-client.hpp"
#endif

class DesktopNotificationClient {
public:
  AbstractDesktopNotificationClient *provider() const { return m_client.get(); }
  DesktopNotificationClient() {
#if defined(Q_OS_LINUX)
    m_client = std::make_unique<FreedesktopNotificationClient>();
#elif defined(Q_OS_MACOS)
    m_client = std::make_unique<MacosNotificationClient>();
#elif defined(Q_OS_WIN)
    m_client = std::make_unique<WindowsNotificationClient>();
#else
    m_client = std::make_unique<DummyDesktopNotificationClient>();
#endif
  }

private:
  std::unique_ptr<AbstractDesktopNotificationClient> m_client;
};

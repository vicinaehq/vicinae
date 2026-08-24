#pragma once
#include <memory>
#include "services/status-notifier/abstract-tray-service.hpp"
#ifdef Q_OS_LINUX
#include "services/status-notifier/sni/sni-tray-service.hpp"
#else
#include "services/status-notifier/dummy-tray-service.hpp"
#endif

class SystemTrayService {
public:
  AbstractTrayService *provider() const { return m_service.get(); }
  SystemTrayService() {
#ifdef Q_OS_LINUX
    m_service = std::make_unique<SniTrayService>();
#else
    m_service = std::make_unique<DummyTrayService>();
#endif
  }

private:
  std::unique_ptr<AbstractTrayService> m_service;
};

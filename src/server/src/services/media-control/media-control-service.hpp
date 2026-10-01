#pragma once
#include <memory>
#include "services/media-control/abstract-media-control.hpp"
#ifdef Q_OS_LINUX
#include "services/media-control/mpris/mpris-media-control.hpp"
#elif defined(Q_OS_WIN)
#include "services/media-control/windows/windows-media-control.hpp"
#else
#include "services/media-control/dummy-media-control.hpp"
#endif

class MediaControlService {
public:
  AbstractMediaControl *provider() const { return m_backend.get(); }

  MediaControlService() {
#ifdef Q_OS_LINUX
    m_backend = std::make_unique<MprisMediaControl>();
#elif defined(Q_OS_WIN)
    m_backend = std::make_unique<WindowsMediaControl>();
#else
    m_backend = std::make_unique<DummyMediaControl>();
#endif
  }

private:
  std::unique_ptr<AbstractMediaControl> m_backend;
};

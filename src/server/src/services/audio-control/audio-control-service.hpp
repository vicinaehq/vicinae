#pragma once
#include <memory>
#include "services/audio-control/abstract-audio-control.hpp"
#ifdef Q_OS_LINUX
#include "services/audio-control/pactl/pactl-audio-control.hpp"
#else
#include "services/audio-control/dummy-audio-control.hpp"
#endif

class AudioControlService {
public:
  AbstractAudioControl *provider() const { return m_backend.get(); }
  AudioControlService() {
#ifdef Q_OS_LINUX
    m_backend = std::make_unique<PactlAudioControl>();
#else
    m_backend = std::make_unique<DummyAudioControl>();
#endif
  }

private:
  std::unique_ptr<AbstractAudioControl> m_backend;
};

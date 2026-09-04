#pragma once
#include <memory>
#include "services/audio-control/abstract-audio-control.hpp"
#ifdef Q_OS_LINUX
#include "services/audio-control/pactl/pactl-audio-control.hpp"
#elif defined(Q_OS_MACOS)
#include "services/audio-control/macos/coreaudio-audio-control.hpp"
#elif defined(Q_OS_WIN)
#include "services/audio-control/windows/wasapi-audio-control.hpp"
#else
#include "services/audio-control/dummy-audio-control.hpp"
#endif

class AudioControlService {
public:
  AbstractAudioControl *provider() const { return m_backend.get(); }
  AudioControlService() {
#ifdef Q_OS_LINUX
    m_backend = std::make_unique<PactlAudioControl>();
#elif defined(Q_OS_MACOS)
    m_backend = std::make_unique<CoreAudioControl>();
#elif defined(Q_OS_WIN)
    m_backend = std::make_unique<WasapiAudioControl>();
#else
    m_backend = std::make_unique<DummyAudioControl>();
#endif
  }

private:
  std::unique_ptr<AbstractAudioControl> m_backend;
};

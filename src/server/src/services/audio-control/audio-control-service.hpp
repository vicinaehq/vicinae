#pragma once
#include <memory>
#include "services/audio-control/abstract-audio-control.hpp"
#include "services/audio-control/pactl/pactl-audio-control.hpp"

class AudioControlService {
public:
  AbstractAudioControl *provider() const { return m_backend.get(); }
  AudioControlService() { m_backend = std::make_unique<PactlAudioControl>(); }

private:
  std::unique_ptr<AbstractAudioControl> m_backend;
};

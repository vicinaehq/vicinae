#pragma once
#include <memory>
#include "services/audio-control/abstract-audio-control.hpp"
#include "services/audio-control/wpctl/wpctl-audio-control.hpp"

class AudioControlService {
public:
  AbstractAudioControl *provider() const { return m_backend.get(); }
  AudioControlService() { m_backend = std::make_unique<WpctlAudioControl>(); }

private:
  std::unique_ptr<AbstractAudioControl> m_backend;
};

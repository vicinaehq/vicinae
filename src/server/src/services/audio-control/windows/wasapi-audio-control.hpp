#pragma once
#include "services/audio-control/abstract-audio-control.hpp"

class WasapiAudioControl : public AbstractAudioControl {
public:
  QString id() const override;

  float getVolume() const override;
  std::optional<float> setVolume(float level) override;
  std::optional<float> adjustVolume(float delta) override;

  bool isMuted() const override;
  bool setMuted(bool muted) override;
  bool toggleMute() override;
};

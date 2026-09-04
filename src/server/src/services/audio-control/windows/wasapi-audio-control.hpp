#pragma once
#include "services/audio-control/abstract-audio-control.hpp"

/**
 * System audio control through WASAPI endpoint volume, acting on the default render device.
 */
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

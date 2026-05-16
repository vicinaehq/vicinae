#pragma once
#include "services/audio-control/abstract-audio-control.hpp"

class DummyAudioControl : public AbstractAudioControl {
public:
  QString id() const override { return "dummy"; }

  float getVolume() const override { return 0.0f; }
  std::optional<float> setVolume(float) override { return std::nullopt; }
  std::optional<float> adjustVolume(float) override { return std::nullopt; }

  bool isMuted() const override { return false; }
  bool setMuted(bool) override { return false; }
  bool toggleMute() override { return false; }

  std::vector<AudioSink> listSinks() const override { return {}; }
  bool setDefaultSink(const QString &) override { return false; }
};

#pragma once
#include <optional>
#include <QString>

class AbstractAudioControl {
public:
  virtual ~AbstractAudioControl() = default;
  virtual QString id() const = 0;

  virtual float getVolume() const = 0;
  virtual std::optional<float> setVolume(float level) = 0;
  virtual std::optional<float> adjustVolume(float delta) = 0;

  virtual bool isMuted() const = 0;
  virtual bool setMuted(bool muted) = 0;
  virtual bool toggleMute() = 0;
};

#pragma once
#include <optional>
#include <vector>
#include <QString>

struct AudioSink {
  QString name;
  QString description;
  std::optional<QString> activePort;
  float volume = 0.0f;
  bool muted = false;
  bool isDefault = false;
};

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

  virtual std::vector<AudioSink> listSinks() const = 0;
  virtual bool setDefaultSink(const QString &sinkName) = 0;
};

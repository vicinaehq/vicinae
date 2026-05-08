#pragma once
#include <optional>
#include <QString>
#include "../abstract-audio-control.hpp"

class WpctlAudioControl : public AbstractAudioControl {
public:
  WpctlAudioControl();

  QString id() const override;

  float getVolume() const override;
  bool setVolume(float level) override;
  bool adjustVolume(float delta) override;

  bool isMuted() const override;
  bool setMuted(bool muted) override;
  bool toggleMute() override;

  std::vector<AudioSink> listSinks() const override;
  bool setDefaultSink(const QString &sinkId) override;

private:
  struct VolumeInfo {
    float volume = 0.0f;
    bool muted = false;
  };

  std::optional<VolumeInfo> queryDefaultSinkVolume() const;
  std::optional<QString> runWpctl(const QStringList &args) const;

  QString m_wpctlPath;
};

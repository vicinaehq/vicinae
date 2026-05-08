#pragma once
#include <optional>
#include <QString>
#include "../abstract-audio-control.hpp"

class PactlAudioControl : public AbstractAudioControl {
public:
  PactlAudioControl();

  QString id() const override;

  float getVolume() const override;
  bool setVolume(float level) override;
  bool adjustVolume(float delta) override;

  bool isMuted() const override;
  bool setMuted(bool muted) override;
  bool toggleMute() override;

  std::vector<AudioSink> listSinks() const override;
  bool setDefaultSink(const QString &sinkName) override;

private:
  std::optional<AudioSink> getDefaultSink() const;
  std::optional<QString> run(const QStringList &args) const;

  QString m_pactlPath;
};

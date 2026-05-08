#pragma once
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include "../abstract-audio-control.hpp"

class PactlAudioControl : public AbstractAudioControl {
public:
  PactlAudioControl();

  QString id() const override;

  float getVolume() const override;
  std::optional<float> setVolume(float level) override;
  std::optional<float> adjustVolume(float delta) override;

  bool isMuted() const override;
  bool setMuted(bool muted) override;
  bool toggleMute() override;

  std::vector<AudioSink> listSinks() const override;
  bool setDefaultSink(const QString &sinkName) override;

private:
  std::optional<AudioSink> getDefaultSink() const;
  std::optional<std::string> run(std::initializer_list<std::string_view> args) const;

  std::string m_pactlPath;
};

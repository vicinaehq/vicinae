#pragma once
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "../abstract-audio-control.hpp"

struct AudioSink {
  QString name;
  QString description;
  std::optional<QString> activePort;
  float volume = 0.0f;
  bool muted = false;
  bool isDefault = false;
};

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

private:
  std::vector<AudioSink> listSinks() const;
  bool setDefaultSink(const QString &sinkName);
  std::optional<AudioSink> getDefaultSink() const;
  std::optional<std::string> run(std::initializer_list<std::string_view> args) const;

  std::string m_pactlPath;
};

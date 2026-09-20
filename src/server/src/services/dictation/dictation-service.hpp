#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <QObject>
#include "services/ai/ai-provider.hpp"
#include "services/dictation/dictation-history.hpp"
#include "services/dictation/dictation-vocabulary.hpp"
#include "services/dictation/transcription-session.hpp"

namespace AI {
class Service;
}
class MediaControlService;

/**
 * What the dictation extension configured, pushed through its preference hook.
 */
struct TranscriptionSettings {
  std::optional<AI::ModelRef> model;
  std::optional<std::string> language;
  bool playSoundEffects = true;
  bool pauseMedia = true;
};

/**
 * Everything dictation shares between its callers: persisted data, the configured model and sessions.
 */
class DictationService : public QObject {
  Q_OBJECT

signals:
  void settingsChanged();

public:
  enum class Readiness : std::uint8_t { NoModels, NotSelected, Ready };

  DictationService(const std::filesystem::path &dataDir, AI::Service &ai, MediaControlService &mediaControl,
                   QObject *parent = nullptr);

  DictationHistory *history() { return &m_history; }
  DictationVocabulary *vocabulary() { return &m_vocabulary; }
  AI::Service &ai() { return m_ai; }
  MediaControlService &mediaControl() { return m_mediaControl; }

  void setSettings(TranscriptionSettings settings);
  const TranscriptionSettings &settings() const { return m_settings; }

  Readiness readiness() const;
  std::optional<TranscriptionSetup> setup() const;

private:
  std::optional<AI::ProviderModel> selectedModel() const;

  AI::Service &m_ai;
  MediaControlService &m_mediaControl;
  DictationHistory m_history;
  DictationVocabulary m_vocabulary;
  TranscriptionSettings m_settings;
};

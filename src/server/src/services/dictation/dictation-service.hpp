#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <QObject>
#include <QPointer>
#include "services/ai/ai-provider.hpp"
#include "services/dictation/dictation-action.hpp"
#include "services/dictation/dictation-history.hpp"
#include "services/dictation/dictation-styles.hpp"
#include "services/dictation/dictation-vocabulary.hpp"
#include "services/dictation/transcription-session.hpp"
#include "services/paste/paste-service.hpp"

namespace AI {
class Service;
}
class DictationSession;
class MediaControlService;
class ServiceRegistry;

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

public:
  enum class Readiness : std::uint8_t { NoModels, NotSelected, Ready };

  DictationService(const std::filesystem::path &dataDir, AI::Service &ai, MediaControlService &mediaControl,
                   ServiceRegistry &services, QObject *parent = nullptr);

  DictationHistory *history() { return &m_history; }
  DictationVocabulary *vocabulary() { return &m_vocabulary; }
  DictationStyles *styles() { return &m_styles; }
  AI::Service &ai() { return m_ai; }
  MediaControlService &mediaControl() { return m_mediaControl; }

  void setSettings(TranscriptionSettings settings);

  Readiness readiness() const;
  std::optional<TranscriptionSetup> setup() const;

  /** Null when a session is already active or recording could not start. */
  DictationSession *startSession(TranscriptionSetup setup, Dictation::DictationAction action,
                                 PasteService::Target pasteTarget);
  /** The latest session, which may be past delivery and only showing its final message. */
  DictationSession *activeSession() const;
  bool hasActiveSession() const;
  bool isRecording() const;
  /** Finish the current recording and transcribe it. False when nothing is recording. */
  bool acceptSession();
  /** Discard the current dictation. False when there is none. */
  bool cancelSession();

private:
  std::optional<AI::ProviderModel> selectedModel() const;

  AI::Service &m_ai;
  MediaControlService &m_mediaControl;
  ServiceRegistry &m_services;
  DictationHistory m_history;
  DictationVocabulary m_vocabulary;
  DictationStyles m_styles;
  TranscriptionSettings m_settings;
  QPointer<DictationSession> m_activeSession;
};

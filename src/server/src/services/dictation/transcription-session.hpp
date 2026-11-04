#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <QSoundEffect>
#include <QString>
#include <QTimer>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "services/media-control/media-control-service.hpp"

class DictationService;

struct TranscriptionSetup {
  AI::ModelRef model;
  AI::TranscriptionOptions options;
  // Terms to recognize on top of the user's vocabulary.
  std::vector<std::string> extraVocabulary;
  bool playSoundEffects = true;
  bool pauseMedia = true;
};

struct Transcript {
  QString text;
  std::optional<std::string> language;
  qint64 durationMs = 0;
};

/**
 * One recording turned into text through the selected model, independent of any UI.
 * Emits exactly one of transcribed, failed or cancelled per start.
 */
class TranscriptionSession : public QObject {
  Q_OBJECT

signals:
  void audioLevelChanged();
  void elapsedTimeChanged();
  void stateChanged();
  void transcribed(const Transcript &transcript);
  void failed(const QString &message);
  void cancelled();

public:
  explicit TranscriptionSession(DictationService &dictation, QObject *parent = nullptr);

  bool start(TranscriptionSetup setup);
  void accept();
  void cancel();

  bool isActive() const { return m_recorder.state() != Audio::Recorder::State::Idle || m_transcribing; }
  bool isRecording() const {
    return m_recorder.state() == Audio::Recorder::State::Recording && !m_transcribing;
  }
  qint64 recordingMs() const { return m_recorder.elapsedMs(); }
  float audioLevel() const { return m_recorder.level(); }
  QString elapsedTime() const;
  bool transcribing() const { return m_transcribing; }
  bool captureReady() const { return m_captureReady; }

private:
  void fail(const QString &message);

  DictationService &m_dictation;
  TranscriptionSetup m_setup;
  std::vector<std::string_view> m_vocabulary;
  Audio::Recorder m_recorder;
  QTimer m_elapsedTimer;
  QTimer m_recordingLimit;
  // A cancelled transcription still answers at some point, this tells it apart from the current one.
  int m_transcriptionId = 0;
  QElapsedTimer m_transcribeTimer;
  bool m_transcribing = false;
  bool m_captureReady = false;
  qint64 m_durationMs = 0;
  std::unique_ptr<MediaControlService::TransientPauseHandle> m_pauseHandle;
  QSoundEffect m_startSound;
  QSoundEffect m_stopSound;
};

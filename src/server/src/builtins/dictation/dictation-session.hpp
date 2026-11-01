#pragma once
#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include "builtins/dictation/dictation.hpp"
#include "services/dictation/dictation-rewrite.hpp"
#include "services/paste/paste-service.hpp"
#include "services/dictation/transcription-session.hpp"

class ApplicationContext;

/**
 * One dictation from key press to pasted text, independent of what presents it. Deletes itself once finished.
 */
class DictationSession : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(DictationSession)
  QML_UNCREATABLE("")

  Q_PROPERTY(float audioLevel READ audioLevel NOTIFY audioLevelChanged)
  Q_PROPERTY(QString elapsedTime READ elapsedTime NOTIFY elapsedTimeChanged)
  Q_PROPERTY(bool transcribing READ transcribing NOTIFY stateChanged)
  Q_PROPERTY(bool showControls READ showControls NOTIFY stateChanged)
  Q_PROPERTY(QString message READ message NOTIFY stateChanged)
  Q_PROPERTY(bool rewriting READ rewriting NOTIFY stateChanged)

signals:
  void audioLevelChanged();
  void elapsedTimeChanged();
  void stateChanged();
  // The text is about to be pasted or copied.
  void delivering();
  void finished();

public:
  DictationSession(const ApplicationContext *ctx, TranscriptionSetup setup, Dictation::DictationAction action,
                   bool recordHistory, PasteService::Target pasteTarget, QObject *parent = nullptr);

  bool start();
  Q_INVOKABLE void accept() { m_session.accept(); }
  /**
   * Stop everything and deliver nothing.
   */
  Q_INVOKABLE void cancel();

  bool isActive() const { return m_session.isActive() || m_rewrite.running(); }
  bool isRecording() const { return m_session.isRecording(); }
  auto recordingMs() const { return m_session.recordingMs(); }
  float audioLevel() const { return m_session.audioLevel(); }
  QString elapsedTime() const { return m_session.elapsedTime(); }
  bool transcribing() const { return m_session.transcribing(); }
  bool showControls() const {
    return !m_session.transcribing() && !m_rewrite.running() && m_message.isEmpty();
  }
  QString message() const { return m_message; }
  bool rewriting() const { return m_rewrite.running(); }

private:
  void finish();
  void finishWithMessage(const QString &message);
  void handleTranscript(const Transcript &transcript);
  /** Where delivered text came from, when it isn't the transcript itself. */
  struct DeliverySource {
    std::optional<std::string> style;
    std::optional<std::string> snippet;
  };

  void deliver(const QString &text, const DeliverySource &source = {});

  const ApplicationContext *m_ctx;
  TranscriptionSetup m_setup;
  TranscriptionSession m_session;
  DictationRewrite m_rewrite;
  Transcript m_transcript;
  std::string m_styleName;
  Dictation::DictationAction m_action;
  PasteService::Target m_pasteTarget;
  bool m_finished = false;
  bool m_recordHistory = true;
  QString m_message;
};

#pragma once
#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include "builtins/dictation/dictation.hpp"
#include "services/dictation/dictation-rewrite.hpp"
#include "services/paste/paste-service.hpp"
#include "services/dictation/transcription-session.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"

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
  Q_PROPERTY(bool captureReady READ captureReady NOTIFY stateChanged)

public:
  Q_INVOKABLE void accept() {
    if (!m_finished) m_session.accept();
  }
  Q_INVOKABLE void cancel();

signals:
  void audioLevelChanged();
  void elapsedTimeChanged();
  void stateChanged();
  // The text is about to be pasted or copied.
  void delivering();
  void finished();

public:
  DictationSession(const ApplicationContext *ctx, TranscriptionSetup setup, Dictation::DictationAction action,
                   PasteService::Target pasteTarget, QObject *parent = nullptr);

  bool start();

  bool isActive() const { return m_session.isActive() || m_rewrite.running(); }
  bool isRecording() const { return m_session.isRecording(); }
  auto recordingMs() const { return m_session.recordingMs(); }
  float audioLevel() const { return m_session.audioLevel(); }
  QString elapsedTime() const { return m_session.elapsedTime(); }
  bool transcribing() const { return m_session.transcribing(); }
  bool showControls() const { return !m_finished && m_session.isRecording() && m_message.isEmpty(); }
  QString message() const { return m_message; }
  bool rewriting() const { return m_rewrite.running(); }
  bool captureReady() const { return m_session.captureReady(); }

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
  QString m_message;
  GlobalShortcutHandle m_cancelShortcut;
};

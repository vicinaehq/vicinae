#pragma once
#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include "builtins/dictation/dictation.hpp"
#include "services/dictation/transcription-session.hpp"

class ApplicationContext;

/**
 * One dictation from key press to pasted text, shown in the HUD. Deletes itself once finished.
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

signals:
  void audioLevelChanged();
  void elapsedTimeChanged();
  void stateChanged();
  void finished();

public:
  DictationSession(const ApplicationContext *ctx, TranscriptionSetup setup, Dictation::DictationAction action,
                   bool recordHistory, QObject *parent = nullptr);

  bool start();
  Q_INVOKABLE void accept() { m_session.accept(); }
  Q_INVOKABLE void cancel() { m_session.cancel(); }

  bool isActive() const { return m_session.isActive(); }
  bool isRecording() const { return m_session.isRecording(); }
  auto recordingMs() const { return m_session.recordingMs(); }
  float audioLevel() const { return m_session.audioLevel(); }
  QString elapsedTime() const { return m_session.elapsedTime(); }
  bool transcribing() const { return m_session.transcribing(); }
  bool showControls() const { return !m_session.transcribing() && m_message.isEmpty(); }
  QString message() const { return m_message; }

private:
  void finish();
  void finishWithMessage(const QString &message);
  void deliver(const Transcript &transcript);

  const ApplicationContext *m_ctx;
  TranscriptionSetup m_setup;
  TranscriptionSession m_session;
  Dictation::DictationAction m_action;
  bool m_recordHistory = true;
  QString m_message;
};

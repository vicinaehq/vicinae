#pragma once
#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"

class ApplicationContext;

/**
 * One dictation from key press to pasted text, independent of any view.
 * Drives the recorder, then transcription through the selected model, then the paste.
 * Deletes itself once finished.
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
  DictationSession(const ApplicationContext *ctx, AI::ModelRef model, QObject *parent = nullptr);

  bool start();
  Q_INVOKABLE void accept();
  Q_INVOKABLE void cancel();

  bool isActive() const { return m_recorder.state() != Audio::Recorder::State::Idle || m_transcribing; }
  float audioLevel() const { return m_recorder.level(); }
  QString elapsedTime() const;
  bool transcribing() const { return m_transcribing; }
  bool showControls() const { return !m_transcribing && m_message.isEmpty(); }
  QString message() const { return m_message; }

private:
  void finish();
  void finishWithMessage(const QString &message);

  const ApplicationContext *m_ctx;
  AI::ModelRef m_model;
  Audio::Recorder m_recorder;
  QTimer m_elapsedTimer;
  bool m_transcribing = false;
  QString m_message;
};

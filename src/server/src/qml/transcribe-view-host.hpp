#pragma once
#include "bridge-view.hpp"
#include <QTimer>

namespace Audio {
class Recorder;
}

namespace AI {
class Service;
}

class TranscribeViewHost : public FormViewBase {
  Q_OBJECT

  Q_PROPERTY(float audioLevel READ audioLevel NOTIFY audioLevelChanged)
  Q_PROPERTY(QString elapsedTime READ elapsedTime NOTIFY elapsedTimeChanged)
  Q_PROPERTY(bool transcribing READ transcribing NOTIFY transcribingChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

signals:
  void audioLevelChanged();
  void elapsedTimeChanged();
  void transcribingChanged();
  void errorMessageChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  QString initialNavigationTitle() const override { return QStringLiteral("Transcribe"); }

  float audioLevel() const;
  QString elapsedTime() const;
  bool transcribing() const { return m_transcribing; }
  QString errorMessage() const { return m_errorMessage; }

private:
  void stopAndTranscribe();
  void togglePause();
  void updateActions();
  void updateNavigationTitle();

  Audio::Recorder *m_recorder = nullptr;
  AI::Service *m_aiService = nullptr;
  QTimer m_elapsedTimer;
  bool m_transcribing = false;
  QString m_errorMessage;
};

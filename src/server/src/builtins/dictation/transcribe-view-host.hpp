#pragma once
#include <QPointer>
#include <QtQml/qqmlregistration.h>
#include "services/dictation/dictation-session.hpp"
#include "ui/views/bridge-view.hpp"

/**
 * Presents a dictation in the launcher, where the HUD is not available.
 */
class TranscribeViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(TranscribeViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(float audioLevel READ audioLevel NOTIFY audioLevelChanged)
  Q_PROPERTY(QString elapsedTime READ elapsedTime NOTIFY elapsedTimeChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY stateChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY stateChanged)

signals:
  void audioLevelChanged();
  void elapsedTimeChanged();
  void stateChanged();

public:
  explicit TranscribeViewHost(DictationSession *session);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void beforePop() override;
  QString initialNavigationTitle() const override;
  ImageURL initialNavigationIcon() const override;

  float audioLevel() const { return m_session ? m_session->audioLevel() : 0.0f; }
  QString elapsedTime() const { return m_session ? m_session->elapsedTime() : QString(); }
  bool busy() const { return !m_session || m_session->transcribing() || m_session->rewriting(); }
  QString errorMessage() const { return m_session ? m_session->message() : QString(); }

private:
  void refresh();
  void leave();

  QPointer<DictationSession> m_session;
};

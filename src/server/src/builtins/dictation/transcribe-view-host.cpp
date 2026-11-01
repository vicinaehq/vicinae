#include "transcribe-view-host.hpp"
#include <memory>
#include <QUrl>
#include "builtins/dictation/dictation.hpp"
#include "navigation-controller.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "view-utils.hpp"

TranscribeViewHost::TranscribeViewHost(DictationSession *session) : m_session(session) {}

QUrl TranscribeViewHost::qmlComponentUrl() const { return qml::componentUrl(u"TranscribeView"); }

QVariantMap TranscribeViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

ImageURL TranscribeViewHost::initialNavigationIcon() const { return Dictation::ICON; }

QString TranscribeViewHost::initialNavigationTitle() const { return tr("Recording"); }

void TranscribeViewHost::initialize() {
  BaseView::initialize();

  connect(m_session, &DictationSession::audioLevelChanged, this, &TranscribeViewHost::audioLevelChanged);
  connect(m_session, &DictationSession::elapsedTimeChanged, this, &TranscribeViewHost::elapsedTimeChanged);
  connect(m_session, &DictationSession::stateChanged, this, &TranscribeViewHost::refresh);

  // the launcher has to give focus back for the text to reach the app
  connect(m_session, &DictationSession::delivering, this, [this] {
    auto *navigation = context()->navigation.get();
    leave();
    navigation->closeWindow();
  });
  connect(m_session, &DictationSession::finished, this, &TranscribeViewHost::leave, Qt::QueuedConnection);

  refresh();
}

void TranscribeViewHost::beforePop() {
  if (auto *session = m_session.data()) {
    m_session = nullptr;
    disconnect(session, nullptr, this, nullptr);
    session->abandon();
  }
}

void TranscribeViewHost::refresh() {
  emit stateChanged();
  if (!m_session) return;

  auto panel = std::make_unique<FormActionPanelState>();
  auto *section = panel->createSection();

  if (m_session->rewriting()) {
    setNavigationTitle(tr("Rewriting"));
  } else if (m_session->transcribing()) {
    setNavigationTitle(tr("Transcribing"));
  } else if (m_session->isRecording()) {
    setNavigationTitle(tr("Recording"));
    section->addAction(new StaticAction(tr("Stop & Transcribe"), ImageURL::builtin(BuiltinIcon::Stop),
                                        [session = m_session]() {
                                          if (session) session->accept();
                                        }));
  }

  setActions(std::move(panel));
}

void TranscribeViewHost::leave() {
  // the session goes on without this view once it is delivering
  if (m_session) disconnect(m_session, nullptr, this, nullptr);
  m_session = nullptr;
  emit stateChanged();

  auto *navigation = context()->navigation.get();
  const auto *top = navigation->topState();
  if (top && top->sender == this) navigation->goBack();
}

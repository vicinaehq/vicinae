#include "dictation-session.hpp"
#include "common/context.hpp"
#include "service-registry.hpp"
#include "navigation-controller.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/image/url.hpp"

namespace {
constexpr int MESSAGE_DURATION_MS = 1500;
}

DictationSession::DictationSession(const ApplicationContext *ctx, TranscriptionSetup setup,
                                   Dictation::DictationAction action, bool recordHistory, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_setup(std::move(setup)), m_session(*ctx->services->dictation(), this),
      m_action(action), m_recordHistory(recordHistory) {
  connect(&m_session, &TranscriptionSession::audioLevelChanged, this, &DictationSession::audioLevelChanged);
  connect(&m_session, &TranscriptionSession::elapsedTimeChanged, this, &DictationSession::elapsedTimeChanged);
  connect(&m_session, &TranscriptionSession::stateChanged, this, &DictationSession::stateChanged);
  connect(&m_session, &TranscriptionSession::transcribed, this, &DictationSession::deliver);
  connect(&m_session, &TranscriptionSession::failed, this, &DictationSession::finishWithMessage);
  connect(&m_session, &TranscriptionSession::cancelled, this, &DictationSession::finish);
}

bool DictationSession::start() {
  if (!m_session.start(m_setup)) {
    m_ctx->navigation->showHud(tr("Could not start recording"),
                               ImageURL::builtin(BuiltinIcon::MicrophoneDisabled));
    finish();
    return false;
  }
  m_ctx->navigation->showDictationHud(this);
  return true;
}

void DictationSession::deliver(const Transcript &transcript) {
  auto content = Clipboard::Text{transcript.text};

  switch (m_action) {
  case Dictation::DictationAction::PasteToActiveWindow:
    m_ctx->services->pasteService()->pasteContent(content, {.transient = true});
    break;
  case Dictation::DictationAction::CopyToClipboard:
    m_ctx->services->clipman()->copyContent(content, {.concealed = true});
    break;
  }

  if (m_recordHistory) {
    m_ctx->services->dictation()->history()->add({
        .text = transcript.text.toStdString(),
        .durationMs = static_cast<std::uint64_t>(transcript.durationMs),
        .language = transcript.language,
    });
  }

  finish();
}

void DictationSession::finish() {
  emit finished();
  deleteLater();
}

void DictationSession::finishWithMessage(const QString &message) {
  m_message = message;
  emit stateChanged();
  QTimer::singleShot(MESSAGE_DURATION_MS, this, &DictationSession::finish);
}

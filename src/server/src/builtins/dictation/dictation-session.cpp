#include "dictation-session.hpp"
#include <utility>
#include "common/context.hpp"
#include "service-registry.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/image/url.hpp"

namespace {
constexpr int MESSAGE_DURATION_MS = 1500;
}

DictationSession::DictationSession(const ApplicationContext *ctx, TranscriptionSetup setup,
                                   Dictation::DictationAction action, bool recordHistory,
                                   PasteService::Target pasteTarget, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_setup(std::move(setup)), m_session(*ctx->services->dictation(), this),
      m_rewrite(ctx->services->dictation()->ai(), *ctx->services->dictation()->vocabulary(), this),
      m_action(action), m_pasteTarget(pasteTarget), m_recordHistory(recordHistory) {
  connect(&m_session, &TranscriptionSession::audioLevelChanged, this, &DictationSession::audioLevelChanged);
  connect(&m_session, &TranscriptionSession::elapsedTimeChanged, this, &DictationSession::elapsedTimeChanged);
  connect(&m_session, &TranscriptionSession::stateChanged, this, &DictationSession::stateChanged);
  connect(&m_session, &TranscriptionSession::transcribed, this, &DictationSession::handleTranscript);
  connect(&m_session, &TranscriptionSession::failed, this, &DictationSession::finishWithMessage);
  connect(&m_session, &TranscriptionSession::cancelled, this, &DictationSession::finish);
  connect(&m_rewrite, &DictationRewrite::finished, this, [this](const QString &text) {
    deliver(text, /*rewritten*/ true);
    finish();
  });
  connect(&m_rewrite, &DictationRewrite::failed, this, [this](const QString &reason) {
    qWarning() << "Dictation: rewrite failed:" << reason;
    deliver(m_transcript.text, /*rewritten*/ false);
    finishWithMessage(tr("Used original text"));
  });
}

bool DictationSession::start() {
  if (m_session.start(m_setup)) return true;
  finish();
  return false;
}

void DictationSession::cancel() {
  m_rewrite.abort();
  m_session.cancel();
  finish();
}

void DictationSession::handleTranscript(const Transcript &transcript) {
  if (m_finished) return;
  m_transcript = transcript;

  const auto app = m_ctx->services->appRuntime()->focusTargetApp();
  const auto appId = app ? std::optional(app->id().toStdString()) : std::nullopt;
  const auto *style = m_ctx->services->dictation()->styles()->forApp(appId);

  if (!style) {
    deliver(transcript.text, /*rewritten*/ false);
    finish();
    return;
  }

  qInfo() << "Dictation: rewriting for" << (app ? app->id() : QStringLiteral("<unknown>")) << "with style"
          << style->name;
  m_styleName = style->name;
  m_rewrite.start(*style, transcript.text);
  emit stateChanged();
}

void DictationSession::deliver(const QString &text, bool rewritten) {
  auto content = Clipboard::Text{text};

  emit delivering();

  switch (m_action) {
  case Dictation::DictationAction::PasteToActiveWindow:
    m_ctx->services->pasteService()->pasteContent(content, {.transient = true}, m_pasteTarget);
    break;
  case Dictation::DictationAction::CopyToClipboard:
    m_ctx->services->clipman()->copyContent(content, {.concealed = true});
    break;
  }

  if (m_recordHistory) {
    m_ctx->services->dictation()->history()->add({
        .text = text.toStdString(),
        .durationMs = static_cast<std::uint64_t>(m_transcript.durationMs),
        .language = m_transcript.language,
        .original = rewritten ? std::optional(m_transcript.text.toStdString()) : std::nullopt,
        .style = rewritten ? std::optional(m_styleName) : std::nullopt,
    });
  }
  emit stateChanged();
}

void DictationSession::finish() {
  if (std::exchange(m_finished, true)) return;
  emit finished();
  deleteLater();
}

void DictationSession::finishWithMessage(const QString &message) {
  m_message = message;
  emit stateChanged();
  QTimer::singleShot(MESSAGE_DURATION_MS, this, &DictationSession::finish);
}

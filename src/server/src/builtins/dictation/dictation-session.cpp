#include "dictation-session.hpp"
#include <QFile>
#include <qlogging.h>
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/image/url.hpp"

namespace {
constexpr int MESSAGE_DURATION_MS = 1500;
}

DictationSession::DictationSession(const ApplicationContext *ctx, AI::ModelRef model, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_model(std::move(model)) {
  m_elapsedTimer.setInterval(1000);
  connect(&m_elapsedTimer, &QTimer::timeout, this, &DictationSession::elapsedTimeChanged);
  connect(&m_recorder, &Audio::Recorder::levelChanged, this, &DictationSession::audioLevelChanged);
  connect(&m_recorder, &Audio::Recorder::errorOccurred, this,
          [this](const QString &message) { finishWithMessage(message); });
}

bool DictationSession::start() {
  if (!m_recorder.start()) {
    m_ctx->navigation->showHud(tr("Could not start recording"),
                               ImageURL::builtin(BuiltinIcon::MicrophoneDisabled));
    finish();
    return false;
  }

  m_elapsedTimer.start();
  emit elapsedTimeChanged();
  m_ctx->navigation->showDictationHud(this);
  return true;
}

void DictationSession::cancel() {
  if (m_transcribing) return;
  m_elapsedTimer.stop();
  m_recorder.discard();
  finish();
}

void DictationSession::accept() {
  if (m_transcribing || m_recorder.state() == Audio::Recorder::State::Idle) return;

  m_elapsedTimer.stop();
  m_recorder.stop();

  const auto path = m_recorder.outputPath();
  if (!path) {
    finishWithMessage(tr("Recording failed"));
    return;
  }

  m_transcribing = true;
  emit stateChanged();

  m_ctx->services->ai()
      ->transcribe(m_model, new QFile(QString::fromStdString(path->string())), QStringLiteral("audio/wav"))
      .then(this, [this](const AI::TranscriptionResult &result) {
        m_transcribing = false;
        emit stateChanged();

        if (!result) {
          finishWithMessage(tr("Transcription failed"));
          return;
        }
        if (result->text.empty()) {
          finishWithMessage(tr("Nothing to transcribe"));
          return;
        }

        m_ctx->services->pasteService()->pasteContent(Clipboard::Text(QString::fromStdString(result->text)),
                                                      {.transient = true});
        finish();
      });
}

QString DictationSession::elapsedTime() const {
  const auto secs = m_recorder.elapsedMs() / 1000;
  return QStringLiteral("%1:%2")
      .arg(secs / 60, 2, 10, QLatin1Char('0'))
      .arg(secs % 60, 2, 10, QLatin1Char('0'));
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

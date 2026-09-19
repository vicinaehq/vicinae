#include "dictation-session.hpp"
#include <QFile>
#include <qlogging.h>
#include <QSoundEffect>
#include "common/context.hpp"
#include "dictation-extension.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/media-control/media-control-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/image/url.hpp"

namespace {
constexpr int MESSAGE_DURATION_MS = 1500;
}

DictationSession::DictationSession(const ApplicationContext *ctx, AI::ModelRef model,
                                   AI::TranscriptionOptions options, bool playSoundEffects, bool pauseMedia,
                                   Dictation::DictationAction action, bool recordHistory, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_model(std::move(model)), m_options(std::move(options)), m_action(action),
      m_recordHistory(recordHistory), m_playSoundEffects(playSoundEffects), m_pauseMedia(pauseMedia) {
  m_elapsedTimer.setInterval(1000);

  if (m_playSoundEffects) {
    const auto prepareSoundEffect = [](QSoundEffect &effect, const QUrl &url) {
      effect.setSource(url);
      effect.setLoopCount(1);
      effect.setVolume(0.3f);
    };

    prepareSoundEffect(m_startSound, QUrl(QStringLiteral("qrc:/sound/dictation-start.wav")));
    prepareSoundEffect(m_stopSound, QUrl(QStringLiteral("qrc:/sound/dictation-stop.wav")));
  }

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

  if (m_pauseMedia) {
    m_pauseHandle = std::make_unique<MediaControlService::TransientPauseHandle>(
        m_ctx->services->mediaControl()->transientPauseAll());
  }

  m_elapsedTimer.start();
  emit elapsedTimeChanged();
  m_ctx->navigation->showDictationHud(this);

  if (m_playSoundEffects) { m_startSound.play(); }

  m_ctx->services->ai()->preloadModel(m_model);

  return true;
}

void DictationSession::cancel() {
  if (m_transcribing) return;
  m_elapsedTimer.stop();
  m_recorder.discard();
  m_ctx->services->ai()->cancelPreload(m_model);
  finish();
}

void DictationSession::accept() {
  if (m_transcribing || m_recorder.state() == Audio::Recorder::State::Idle) return;

  m_elapsedTimer.stop();
  m_durationMs = m_recorder.elapsedMs();
  m_recorder.stop();

  if (m_playSoundEffects) {
    m_startSound.stop();
    m_stopSound.play();
  }

  if (m_pauseHandle) { m_pauseHandle->resume(); }

  auto recording = m_recorder.finish();

  if (recording.toF32().empty()) {
    m_ctx->services->ai()->cancelPreload(m_model);
    finishWithMessage(tr("Nothing to transcribe"));
    return;
  }

  m_transcribing = true;
  emit stateChanged();

  auto options = m_options;
  options.vocabulary = m_ctx->services->dictation()->vocabulary()->words();

  m_ctx->services->ai()
      ->transcribe(m_model, std::move(recording), options)
      .then(this, [this](const AI::TranscriptionResult &result) {
        if (!result) {
          m_transcribing = false;
          emit stateChanged();
          finishWithMessage(tr("Transcription failed"));
          return;
        }
        if (result->text.empty()) {
          m_transcribing = false;
          emit stateChanged();
          finishWithMessage(tr("Nothing to transcribe"));
          return;
        }

        m_transcribing = false;
        emit stateChanged();
        deliver(*result);
      });
}

void DictationSession::deliver(const AI::TranscriptionResponse &response) {
  const auto text = QString::fromStdString(response.text).trimmed();
  auto content = Clipboard::Text{text};

  switch (m_action) {
  case Dictation::DictationAction::PasteToActiveWindow:
    m_ctx->services->pasteService()->pasteContent(content, {.transient = true});
    break;
  case Dictation::DictationAction::CopyToClipboard:
    m_ctx->services->clipman()->copyContent(
        content, {.concealed = true}); // will already be indexed by transcription history
    break;
  }

  if (m_recordHistory) {
    m_ctx->services->dictation()->history()->add({
        .text = text.toStdString(),
        .durationMs = static_cast<std::uint64_t>(m_durationMs),
        .language = m_options.language ? m_options.language : response.language,
    });
  }

  finish();
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

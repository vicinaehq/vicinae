#include "transcription-session.hpp"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <qlogging.h>
#include "services/ai/ai-service.hpp"
#include "services/dictation/dictation-service.hpp"

TranscriptionSession::TranscriptionSession(DictationService &dictation, QObject *parent)
    : QObject(parent), m_dictation(dictation) {
  m_elapsedTimer.setInterval(1000);

  const auto prepareSoundEffect = [](QSoundEffect &effect, const QUrl &url) {
    effect.setSource(url);
    effect.setLoopCount(1);
    effect.setVolume(0.3f);
  };
  prepareSoundEffect(m_startSound, QUrl(QStringLiteral("qrc:/sound/dictation-start.wav")));
  prepareSoundEffect(m_stopSound, QUrl(QStringLiteral("qrc:/sound/dictation-stop.wav")));

  connect(&m_elapsedTimer, &QTimer::timeout, this, &TranscriptionSession::elapsedTimeChanged);
  connect(&m_recorder, &Audio::Recorder::levelChanged, this, &TranscriptionSession::audioLevelChanged);
  connect(&m_recorder, &Audio::Recorder::stateChanged, this, &TranscriptionSession::stateChanged);
  connect(&m_recorder, &Audio::Recorder::errorOccurred, this,
          [this](const QString &message) { fail(message); });
}

bool TranscriptionSession::start(TranscriptionSetup setup) {
  if (isActive()) return false;
  m_setup = std::move(setup);

  if (!m_recorder.start()) {
    fail(tr("Could not start recording"));
    return false;
  }

  if (m_setup.pauseMedia) {
    m_pauseHandle = std::make_unique<MediaControlService::TransientPauseHandle>(
        m_dictation.mediaControl().transientPauseAll());
  }

  m_elapsedTimer.start();
  emit elapsedTimeChanged();
  if (m_setup.playSoundEffects) m_startSound.play();
  m_dictation.ai().preloadModel(m_setup.model);
  return true;
}

void TranscriptionSession::cancel() {
  if (m_transcribing || m_recorder.state() == Audio::Recorder::State::Idle) return;
  m_elapsedTimer.stop();
  m_recorder.discard();
  m_pauseHandle.reset();
  m_dictation.ai().cancelPreload(m_setup.model);
  emit stateChanged();
  emit cancelled();
}

void TranscriptionSession::accept() {
  if (m_transcribing || m_recorder.state() == Audio::Recorder::State::Idle) return;

  m_elapsedTimer.stop();
  m_durationMs = m_recorder.elapsedMs();
  m_transcribeTimer.start();
  m_recorder.stop();

  if (m_setup.playSoundEffects) {
    m_startSound.stop();
    m_stopSound.play();
  }
  m_pauseHandle.reset();

  auto recording = m_recorder.finish();
  const auto sampleCount = recording.toF32().size();
  const auto sampleRate = recording.format().sampleRate();
  qInfo() << "Dictation: recorded" << sampleCount << "samples at" << sampleRate << "Hz ("
          << static_cast<double>(sampleCount) * 1000.0 / sampleRate << "ms of audio) over" << m_durationMs
          << "ms";

  if (const auto dir = qEnvironmentVariable("VICINAE_DICTATION_DUMP_DIR"); !dir.isEmpty()) {
    const auto path = QDir(dir).filePath(
        QStringLiteral("dictation-%1.wav").arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")));
    if (QFile file(path); file.open(QIODevice::WriteOnly)) {
      file.write(recording.toWav());
      qInfo() << "Dictation: dumped recording to" << path;
    }
  }

  if (recording.toF32().empty()) {
    m_dictation.ai().cancelPreload(m_setup.model);
    fail(tr("Nothing to transcribe"));
    return;
  }

  m_transcribing = true;
  emit stateChanged();

  auto options = m_setup.options;
  options.vocabulary = m_dictation.vocabulary()->words();

  m_dictation.ai()
      .transcribe(m_setup.model, std::move(recording), options)
      .then(this, [this](const AI::TranscriptionResult &result) {
        qInfo() << "Dictation: transcribed" << m_durationMs << "ms of audio in" << m_transcribeTimer.elapsed()
                << "ms";
        m_transcribing = false;
        emit stateChanged();

        if (!result) {
          fail(tr("Transcription failed"));
          return;
        }
        const auto text = QString::fromStdString(result->text).trimmed();
        if (text.isEmpty()) {
          fail(tr("Nothing to transcribe"));
          return;
        }
        emit transcribed({
            .text = text,
            .language = m_setup.options.language ? m_setup.options.language : result->language,
            .durationMs = m_durationMs,
        });
      });
}

QString TranscriptionSession::elapsedTime() const {
  const auto secs = m_recorder.elapsedMs() / 1000;
  return QStringLiteral("%1:%2")
      .arg(secs / 60, 2, 10, QLatin1Char('0'))
      .arg(secs % 60, 2, 10, QLatin1Char('0'));
}

void TranscriptionSession::fail(const QString &message) {
  m_elapsedTimer.stop();
  if (m_recorder.state() != Audio::Recorder::State::Idle) m_recorder.discard();
  m_pauseHandle.reset();
  emit stateChanged();
  emit failed(message);
}

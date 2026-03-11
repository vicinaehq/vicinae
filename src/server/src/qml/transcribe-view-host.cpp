#include "transcribe-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/audio/audio-recorder.hpp"
#include "services/paste/paste-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

QUrl TranscribeViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/TranscribeView.qml"));
}

QVariantMap TranscribeViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void TranscribeViewHost::initialize() {
  BaseView::initialize();

  m_recorder = new Audio::Recorder(this);
  m_aiService = ServiceRegistry::instance()->ai();

  connect(m_recorder, &Audio::Recorder::levelChanged, this, &TranscribeViewHost::audioLevelChanged);
  connect(m_recorder, &Audio::Recorder::stateChanged, this, [this]() {
    updateNavigationTitle();
    updateActions();
  });
  connect(m_recorder, &Audio::Recorder::errorOccurred, this, [this](const QString &msg) {
    m_errorMessage = msg;
    emit errorMessageChanged();
  });

  m_elapsedTimer.setInterval(100);
  connect(&m_elapsedTimer, &QTimer::timeout, this, &TranscribeViewHost::elapsedTimeChanged);

  updateActions();
}

void TranscribeViewHost::loadInitialData() {
  if (m_recorder->start()) {
    m_elapsedTimer.start();
    updateNavigationTitle();
  }
}

void TranscribeViewHost::beforePop() {
  m_elapsedTimer.stop();
  m_recorder->discard();
  context()->services->toastService()->clear();
}

float TranscribeViewHost::audioLevel() const { return m_recorder ? m_recorder->level() : 0.0f; }

QString TranscribeViewHost::elapsedTime() const {
  if (!m_recorder) return QStringLiteral("00:00");

  auto ms = m_recorder->elapsedMs();
  auto secs = ms / 1000;
  auto mins = secs / 60;
  secs %= 60;

  return QStringLiteral("%1:%2").arg(mins, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0'));
}

void TranscribeViewHost::stopAndTranscribe() {
  m_elapsedTimer.stop();
  m_recorder->stop();

  auto path = m_recorder->outputPath();
  if (!path) {
    m_errorMessage = "Recording failed";
    emit errorMessageChanged();
    return;
  }

  m_transcribing = true;
  emit transcribingChanged();
  updateNavigationTitle();
  updateActions();

  auto toast = context()->services->toastService();
  toast->dynamic("Transcribing...");

  auto ctx = context();
  m_aiService->transcribe(*path).then([this, ctx, toast](const AI::TranscriptionResult &result) {
    m_transcribing = false;
    emit transcribingChanged();

    if (!result) {
      toast->failure("Transcription failed");
      m_errorMessage = QString::fromStdString(result.error());
      emit errorMessageChanged();
      updateNavigationTitle();
      updateActions();
      return;
    }

    toast->success("Transcription complete");
    ctx->services->pasteService()->pasteContent(Clipboard::Text(result->text.c_str()));
    ctx->navigation->closeWindow({.popToRootType = PopToRootType::Immediate});
  });
}

void TranscribeViewHost::togglePause() {
  if (m_recorder->state() == Audio::Recorder::State::Recording) {
    m_recorder->pause();
    m_elapsedTimer.stop();
  } else if (m_recorder->state() == Audio::Recorder::State::Paused) {
    m_recorder->resume();
    m_elapsedTimer.start();
  }
}

void TranscribeViewHost::updateActions() {
  auto panel = std::make_unique<ActionPanelState>();
  auto *section = panel->createSection();

  if (m_transcribing) {
    setActions(std::move(panel));
    return;
  }

  auto state = m_recorder->state();
  if (state == Audio::Recorder::State::Recording || state == Audio::Recorder::State::Paused) {
    auto *stop = new StaticAction(QStringLiteral("Stop & Transcribe"), ImageURL::builtin("stop-circle"),
                                  [this]() { stopAndTranscribe(); });
    stop->setPrimary(true);
    section->addAction(stop);

    auto pauseLabel =
        state == Audio::Recorder::State::Paused ? QStringLiteral("Resume") : QStringLiteral("Pause");
    auto pauseIcon = state == Audio::Recorder::State::Paused ? "play-circle" : "pause-circle";
    section->addAction(
        new StaticAction(pauseLabel, ImageURL::builtin(pauseIcon), [this]() { togglePause(); }));
  }

  setActions(std::move(panel));
}

void TranscribeViewHost::updateNavigationTitle() {
  if (m_transcribing) {
    setNavigationTitle("Transcribing...");
  } else if (m_recorder->state() == Audio::Recorder::State::Recording) {
    setNavigationTitle("Recording...");
  } else if (m_recorder->state() == Audio::Recorder::State::Paused) {
    setNavigationTitle("Paused");
  } else {
    setNavigationTitle("Transcribe");
  }
}

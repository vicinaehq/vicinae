#include "inference-runtime.hpp"
#include <cstdint>
#include <cstring>
#include <utility>
#include <QElapsedTimer>
#include <qlogging.h>
#include "common/common.hpp"

namespace AI {

namespace {

constexpr auto HELPER_PROGRAM = "vicinae-inference";
constexpr int SAMPLE_RATE = 16000;
// Covers a cold model load plus inference well below real time; anything slower is a stalled helper.
constexpr auto TIMEOUT_BASE = std::chrono::seconds{30};
constexpr int TIMEOUT_PER_AUDIO_SECOND = 2;

std::optional<inference_gen::Engine> toEngine(LocalEngine engine) {
  switch (engine) {
  case LocalEngine::Whisper:
    return inference_gen::Engine::Whisper;
  case LocalEngine::Parakeet:
    return inference_gen::Engine::Parakeet;
  default:
    return std::nullopt;
  }
}

} // namespace

InferenceBus::InferenceBus(QIODevice *device) : m_device(device) {
  connect(device, &QIODevice::readyRead, this, &InferenceBus::readyRead);
}

void InferenceBus::send(std::string_view data) {
  const std::uint32_t size = data.size();
  m_device->write(reinterpret_cast<const char *>(&size), sizeof(size));
  m_device->write(data.data(), static_cast<qint64>(data.size()));
}

void InferenceBus::readyRead() {
  m_buffer.append(m_device->readAll());

  while (std::cmp_greater_equal(m_buffer.size(), sizeof(std::uint32_t))) {
    std::uint32_t length = 0;
    std::memcpy(&length, m_buffer.constData(), sizeof(length));
    if (m_buffer.size() - sizeof(std::uint32_t) < length) break;

    emit messageReceived(m_buffer.sliced(sizeof(std::uint32_t), length));
    m_buffer = m_buffer.sliced(sizeof(std::uint32_t) + length);
  }
}

InferenceRuntime::InferenceRuntime(QObject *parent)
    : QObject(parent), m_bus(&m_process), m_rpc(m_bus), m_client(m_rpc) {
  m_idleTimer.setSingleShot(true);
  connect(&m_idleTimer, &QTimer::timeout, this, &InferenceRuntime::release);
  connect(&m_process, &QProcess::finished, this, &InferenceRuntime::handleExit);
  connect(&m_process, &QProcess::readyReadStandardError, this, &InferenceRuntime::handleStderr);
  connect(&m_bus, &InferenceBus::messageReceived, this, [this](const QByteArray &msg) {
    if (auto res = m_client.route(std::string_view{msg.constData(), static_cast<std::size_t>(msg.size())});
        !res) {
      qWarning() << "Failed to route inference message" << res.error();
    }
  });
}

InferenceRuntime::~InferenceRuntime() {
  if (m_process.state() == QProcess::NotRunning) return;
  m_process.kill();
  m_process.waitForFinished();
}

void InferenceRuntime::setIdleTimeout(std::optional<std::chrono::seconds> timeout) {
  m_idleTimeout = timeout;
  if (idle()) rearmIdle();
}

void InferenceRuntime::preload(LocalEngine engine, const std::filesystem::path &model) {
  const auto wire = toEngine(engine);
  if (!wire || !ensureRunning()) return;

  ++m_holds;
  requestStarted();
  m_client.inference()
      ->load({.engine = *wire, .path = model.string()})
      .then(this, [this](std::expected<void, std::string> result) {
        if (!result) qWarning() << "Failed to preload model:" << result.error();
        requestFinished();
      });
}

QFuture<TranscriptionResult> InferenceRuntime::transcribe(const Audio::Recording &recording,
                                                          const Transcription &request) {
  const auto wire = toEngine(request.engine);
  if (!wire) {
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(
        std::unexpected("Cannot run inference for this engine"));
  }
  if (!ensureRunning()) {
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(
        std::unexpected("Local inference is unavailable"));
  }

  inference_gen::TranscribeRequest req{
      .engine = *wire,
      .path = request.model.string(),
      .language = request.language,
      .initial_prompt = request.initialPrompt,
  };
  QElapsedTimer timer;
  timer.start();
  const auto samples = recording.toF32();
  req.samples.assign(samples.begin(), samples.end());

  auto pending = std::make_shared<QPromise<TranscriptionResult>>();
  auto future = pending->future();
  m_pending.emplace_back(pending);
  if (m_holds > 0) --m_holds;
  requestStarted();

  const auto audioSeconds = static_cast<qint64>(samples.size()) / SAMPLE_RATE;
  const auto timeout = TIMEOUT_BASE + std::chrono::seconds{audioSeconds * TIMEOUT_PER_AUDIO_SECOND};
  QTimer::singleShot(timeout, this, [this, pending]() { timeOut(pending); });

  auto reply = m_client.inference()->transcribe(req);
  const auto sentMs = timer.elapsed();

  reply.then(this, [this, pending, timer, sentMs, count = samples.size()](
                       std::expected<inference_gen::TranscribeResponse, std::string> result) {
    if (std::erase(m_pending, pending) == 0) return;
    qInfo() << "Local inference:" << count << "samples sent in" << sentMs << "ms, answered after"
            << timer.elapsed() << "ms";

    if (result) {
      pending->addResult(
          TranscriptionResponse{.text = std::move(result->text), .language = std::move(result->language)});
    } else {
      pending->addResult(std::unexpected(std::move(result).error()));
    }
    pending->finish();
    requestFinished();
  });

  return future;
}

void InferenceRuntime::cancelPreload() {
  if (m_holds > 0) --m_holds;
  if (idle()) rearmIdle();
}

void InferenceRuntime::timeOut(const Pending &pending) {
  if (std::erase(m_pending, pending) == 0) return;

  qWarning() << HELPER_PROGRAM << "did not answer in time, releasing it";
  pending->addResult(std::unexpected("Transcription timed out"));
  pending->finish();
  release();
}

void InferenceRuntime::release() {
  m_idleTimer.stop();
  if (m_process.state() == QProcess::NotRunning) return;
  qInfo() << "Releasing local inference";
  m_releasing = true;
  m_process.kill();
}

bool InferenceRuntime::ensureRunning() {
  if (m_process.state() == QProcess::Running) return true;

  const auto path = vicinae::findHelperProgram(HELPER_PROGRAM);
  if (!path) {
    qWarning() << "Could not find the" << HELPER_PROGRAM << "helper, local transcription is unavailable";
    return false;
  }

  m_process.setProgram(QString::fromStdString(path->string()));
  m_process.start();

  if (!m_process.waitForStarted()) {
    qCritical() << "Failed to start" << HELPER_PROGRAM << m_process.errorString();
    return false;
  }

  return true;
}

void InferenceRuntime::requestStarted() {
  m_idleTimer.stop();
  ++m_inflight;
}

void InferenceRuntime::requestFinished() {
  if (m_inflight > 0) --m_inflight;
  if (idle()) rearmIdle();
}

void InferenceRuntime::rearmIdle() {
  m_idleTimer.stop();
  if (m_process.state() == QProcess::NotRunning || !m_idleTimeout) return;
  if (m_idleTimeout->count() == 0) {
    release();
    return;
  }
  m_idleTimer.start(*m_idleTimeout);
}

void InferenceRuntime::handleExit(int code, QProcess::ExitStatus status) {
  if (!std::exchange(m_releasing, false) && (status == QProcess::CrashExit || code != 0)) {
    qWarning() << HELPER_PROGRAM << "exited unexpectedly with code" << code;
  }

  m_idleTimer.stop();
  m_inflight = 0;
  m_holds = 0;

  for (const auto &pending : std::exchange(m_pending, {})) {
    pending->addResult(std::unexpected("Local inference stopped unexpectedly"));
    pending->finish();
  }
}

void InferenceRuntime::handleStderr() {
  m_stderrBuffer += m_process.readAllStandardError();

  qsizetype idx = 0;
  while ((idx = m_stderrBuffer.indexOf('\n')) != -1) {
    const auto line = m_stderrBuffer.left(idx);
    m_stderrBuffer = m_stderrBuffer.sliced(idx + 1);
    if (!line.trimmed().isEmpty()) qInfo().noquote() << "[inference]" << QString::fromUtf8(line);
  }
}

} // namespace AI

#include "audio-recorder.hpp"
#include "wav-writer.hpp"
#include <QDir>
#include <QTemporaryFile>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <qlogging.h>

namespace Audio {

Recorder::Recorder(QObject *parent) : QObject(parent) {}

Recorder::~Recorder() { discard(); }

QAudioFormat Recorder::targetFormat() const {
  QAudioFormat fmt;
  fmt.setSampleRate(16000);
  fmt.setChannelCount(1);
  fmt.setSampleFormat(QAudioFormat::Int16);
  return fmt;
}

bool Recorder::start() {
  if (m_state != State::Idle) return false;

  auto device = QMediaDevices::defaultAudioInput();
  if (device.isNull()) {
    emit errorOccurred("No audio input device found");
    return false;
  }

  m_format = targetFormat();
  if (!device.isFormatSupported(m_format)) {
    qDebug() << "Target format (16kHz/mono/16-bit) not supported, using preferred format";
    m_format = device.preferredFormat();
    m_format.setSampleFormat(QAudioFormat::Int16);
  }

  m_source = std::make_unique<QAudioSource>(device, m_format, this);
  m_ioDevice = m_source->start();

  if (!m_ioDevice) {
    emit errorOccurred("Failed to start audio recording");
    m_source.reset();
    return false;
  }

  // Reserve for ~2 minutes of audio
  m_pcmBuffer.clear();
  m_pcmBuffer.reserve(m_format.sampleRate() * m_format.channelCount() * 120);
  m_pausedElapsed = 0;
  m_elapsed.start();

  connect(m_ioDevice, &QIODevice::readyRead, this, &Recorder::processAudioData);

  m_state = State::Recording;
  emit stateChanged();
  return true;
}

void Recorder::pause() {
  if (m_state != State::Recording) return;

  m_source->suspend();
  m_pausedElapsed += m_elapsed.elapsed();
  m_state = State::Paused;
  emit stateChanged();
}

void Recorder::resume() {
  if (m_state != State::Paused) return;

  m_source->resume();
  m_elapsed.start();
  m_state = State::Recording;
  emit stateChanged();
}

void Recorder::stop() {
  if (m_state == State::Idle) return;

  if (m_source) {
    m_source->stop();
    m_source.reset();
  }
  m_ioDevice = nullptr;

  auto tmpPath = QDir::tempPath() + "/vicinae-recording-XXXXXX.wav";
  auto tmpFile = std::make_unique<QTemporaryFile>(tmpPath);
  tmpFile->setAutoRemove(false);

  if (!tmpFile->open()) {
    qWarning() << "Failed to create temp file for WAV output";
    m_state = State::Idle;
    emit stateChanged();
    return;
  }

  auto filePath = std::filesystem::path(tmpFile->fileName().toStdString());
  tmpFile->close();

  auto err = writeWav(filePath, m_format, m_pcmBuffer);
  if (err) {
    qWarning() << "WAV write failed:" << err->c_str();
    std::filesystem::remove(filePath);
  } else {
    m_outputPath = filePath;
  }

  m_state = State::Idle;
  emit stateChanged();
}

void Recorder::discard() {
  if (m_source) {
    m_source->stop();
    m_source.reset();
  }
  m_ioDevice = nullptr;
  m_pcmBuffer.clear();

  if (m_outputPath) {
    std::error_code ec;
    std::filesystem::remove(*m_outputPath, ec);
    m_outputPath.reset();
  }

  m_level = 0.0f;
  m_state = State::Idle;
}

qint64 Recorder::elapsedMs() const {
  if (m_state == State::Recording) return m_pausedElapsed + m_elapsed.elapsed();
  if (m_state == State::Paused) return m_pausedElapsed;
  return 0;
}

void Recorder::processAudioData() {
  auto data = m_ioDevice->readAll();
  if (data.isEmpty()) return;

  auto sampleCount = data.size() / static_cast<qsizetype>(sizeof(std::int16_t));
  auto *samples = reinterpret_cast<const std::int16_t *>(data.constData());

  m_pcmBuffer.insert(m_pcmBuffer.end(), samples, samples + sampleCount);

  // Compute RMS level
  double sum = 0.0;
  for (qsizetype i = 0; i < sampleCount; ++i) {
    double s = static_cast<double>(samples[i]) / static_cast<double>(INT16_MAX);
    sum += s * s;
  }

  auto rms = std::sqrt(sum / static_cast<double>(sampleCount));
  // Convert to dB scale, map -40dB..0dB to 0.0..1.0
  auto db = 20.0 * std::log10(std::max(rms, 1e-10));
  m_level = static_cast<float>(std::clamp((db + 40.0) / 40.0, 0.0, 1.0));
  emit levelChanged();
}

} // namespace Audio

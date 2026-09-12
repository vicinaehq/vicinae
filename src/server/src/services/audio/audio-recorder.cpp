#include "audio-recorder.hpp"
#include <QDir>
#include <QTemporaryFile>
#include <algorithm>
#include <cmath>
#include <qaudioformat.h>
#include <qbuffer.h>
#include <qlogging.h>
#include <qstringview.h>

namespace Audio {

Recorder::Recorder(QObject *parent) : QObject(parent) {}

Recorder::~Recorder() { discard(); }

QAudioFormat Recorder::targetFormat() const {
  QAudioFormat fmt;
  fmt.setSampleRate(16000);
  fmt.setChannelCount(1);
  fmt.setSampleFormat(QAudioFormat::Float);
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

  auto sampleCount = data.size() / static_cast<qsizetype>(sizeof(float));
  auto *samples = reinterpret_cast<const float *>(data.constData());

  m_pcmBuffer.insert(m_pcmBuffer.end(), samples, samples + sampleCount);

  // Compute RMS level
  double sum = 0.0;

  for (qsizetype i = 0; i < sampleCount; ++i) {
    double s = samples[i];
    sum += s * s;
  }

  auto rms = std::sqrt(sum / static_cast<double>(sampleCount));
  // Convert to dB scale, map -40dB..0dB to 0.0..1.0
  auto db = 20.0 * std::log10(std::max(rms, 1e-10));
  m_level = static_cast<float>(std::clamp((db + 40.0) / 40.0, 0.0, 1.0));
  emit levelChanged();
}

Recording Recorder::finish() const { return Recording{std::move(m_pcmBuffer), m_format}; }

// Audio Recording

Recording::Recording(PCMF32 data, QAudioFormat fmt) : m_data(std::move(data)), m_format(fmt) {}

std::span<const float> Recording::toF32() const { return m_data; }

std::vector<std::int16_t> Recording::toInt16() const {
  std::vector<std::int16_t> pcm16{};
  pcm16.resize(m_data.size());

  for (size_t i = 0; i < m_data.size(); ++i) {
    float s = std::clamp(m_data[i], -1.0f, 1.0f);
    pcm16[i] = static_cast<int16_t>(s * 32767.0f);
  }

  return pcm16;
}

namespace {

struct WavHeader {
  std::array<char, 4> riffId = {'R', 'I', 'F', 'F'};
  std::uint32_t fileSize = 0;
  std::array<char, 4> waveId = {'W', 'A', 'V', 'E'};
  std::array<char, 4> fmtId = {'f', 'm', 't', ' '};
  std::uint32_t fmtSize = 16;
  std::uint16_t audioFormat = 1; // PCM
  std::uint16_t numChannels = 0;
  std::uint32_t sampleRate = 0;
  std::uint32_t byteRate = 0;
  std::uint16_t blockAlign = 0;
  std::uint16_t bitsPerSample = 0;
  std::array<char, 4> dataId = {'d', 'a', 't', 'a'};
  std::uint32_t dataSize = 0;
};

static_assert(sizeof(WavHeader) == 44);

} // namespace

QByteArray Recording::toWav() const {
  auto samples = toInt16();
  auto dataSize = static_cast<std::uint32_t>(samples.size() * sizeof(std::int16_t));
  auto channels = static_cast<std::uint16_t>(m_format.channelCount());
  auto rate = static_cast<std::uint32_t>(m_format.sampleRate());
  constexpr std::uint16_t bitsPerSample = 16;
  QByteArray buf{};
  WavHeader header;

  header.numChannels = channels;
  header.sampleRate = rate;
  header.bitsPerSample = bitsPerSample;
  header.blockAlign = static_cast<std::uint16_t>(channels * bitsPerSample / 8);
  header.byteRate = rate * header.blockAlign;
  header.dataSize = dataSize;
  header.fileSize = sizeof(WavHeader) - 8 + dataSize;

  buf.append(reinterpret_cast<const char *>(&header), sizeof(header));
  buf.append(reinterpret_cast<const char *>(samples.data()), static_cast<qint64>(dataSize));

  return buf;
}

} // namespace Audio

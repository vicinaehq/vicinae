#include "audio-recorder.hpp"
#include <QDir>
#include <QTemporaryFile>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <span>
#include <utility>
#include <qaudioformat.h>
#include <qbuffer.h>
#include <qlogging.h>
#include <qstringview.h>

namespace Audio {

namespace {

// Raw CoreAudio input is 20-30 dB quieter than software-boosted PulseAudio sources, so the meter
// tracks a decaying running peak instead of a fixed range.
constexpr double DYNAMIC_RANGE_DB = 30.0;
constexpr double MIN_PEAK_DB = -30.0;
constexpr double PEAK_DECAY_DB_PER_SEC = 6.0;
// Models drop the last word when speech runs straight into the end of the buffer.
constexpr int TRAILING_SILENCE_MS = 300;

// Qt's own rate conversion on macOS (6.11) stretches the stream by ~18% and garbles it, so we capture
// at the device rate and downsample here with a polyphase windowed sinc.
std::vector<float> resample(std::span<const float> in, int inRate, int outRate) {
  if (inRate == outRate || in.empty()) return {in.begin(), in.end()};

  constexpr int PHASES = 64;
  const double step = static_cast<double>(inRate) / outRate;
  const double cutoff = std::min(1.0, 1.0 / step);
  const int half = static_cast<int>(std::ceil(16.0 * std::max(1.0, step)));
  const int taps = 2 * half;

  std::vector<double> kernel(static_cast<std::size_t>(PHASES) * taps);
  for (int phase = 0; phase < PHASES; ++phase) {
    const auto weights = std::span(kernel).subspan(static_cast<std::size_t>(phase) * taps, taps);
    const double frac = static_cast<double>(phase) / PHASES;
    double norm = 0.0;
    for (int j = 0; j < taps; ++j) {
      const double x = frac + half - 1 - j;
      const double window = 0.5 + 0.5 * std::cos(std::numbers::pi * x / half);
      const double arg = std::numbers::pi * cutoff * x;
      const double sinc = std::abs(arg) < 1e-9 ? 1.0 : std::sin(arg) / arg;
      weights[j] = sinc * window;
      norm += weights[j];
    }
    for (auto &weight : weights) {
      weight /= norm;
    }
  }

  const auto count = static_cast<std::size_t>(static_cast<double>(in.size()) / step);
  const auto size = std::ssize(in);
  std::vector<float> out;
  out.reserve(count);

  for (std::size_t n = 0; n < count; ++n) {
    const double center = static_cast<double>(n) * step;
    const auto i0 = static_cast<std::ptrdiff_t>(std::floor(center));
    const int phase =
        std::min(static_cast<int>((center - static_cast<double>(i0)) * PHASES + 0.5), PHASES - 1);
    const auto weights = std::span(kernel).subspan(static_cast<std::size_t>(phase) * taps, taps);
    const std::ptrdiff_t first = i0 - half + 1;
    const std::ptrdiff_t last = std::min(size - 1, first + taps - 1);
    double acc = 0.0;
    for (std::ptrdiff_t i = std::max<std::ptrdiff_t>(0, first); i <= last; ++i) {
      acc += weights[static_cast<std::size_t>(i - first)] * in[static_cast<std::size_t>(i)];
    }
    out.emplace_back(static_cast<float>(acc));
  }

  return out;
}

} // namespace

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
  m_captureFormat = m_format;
  if (const auto nativeRate = device.preferredFormat().sampleRate(); nativeRate > 0) {
    m_captureFormat.setSampleRate(nativeRate);
  }
  if (!device.isFormatSupported(m_captureFormat)) m_captureFormat = m_format;
  if (!device.isFormatSupported(m_captureFormat)) {
    emit errorOccurred(tr("Audio input does not support 16 kHz mono recording"));
    return false;
  }

  qInfo() << "Recorder: capturing from" << device.description() << "at" << m_captureFormat.sampleRate()
          << "Hz";

  m_source = std::make_unique<QAudioSource>(device, m_captureFormat, this);
  m_ioDevice = m_source->start();

  if (!m_ioDevice) {
    emit errorOccurred("Failed to start audio recording");
    m_source.reset();
    return false;
  }

  // Reserve for ~2 minutes of audio
  m_pcmBuffer.clear();
  m_pcmBuffer.reserve(m_captureFormat.sampleRate() * m_captureFormat.channelCount() * 120);
  m_pausedElapsed = 0;
  m_level = 0.0f;
  m_peakDb = MIN_PEAK_DB;
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

  // audio delivered since the last readyRead is lost once the source stops
  if (m_ioDevice) processAudioData();

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

  const auto before = m_pcmBuffer.size();
  appendSamples(data);
  updateLevel(std::span(std::as_const(m_pcmBuffer)).subspan(before));
}

void Recorder::appendSamples(const QByteArray &data) {
  auto count = data.size() / static_cast<qsizetype>(sizeof(float));
  auto *samples = reinterpret_cast<const float *>(data.constData());
  m_pcmBuffer.insert(m_pcmBuffer.end(), samples, samples + count);
}

void Recorder::updateLevel(std::span<const float> samples) {
  if (samples.empty()) return;

  double sum = 0.0;
  for (float s : samples) {
    sum += static_cast<double>(s) * s;
  }

  const auto rms = std::sqrt(sum / static_cast<double>(samples.size()));
  const auto db = 20.0 * std::log10(std::max(rms, 1e-10));
  const auto seconds = static_cast<double>(samples.size()) /
                       static_cast<double>(m_captureFormat.sampleRate() * m_captureFormat.channelCount());

  m_peakDb = std::max({db, m_peakDb - PEAK_DECAY_DB_PER_SEC * seconds, MIN_PEAK_DB});
  const auto floorDb = m_peakDb - DYNAMIC_RANGE_DB;
  m_level = static_cast<float>(std::clamp((db - floorDb) / DYNAMIC_RANGE_DB, 0.0, 1.0));
  emit levelChanged();
}

Recording Recorder::finish() {
  auto pcm = resample(m_pcmBuffer, m_captureFormat.sampleRate(), m_format.sampleRate());
  m_pcmBuffer.clear();

  if (!pcm.empty()) {
    const auto silence = static_cast<std::size_t>(m_format.sampleRate()) * TRAILING_SILENCE_MS / 1000;
    pcm.insert(pcm.end(), silence, 0.0f);
  }

  return Recording{std::move(pcm), m_format};
}

// Audio Recording

Recording::Recording(PCMF32 data, QAudioFormat fmt) : m_data(std::move(data)), m_format(fmt) {}

std::span<const float> Recording::toF32() const { return m_data; }

QAudioFormat Recording::format() const { return m_format; }

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

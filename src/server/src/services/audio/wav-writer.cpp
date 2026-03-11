#include "wav-writer.hpp"
#include <QFile>
#include <array>
#include <cstring>
#include <format>

namespace Audio {

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

std::optional<std::string> writeWav(const std::filesystem::path &path, const QAudioFormat &format,
                                    std::span<const std::int16_t> samples) {
  auto dataSize = static_cast<std::uint32_t>(samples.size_bytes());
  auto channels = static_cast<std::uint16_t>(format.channelCount());
  auto rate = static_cast<std::uint32_t>(format.sampleRate());
  constexpr std::uint16_t bitsPerSample = 16;

  WavHeader header;
  header.numChannels = channels;
  header.sampleRate = rate;
  header.bitsPerSample = bitsPerSample;
  header.blockAlign = static_cast<std::uint16_t>(channels * bitsPerSample / 8);
  header.byteRate = rate * header.blockAlign;
  header.dataSize = dataSize;
  header.fileSize = sizeof(WavHeader) - 8 + dataSize;

  QFile file(path.c_str());
  if (!file.open(QIODevice::WriteOnly)) {
    return std::format("Failed to open {}: {}", path.c_str(), file.errorString().toStdString());
  }

  if (file.write(reinterpret_cast<const char *>(&header), sizeof(header)) != sizeof(header)) {
    return std::format("Failed to write WAV header to {}", path.c_str());
  }

  auto written = file.write(reinterpret_cast<const char *>(samples.data()), static_cast<qint64>(dataSize));
  if (written != static_cast<qint64>(dataSize)) {
    return std::format("Failed to write PCM data to {}", path.c_str());
  }

  return std::nullopt;
}

} // namespace Audio

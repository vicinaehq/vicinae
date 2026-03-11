#pragma once
#include <QAudioFormat>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>

namespace Audio {

/// Writes raw PCM samples as a WAV file. Returns an error string on failure, nullopt on success.
std::optional<std::string> writeWav(const std::filesystem::path &path, const QAudioFormat &format,
                                    std::span<const std::int16_t> samples);

} // namespace Audio

#pragma once
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <vector>

struct whisper_vad_context;

namespace inference {

class VoiceActivityDetector {
public:
  static constexpr int SAMPLE_RATE = 16000;
  // Window size of the bundled Silero model. Must match the n_window stored in its ggml header.
  static constexpr std::size_t FRAME_SIZE = 512;

  static std::optional<VoiceActivityDetector> create();

  VoiceActivityDetector(VoiceActivityDetector &&) noexcept = default;
  VoiceActivityDetector &operator=(VoiceActivityDetector &&) noexcept = default;
  ~VoiceActivityDetector();

  std::vector<float> frameProbabilities(std::span<const float> pcm);

private:
  struct ContextDeleter {
    void operator()(whisper_vad_context *ctx) const;
  };

  explicit VoiceActivityDetector(std::unique_ptr<whisper_vad_context, ContextDeleter> ctx);

  std::unique_ptr<whisper_vad_context, ContextDeleter> m_ctx;
};

struct SpeechTrimParams {
  float threshold = 0.5f;
  float releaseThreshold = 0.35f;
  int minSilenceMs = 300;
  int minSpeechMs = 150;
  // Silence kept on each side of a speech segment; consecutive segments closer than twice this are
  // left untouched, so pauses are capped rather than removed, which is important as pauses encode
  // speech punctuation.
  int padMs = 250;
};

/**
 * Returns the speech-only portion of `pcm`, using one probability per FRAME_SIZE samples.
 * An empty result means no speech was detected at all.
 */
std::vector<float> extractSpeech(std::span<const float> pcm, std::span<const float> frameProbabilities,
                                 const SpeechTrimParams &params = {});

} // namespace inference

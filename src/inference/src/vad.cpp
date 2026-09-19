#include "vad.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include "whisper.h"

extern const unsigned char SILERO_MODEL[];
extern const std::size_t SILERO_MODEL_SIZE;

namespace inference {

namespace {

struct MemoryReader {
  const unsigned char *data;
  std::size_t size;
  std::size_t pos = 0;
};

} // namespace

VoiceActivityDetector::VoiceActivityDetector(std::unique_ptr<whisper_vad_context, ContextDeleter> ctx)
    : m_ctx(std::move(ctx)) {}

VoiceActivityDetector::~VoiceActivityDetector() = default;

void VoiceActivityDetector::ContextDeleter::operator()(whisper_vad_context *ctx) const {
  whisper_vad_free(ctx);
}

std::optional<VoiceActivityDetector> VoiceActivityDetector::create() {
  MemoryReader reader{.data = SILERO_MODEL, .size = SILERO_MODEL_SIZE};

  whisper_model_loader loader{
      .context = &reader,
      .read = [](void *ctx, void *out, std::size_t size) -> std::size_t {
        auto *r = static_cast<MemoryReader *>(ctx);
        const auto count = std::min(size, r->size - r->pos);
        std::memcpy(out, r->data + r->pos, count);
        r->pos += count;
        return count;
      },
      .eof =
          [](void *ctx) {
            auto *r = static_cast<MemoryReader *>(ctx);
            return r->pos >= r->size;
          },
      .close = [](void *) {},
  };

  auto params = whisper_vad_default_context_params();
  params.n_threads = 1;
  params.use_gpu = false;

  std::unique_ptr<whisper_vad_context, ContextDeleter> ctx(whisper_vad_init_with_params(&loader, params));
  if (!ctx) {
    std::cerr << "Failed to load the voice activity detection model\n";
    return std::nullopt;
  }

  return VoiceActivityDetector(std::move(ctx));
}

std::vector<float> VoiceActivityDetector::frameProbabilities(std::span<const float> pcm) {
  const auto frames = pcm.size() / FRAME_SIZE;
  if (frames == 0) return {};

  if (!whisper_vad_detect_speech(m_ctx.get(), pcm.data(), static_cast<int>(frames * FRAME_SIZE))) {
    // never drop audio on a detector failure
    return std::vector<float>(frames, 1.0f);
  }

  const auto probs =
      std::span(whisper_vad_probs(m_ctx.get()), static_cast<std::size_t>(whisper_vad_n_probs(m_ctx.get())));
  return {probs.begin(), probs.end()};
}

std::vector<float> extractSpeech(std::span<const float> pcm, std::span<const float> frameProbabilities,
                                 const SpeechTrimParams &params) {
  constexpr auto FRAME = VoiceActivityDetector::FRAME_SIZE;
  const auto toFrames = [](int ms) {
    return static_cast<std::size_t>(ms) * VoiceActivityDetector::SAMPLE_RATE / 1000 / FRAME;
  };
  const auto minSilence = toFrames(params.minSilenceMs);
  const auto minSpeech = toFrames(params.minSpeechMs);
  const auto pad = toFrames(params.padMs);
  const auto frameCount = frameProbabilities.size();

  struct Segment {
    std::size_t start;
    std::size_t end;
  };

  std::vector<Segment> segments;
  segments.reserve(32);

  std::optional<std::size_t> speechStart;
  std::optional<std::size_t> silenceStart;

  const auto closeSegment = [&](std::size_t end) {
    if (end - *speechStart >= minSpeech) segments.emplace_back(*speechStart, end);
    speechStart.reset();
    silenceStart.reset();
  };

  for (std::size_t frame = 0; frame < frameCount; ++frame) {
    const float prob = frameProbabilities[frame];

    if (!speechStart) {
      if (prob >= params.threshold) speechStart = frame;
      continue;
    }
    if (prob >= params.threshold) {
      silenceStart.reset();
      continue;
    }
    if (!silenceStart) {
      if (prob < params.releaseThreshold) silenceStart = frame;
      continue;
    }
    if (frame - *silenceStart >= minSilence) closeSegment(*silenceStart);
  }
  if (speechStart) closeSegment(silenceStart.value_or(frameCount));

  std::vector<Segment> padded;
  padded.reserve(segments.size());

  for (const auto &segment : segments) {
    const auto start = segment.start > pad ? segment.start - pad : 0;
    const auto end = std::min(segment.end + pad, frameCount);
    if (!padded.empty() && start <= padded.back().end) {
      padded.back().end = end;
    } else {
      padded.emplace_back(start, end);
    }
  }

  std::vector<float> speech;
  std::size_t total = 0;
  for (const auto &segment : padded) {
    total += (segment.end - segment.start) * FRAME;
  }
  speech.reserve(total);

  for (const auto &segment : padded) {
    const auto first = segment.start * FRAME;
    // the trailing partial frame has no probability of its own, so it follows the last full frame
    const auto last = segment.end == frameCount ? pcm.size() : std::min(segment.end * FRAME, pcm.size());
    speech.insert(speech.end(), pcm.begin() + static_cast<std::ptrdiff_t>(first),
                  pcm.begin() + static_cast<std::ptrdiff_t>(last));
  }

  return speech;
}

} // namespace inference

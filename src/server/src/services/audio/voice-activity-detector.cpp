#include "voice-activity-detector.hpp"
#include <algorithm>
#include <mutex>
#include <optional>
#include <QDebug>
#include <QFile>
#include <QString>
#include "common/enumerate.hpp"
#include "whisper.h"
#include <ggml-backend.h>

namespace Audio {

VoiceActivityDetector::VoiceActivityDetector(std::unique_ptr<whisper_vad_context, ContextDeleter> ctx)
    : m_ctx(std::move(ctx)) {
  constexpr std::size_t EXPECTED_SECONDS = 120;
  m_pending.reserve(FRAME_SIZE * 4);
  m_probs.reserve(EXPECTED_SECONDS * SAMPLE_RATE / FRAME_SIZE);
}

VoiceActivityDetector::~VoiceActivityDetector() = default;

namespace {

// whisper logs a few INFO lines on every detection call, which is far too chatty for a streaming use.
void whisperLog(ggml_log_level level, const char *text, void *) {
  switch (level) {
  case GGML_LOG_LEVEL_WARN:
    qWarning().noquote() << "whisper:" << QString::fromUtf8(text).trimmed();
    break;
  case GGML_LOG_LEVEL_ERROR:
    qCritical().noquote() << "whisper:" << QString::fromUtf8(text).trimmed();
    break;
  default:
    break;
  }
}

} // namespace

void VoiceActivityDetector::ContextDeleter::operator()(whisper_vad_context *ctx) const {
  whisper_vad_free(ctx);
}

std::optional<VoiceActivityDetector> VoiceActivityDetector::create() {
  static std::once_flag logFlag;
  std::call_once(logFlag, [] { whisper_log_set(whisperLog, nullptr); });

  QFile file(QStringLiteral(":/models/ggml-silero-v6.2.0.bin"));
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Voice activity detection model is missing from resources";
    return std::nullopt;
  }

  whisper_model_loader loader{
      .context = &file,
      .read = [](void *ctx, void *out, std::size_t size) -> std::size_t {
        const auto read =
            static_cast<QFile *>(ctx)->read(static_cast<char *>(out), static_cast<qint64>(size));
        return read < 0 ? 0 : static_cast<std::size_t>(read);
      },
      .eof = [](void *ctx) { return static_cast<QFile *>(ctx)->atEnd(); },
      .close = [](void *ctx) { static_cast<QFile *>(ctx)->close(); },
  };

  ggml_backend_load_all();

  auto params = whisper_vad_default_context_params();
  params.n_threads = 1;
  params.use_gpu = false;

  std::unique_ptr<whisper_vad_context, ContextDeleter> ctx(whisper_vad_init_with_params(&loader, params));
  if (!ctx) {
    qWarning() << "Failed to load the voice activity detection model";
    return std::nullopt;
  }

  whisper_vad_reset_state(ctx.get());
  return VoiceActivityDetector(std::move(ctx));
}

void VoiceActivityDetector::feed(std::span<const float> samples) {
  m_pending.insert(m_pending.end(), samples.begin(), samples.end());

  const auto frames = m_pending.size() / FRAME_SIZE;
  if (frames == 0) return;
  const auto count = frames * FRAME_SIZE;

  if (whisper_vad_detect_speech_no_reset(m_ctx.get(), m_pending.data(), static_cast<int>(count))) {
    const auto probs =
        std::span(whisper_vad_probs(m_ctx.get()), static_cast<std::size_t>(whisper_vad_n_probs(m_ctx.get())));
    m_probs.insert(m_probs.end(), probs.begin(), probs.end());
  } else {
    // keep frames and probabilities aligned, and never drop audio on a detector failure
    m_probs.insert(m_probs.end(), frames, 1.0f);
  }

  m_pending.erase(m_pending.begin(), m_pending.begin() + static_cast<std::ptrdiff_t>(count));
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

  for (const auto [frame, prob] : vicinae::enumerate(frameProbabilities)) {

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

} // namespace Audio

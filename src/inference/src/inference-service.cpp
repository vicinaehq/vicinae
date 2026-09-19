#include "inference-service.hpp"
#include <iostream>
#include <vector>
#include <ggml-backend.h>
#include "parakeet.h"
#include "whisper.h"

namespace inference {

namespace {

std::vector<float> toF32(const std::vector<double> &samples) {
  std::vector<float> pcm;
  pcm.reserve(samples.size());
  for (const double sample : samples) {
    pcm.emplace_back(static_cast<float>(sample));
  }
  return pcm;
}

void log(std::string_view message) { std::cerr << message << '\n' << std::flush; }

} // namespace

void InferenceService::WhisperDeleter::operator()(whisper_context *ctx) const { whisper_free(ctx); }

void InferenceService::ParakeetDeleter::operator()(parakeet_context *ctx) const { parakeet_free(ctx); }

InferenceService::InferenceService(inference_gen::RpcTransport &transport)
    : AbstractInference(transport), m_worker([this]() { run(); }) {}

InferenceService::~InferenceService() {
  {
    std::scoped_lock const lock(m_mutex);
    m_stopping = true;
  }
  m_wake.notify_all();
  if (m_worker.joinable()) m_worker.join();
}

void InferenceService::enqueue(Job job) {
  {
    std::scoped_lock const lock(m_mutex);
    m_jobs.emplace_back(std::move(job));
  }
  m_wake.notify_one();
}

void InferenceService::run() {
  ggml_backend_load_all();

  while (true) {
    Job job;
    {
      std::unique_lock lock(m_mutex);
      m_wake.wait(lock, [this]() { return m_stopping || !m_jobs.empty(); });
      if (m_stopping) return;
      job = std::move(m_jobs.front());
      m_jobs.pop_front();
    }
    job();
  }
}

void InferenceService::load(inference_gen::LoadRequest req,
                            std::function<void(std::expected<void, std::string>)> reply) {
  enqueue([this, req = std::move(req), reply = std::move(reply)]() {
    reply(ensureLoaded(req.engine, req.path));
  });
}

void InferenceService::transcribe(inference_gen::TranscribeRequest req, std::function<void(Response)> reply) {
  enqueue([this, req = std::move(req), reply = std::move(reply)]() {
    auto pcm = toF32(req.samples);
    if (auto *detector = vad()) pcm = extractSpeech(pcm, detector->frameProbabilities(pcm));
    if (pcm.empty()) {
      reply(inference_gen::TranscribeResponse{});
      return;
    }

    if (auto loaded = ensureLoaded(req.engine, req.path); !loaded) {
      reply(std::unexpected(loaded.error()));
      return;
    }

    switch (req.engine) {
    case inference_gen::Engine::Whisper:
      reply(transcribeWhisper(req, pcm));
      break;
    case inference_gen::Engine::Parakeet:
      reply(transcribeParakeet(pcm));
      break;
    }
  });
}

VoiceActivityDetector *InferenceService::vad() {
  if (!m_vadTried) {
    m_vadTried = true;
    m_vad = VoiceActivityDetector::create();
  }
  return m_vad ? &*m_vad : nullptr;
}

std::expected<void, std::string> InferenceService::ensureLoaded(inference_gen::Engine engine,
                                                                const std::string &path) {
  switch (engine) {
  case inference_gen::Engine::Whisper:
    return loadWhisper(path);
  case inference_gen::Engine::Parakeet:
    return loadParakeet(path);
  }
  return std::unexpected("Unknown engine");
}

std::expected<void, std::string> InferenceService::loadWhisper(const std::string &path) {
  if (m_whisper.holds(path)) return {};
  m_whisper = {};

  log("loading whisper model " + path);

  auto *ctx = whisper_init_from_file_with_params(path.c_str(), whisper_context_default_params());
  if (!ctx) return std::unexpected("Failed to load whisper model " + path);

  m_whisper = {.ctx = std::unique_ptr<whisper_context, WhisperDeleter>(ctx), .path = path};
  return {};
}

std::expected<void, std::string> InferenceService::loadParakeet(const std::string &path) {
  if (m_parakeet.holds(path)) return {};
  m_parakeet = {};

  log("loading parakeet model " + path);

  auto *ctx = parakeet_init_from_file_with_params(path.c_str(), parakeet_context_default_params());
  if (!ctx) return std::unexpected("Failed to load parakeet model " + path);

  m_parakeet = {.ctx = std::unique_ptr<parakeet_context, ParakeetDeleter>(ctx), .path = path};
  return {};
}

InferenceService::Response InferenceService::transcribeWhisper(const inference_gen::TranscribeRequest &req,
                                                               std::span<const float> pcm) {
  auto *ctx = m_whisper.ctx.get();
  whisper_full_params params =
      whisper_full_default_params(whisper_sampling_strategy::WHISPER_SAMPLING_BEAM_SEARCH);
  params.language = req.language ? req.language->c_str() : "auto";
  params.initial_prompt = req.initial_prompt ? req.initial_prompt->c_str() : nullptr;

  if (whisper_full(ctx, params, pcm.data(), static_cast<int>(pcm.size())) != 0) {
    return std::unexpected("Failed to transcribe");
  }

  inference_gen::TranscribeResponse response;
  for (int i = 0, n = whisper_full_n_segments(ctx); i < n; ++i) {
    response.text += whisper_full_get_segment_text(ctx, i);
  }
  if (const char *lang = whisper_lang_str(whisper_full_lang_id(ctx))) response.language = lang;

  return response;
}

InferenceService::Response InferenceService::transcribeParakeet(std::span<const float> pcm) {
  auto *ctx = m_parakeet.ctx.get();
  parakeet_full_params params =
      parakeet_full_default_params(parakeet_sampling_strategy::PARAKEET_SAMPLING_GREEDY);

  if (parakeet_full(ctx, params, pcm.data(), static_cast<int>(pcm.size())) != 0) {
    return std::unexpected("Failed to transcribe");
  }

  inference_gen::TranscribeResponse response;
  for (int i = 0, n = parakeet_full_n_segments(ctx); i < n; ++i) {
    response.text += parakeet_full_get_segment_text(ctx, i);
  }

  return response;
}

} // namespace inference

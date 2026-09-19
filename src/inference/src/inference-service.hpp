#pragma once
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include "generated/inference-server.hpp"
#include "vad.hpp"

struct whisper_context;
struct parakeet_context;

namespace inference {

class InferenceService : public inference_gen::AbstractInference {
public:
  explicit InferenceService(inference_gen::RpcTransport &transport);
  ~InferenceService();

  void load(inference_gen::LoadRequest req,
            std::function<void(std::expected<void, std::string>)> reply) override;
  void transcribe(
      inference_gen::TranscribeRequest req,
      std::function<void(std::expected<inference_gen::TranscribeResponse, std::string>)> reply) override;

private:
  using Job = std::function<void()>;
  using Response = std::expected<inference_gen::TranscribeResponse, std::string>;

  struct WhisperDeleter {
    void operator()(whisper_context *ctx) const;
  };
  struct ParakeetDeleter {
    void operator()(parakeet_context *ctx) const;
  };

  template <typename Context, typename Deleter> struct Loaded {
    std::unique_ptr<Context, Deleter> ctx;
    std::string path;

    bool holds(const std::string &p) const { return ctx && path == p; }
  };

  void enqueue(Job job);
  void run();

  VoiceActivityDetector *vad();
  std::expected<void, std::string> ensureLoaded(inference_gen::Engine engine, const std::string &path);
  std::expected<void, std::string> loadWhisper(const std::string &path);
  std::expected<void, std::string> loadParakeet(const std::string &path);
  Response transcribeWhisper(const inference_gen::TranscribeRequest &req, std::span<const float> pcm);
  Response transcribeParakeet(std::span<const float> pcm);

  Loaded<whisper_context, WhisperDeleter> m_whisper;
  Loaded<parakeet_context, ParakeetDeleter> m_parakeet;
  std::optional<VoiceActivityDetector> m_vad;
  bool m_vadTried = false;

  std::mutex m_mutex;
  std::condition_variable m_wake;
  std::deque<Job> m_jobs;
  bool m_stopping = false;
  std::thread m_worker;
};

} // namespace inference

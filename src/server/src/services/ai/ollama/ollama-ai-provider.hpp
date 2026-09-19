#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include "internal/http-client.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace ollama {

struct ModelShowResponse {
  std::vector<std::string> capabilities;
};

struct FullModelResponse {
  std::string name;
  std::string model;
  Capabilities capabilities;
  std::string family;
};

struct VersionResponse {
  std::string version;
};

struct ListModelsResponse {
  struct Model {
    struct Details {
      std::string family;
    };

    std::string name;
    std::string model;
    std::uint64_t size = 0;
    Details details;
  };

  std::vector<Model> models;
};

struct ModelShowRequest {
  std::string model;
  bool verbose = false;
};

struct ChatMessage {
  std::string role;
  std::string content;
};

struct ChatPayload {
  struct Options {
    std::optional<float> temperature;
  };
  std::string model;
  std::vector<ChatMessage> messages;
  bool stream = true;
  Options options;
};

} // namespace ollama

class OllamaProvider : public AbstractProvider {
public:
  OllamaProvider();
  ~OllamaProvider() override;

  std::string id() const override { return "ollama"; }
  std::string_view type() const override { return "ollama"; }
  std::optional<ImageUrl> icon() const override { return {}; }
  std::string_view description() const override { return "Connect to a local or remote Ollama instance."; }

  void configure(const ProviderFields &fields) override;
  void start() override;

  ModelList listModels(const ListModelFilters &filters = {}) const override;
  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) override;
  QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                          const TranscriptionOptions &opts = {}) override;

private:
  using ModelsResult = Result<std::vector<ollama::FullModelResponse>>;

  QFuture<Result<ollama::VersionResponse>> fetchVersion();
  QFuture<Result<ollama::ListModelsResponse>> fetchModels();
  QFuture<ModelsResult> listModelsFull();

  http::Client m_client;
  http::Client::Watcher<Result<ollama::VersionResponse>> m_handshakeWatcher;
  http::Client::Watcher<ModelsResult> m_listWatcher;
  std::string m_url;
  std::vector<ollama::FullModelResponse> m_models;
  bool m_started = false;
};

} // namespace AI

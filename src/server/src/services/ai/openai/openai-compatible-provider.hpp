#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <qfuture.h>
#include "internal/http-client.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-provider-types.hpp"
#include "services/ai/openai/openai-chat-completion.hpp"
#include "services/audio/audio-recorder.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace openai {

struct ModelListResponse {
  struct Entry {
    std::string id;
  };

  std::vector<Entry> data;
};

struct TranscriptionResponse {
  std::string text;
  std::optional<std::string> language;
};

} // namespace openai

class OpenAICompatibleProvider : public AbstractProvider {
  Q_OBJECT

public:
  OpenAICompatibleProvider(std::string id, std::string_view type, std::string defaultUrl);
  ~OpenAICompatibleProvider() override;

  std::string id() const final { return m_id; }
  std::string_view type() const final { return m_info.type; }
  std::string displayName() const override { return m_name.empty() ? std::string(m_info.label) : m_name; }
  std::optional<ImageUrl> icon() const override { return ImageUrl{m_info.icon}; }
  std::string_view description() const override { return m_info.description; }

  void configure(const PreferenceValues &fields) final;
  void start() final;

  ModelList listModels(const ListModelFilters &filters = {}) const final;
  std::optional<Model> findBestModel(Capabilities caps, Preference preference = Preference::None) const final;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) final;
  QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                          const TranscriptionOptions &opts = {}) override;

protected:
  const std::string &url() const { return m_url; }
  http::Client &client() { return m_client; }
  virtual std::string openAIRoot(std::string_view url) const { return std::string(url); }

  virtual QFuture<Result<ModelList>> fetchModels();

  virtual Capabilities capabilitiesFor(std::string_view) const { return Capability::Completion; }

  virtual openai::ChatRequest makeRequest(std::string_view modelId, const Model &,
                                          const ChatCompletionPayload &payload) const {
    return openai::ChatRequest::make(modelId, payload);
  }

private:
  void handleModels();

  std::string m_id;
  const ProviderTypeInfo &m_info;
  std::string m_defaultUrl;
  http::Client m_client;
  http::Client::Watcher<Result<ModelList>> m_listWatcher;
  ModelList m_models;
  std::string m_name;
  std::string m_url;
  std::string m_apiKey;
  bool m_started = false;
};

} // namespace AI

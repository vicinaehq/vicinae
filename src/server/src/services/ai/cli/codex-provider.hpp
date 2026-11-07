#pragma once
#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <QPointer>
#include "services/ai/ai-provider.hpp"

namespace AI {

class JsonLineProcess;
class CodexChatStream;

/**
 * Talks to a `codex app-server` process over JSON-RPC. One process serves every conversation, each
 * response running in its own ephemeral thread seeded with the conversation history.
 */
class CodexProvider : public AbstractProvider {
  Q_OBJECT

public:
  static constexpr std::string_view TYPE = "codex";

  using Response = std::expected<std::string, std::string>;
  using ResponseHandler = std::function<void(Response)>;

  explicit CodexProvider(std::string id);
  ~CodexProvider() override;

  std::string id() const override { return m_id; }
  std::string_view type() const override { return TYPE; }
  std::string displayName() const override;
  std::optional<ImageUrl> icon() const override;
  std::string_view description() const override;
  std::optional<std::string> statusMessage() const override { return m_status; }

  void configure(const PreferenceValues &fields) override;
  void start() override;

  ModelList listModels(const ListModelFilters &filters = {}) const override;
  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) override;
  QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                          const TranscriptionOptions &opts = {}) override;

  /**
   * Send a request, `params` being serialized JSON. The handler receives the raw result or an error.
   */
  void request(std::string_view method, std::string params, ResponseHandler handler = {});
  void watchThread(const std::string &threadId, CodexChatStream *stream);
  void unwatchThread(const std::string &threadId);

private:
  void launch();
  void stop(const std::string &reason);
  void send(std::int64_t id, std::string_view method, std::string params);
  void handleLine(const std::string &line);
  void handleInitialized();
  void handleAccount(Response response);
  void handleModels(Response response);

  std::string m_id;
  std::string m_executableSetting;
  std::optional<QString> m_executable;
  std::optional<std::string> m_status;
  bool m_started = false;
  bool m_ready = false;
  bool m_signedIn = false;
  JsonLineProcess *m_server = nullptr;
  std::int64_t m_nextId = 1;
  std::unordered_map<std::int64_t, ResponseHandler> m_pending;
  std::vector<std::string> m_queue;
  std::unordered_map<std::string, QPointer<CodexChatStream>> m_threads;
  std::vector<Model> m_models;
};

} // namespace AI

#pragma once
#include <chrono>
#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <QByteArray>
#include <QPointer>
#include <QTimer>
#include "services/ai/ai-provider.hpp"

namespace AI {

class JsonLineProcess;
class AcpChatStream;

/**
 * An agent speaking the Agent Client Protocol (https://agentclientprotocol.com). One process serves every
 * conversation, each in its own ACP session. The agent runs its own tools, Vicinae only shows them and
 * answers its permission requests.
 */
class AcpProvider : public AbstractProvider {
  Q_OBJECT

public:
  struct RpcError {
    int code = 0;
    std::string message;
  };
  using Response = std::expected<std::string, RpcError>;
  using ResponseHandler = std::function<void(Response)>;
  using ReadyHandler = std::function<void(std::expected<void, ProviderStatus>)>;

  static constexpr int AUTH_REQUIRED = -32000;

  /**
   * What the agent of a provider type is missing to run, if anything. Only looks for the default command.
   */
  static std::optional<QString> missingRequirement(std::string_view type);

  AcpProvider(std::string id, std::string_view type);
  ~AcpProvider() override;

  std::string id() const override { return m_id; }
  std::string_view type() const override { return m_type; }
  std::string displayName() const override;
  std::optional<ImageUrl> icon() const override;
  std::string_view description() const override;
  std::optional<ProviderStatus> status() const override { return m_status; }

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
   * Run a turn of `stream`, in the session it was given or a new one.
   */
  void runTurn(AcpChatStream *stream);
  void cancelTurn(const std::string &sessionId);
  void respond(const std::string &requestId, std::string_view result);
  std::string signInMessage() const;

private:
  struct Session {
    std::string id;
    // Identifies the history the agent has seen, if it can be continued at all.
    std::optional<QByteArray> fingerprint;
    std::string model;
    bool fresh = true;
    bool busy = false;
    QPointer<AcpChatStream> stream;
    std::chrono::steady_clock::time_point lastUsed = std::chrono::steady_clock::now();
  };

  static constexpr std::size_t MAX_SESSIONS = 6;
  static constexpr auto IDLE_TIMEOUT = std::chrono::minutes(10);

  void connectAgent(ReadyHandler handler);
  void launch();
  void stop(const std::string &reason);
  void request(std::string_view method, std::string params, ResponseHandler handler = {});
  void handleLine(const std::string &line);
  void handleRequest(const std::string &id, std::string_view method, std::string_view params);
  void handleUpdate(std::string_view params);
  void discover();
  void newSession(std::function<void(std::expected<std::string, RpcError>)> handler);
  void prompt(Session &session, AcpChatStream *stream);
  void finishTurn(const std::string &sessionId, Response response);
  void closeSession(const std::string &sessionId);
  void updateIdle();
  void readModels(std::string_view configOptions);
  Session *findSession(std::string_view id);

  std::string m_id;
  std::string m_type;
  std::string m_name;
  std::string m_command;
  std::optional<ProviderStatus> m_status;
  bool m_started = false;
  bool m_available = false;
  bool m_ready = false;
  bool m_supportsImages = false;
  bool m_supportsClose = false;
  JsonLineProcess *m_process = nullptr;
  std::int64_t m_nextId = 1;
  std::unordered_map<std::int64_t, ResponseHandler> m_pending;
  std::vector<ReadyHandler> m_waiting;
  std::vector<Session> m_sessions;
  std::vector<Model> m_models;
  std::optional<std::string> m_modelOption;
  std::optional<std::string> m_defaultModel;
  QTimer m_idle;
};

} // namespace AI

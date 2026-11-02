#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <QByteArray>
#include <QPointer>
#include <QTimer>
#include "services/ai/ai-provider.hpp"

class QProcess;

namespace AI {

class JsonLineProcess;

/**
 * A running `claude` process holding one conversation. It can only continue the conversation it has seen,
 * identified by the fingerprint of its history.
 */
class ClaudeSession : public QObject {
  Q_OBJECT

signals:
  void activityChanged(ResponseActivity activity);
  void textReceived(const std::string &text);
  void turnSucceeded();
  void turnFailed(const std::string &reason);
  void closed();

public:
  ClaudeSession(const QString &executable, std::string model, std::string effort,
                const std::string &systemPrompt, QObject *parent = nullptr);
  ~ClaudeSession() override;

  const std::string &model() const { return m_model; }
  const std::string &effort() const { return m_effort; }
  const std::optional<QByteArray> &fingerprint() const { return m_fingerprint; }
  bool busy() const { return m_busy; }
  std::chrono::steady_clock::time_point lastUsed() const { return m_lastUsed; }

  /**
   * Send a user message. `fingerprint` identifies the history including this message.
   */
  void send(const std::string &line, QByteArray fingerprint);
  void close();

private:
  static constexpr auto IDLE_TIMEOUT = std::chrono::minutes(10);

  void handleLine(const std::string &line);
  void endTurn();

  JsonLineProcess *m_process = nullptr;
  QTimer m_idle;
  std::string m_model;
  std::string m_effort;
  std::optional<QByteArray> m_fingerprint;
  QByteArray m_pendingFingerprint;
  std::string m_text;
  std::chrono::steady_clock::time_point m_lastUsed = std::chrono::steady_clock::now();
  bool m_busy = false;
  bool m_closed = false;
};

class ClaudeCodeProvider : public AbstractProvider {
  Q_OBJECT

public:
  static constexpr std::string_view TYPE = "claude-code";

  explicit ClaudeCodeProvider(std::string id);
  ~ClaudeCodeProvider() override;

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

private:
  static constexpr std::size_t MAX_SESSIONS = 2;

  void detect();
  void handleDetection(int exitCode);
  void closeSessions();
  ClaudeSession *findSession(std::string_view model, std::string_view effort,
                             const QByteArray &fingerprint) const;
  ClaudeSession *createSession(std::string model, std::string effort, const std::string &systemPrompt,
                               bool oneShot);

  std::string m_id;
  std::string m_executableSetting;
  std::optional<QString> m_executable;
  std::optional<std::string> m_status;
  bool m_available = false;
  bool m_started = false;
  QPointer<QProcess> m_detection;
  std::vector<ClaudeSession *> m_sessions;
};

} // namespace AI

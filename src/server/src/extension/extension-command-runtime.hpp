#pragma once
#include <QTimer>
#include <chrono>
#include "command/command.hpp"
#include "command/command-types.hpp"
#include "extension/extension-command.hpp"
#include "extension/manager/extension-manager.hpp"
#include "generated/tsapi.hpp"
#include <expected>
#include <functional>
#include <glaze/json/generic.hpp>
#include <qlogging.h>

class ExtUIService;

class ExtensionManagerBus : public tsapi::AbstractTransport {
public:
  ExtensionManagerBus(ExtensionManager &manager) : m_manager(manager) {}

  void send(std::string_view data) override {
    m_manager.client().manager()->messageExtension(m_sessionId, std::string{data});
  }

  void setSessionId(std::string str) { m_sessionId = std::move(str); }

private:
  ExtensionManager &m_manager;
  std::string m_sessionId;
};

class ExtensionLogger : public tsapi::AbstractLogger {
  void onRequest(std::string_view method) override { qDebug() << "-->" << method; }
  void onResponse(std::string_view method, bool ok, double latencyMs) override {
    qDebug() << "<-- " << method << latencyMs << "ms" << (ok ? "OK" : "ERROR");
  }
  void onEvent(std::string_view method) override { qDebug() << "--> " << method; }
};

class ExtensionCommandRuntime : public CommandContext {
public:
  /** The command's output, or why it failed. */
  using RunResult = std::expected<std::string, std::string>;
  using RunCallback = std::function<void(RunResult)>;

  struct ToolCall {
    glz::generic input;
    bool requiresApproval = false;
  };

  static constexpr std::chrono::minutes HEADLESS_TIMEOUT{1};

private:
  void initialize();
  PreferenceValues resolvePreferenceValues(PreferenceValues values) const;
  std::optional<std::string> missingRequiredPreference(const PreferenceValues &values) const;
  void finish(RunResult result);

public:
  void load(const LaunchProps &props) override;
  void unload() override;

  /**
   * Runs the command without UI. The runtime owns itself from then on: once the command finishes,
   * fails, crashes or times out, `onFinished` is called and the runtime unloads and deletes itself.
   */
  void setHeadless(RunCallback onFinished) {
    m_headless = true;
    m_onFinished = std::move(onFinished);
  }
  void cancel(std::string reason) { finish(std::unexpected(std::move(reason))); }

  /** Runs the command's entrypoint as an AI tool, with the given input. */
  void setToolCall(ToolCall call) { m_toolCall = std::move(call); }

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command);

private:
  std::unique_ptr<ExtensionManagerBus> m_bus;
  std::unique_ptr<ExtensionLogger> m_logger;
  std::unique_ptr<tsapi::RpcTransport> m_transport;
  std::shared_ptr<ExtensionCommand> m_command;
  tsapi::Server *m_server = nullptr;
  ExtUIService *m_ui = nullptr;

  std::string m_sessionId;
  bool m_isDevMode = false;
  bool m_headless = false;
  RunCallback m_onFinished;
  QTimer m_timeout;
  std::optional<ToolCall> m_toolCall;
};

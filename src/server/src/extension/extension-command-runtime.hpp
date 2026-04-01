#pragma once
#include "command.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include "extension/manager/extension-manager.hpp"
#include "generated/tsapi.hpp"

class ExtensionManagerBus : public tsapi::AbstractTransport {
public:
  ExtensionManagerBus(ExtensionManager &manager) : m_manager(manager) {}

  void send(std::string_view data) override {
    m_manager.client().manager()->extensionMessage(m_sessionId, std::string{data});
  }

  void setSessionId(std::string str) { m_sessionId = std::move(str); }

private:
  ExtensionManager &m_manager;
  std::string m_sessionId;
};

class ExtensionCommandRuntime : public CommandContext {
  void initialize();

public:
  void load(const LaunchProps &props) override;
  void unload() override;

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command);

private:
  std::unique_ptr<ExtensionManagerBus> m_bus;
  std::unique_ptr<tsapi::RpcTransport> m_transport;
  std::shared_ptr<ExtensionCommand> m_command;
  tsapi::Server *m_server = nullptr;

  std::string m_sessionId;
  bool m_isDevMode = false;
};

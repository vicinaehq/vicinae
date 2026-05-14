#pragma once
#include "command.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include "extension/manager/extension-manager.hpp"
#include "generated/tsapi.hpp"
#include <qlogging.h>

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
  void initialize();

public:
  void load(const LaunchProps &props) override;
  void unload() override;

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command);

private:
  std::unique_ptr<ExtensionManagerBus> m_bus;
  std::unique_ptr<ExtensionLogger> m_logger;
  std::unique_ptr<tsapi::RpcTransport> m_transport;
  std::shared_ptr<ExtensionCommand> m_command;
  tsapi::Server *m_server = nullptr;

  std::string m_sessionId;
  bool m_isDevMode = false;
  bool m_headless = false;

public:
  void setHeadless(bool value) { m_headless = value; }
};

#pragma once
#include "common/context.hpp"
#include <QLocalSocket>
#include <QDebug>
#include <qlocalserver.h>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "generated/ipc-server.hpp"
#include "common/qt.hpp"

namespace AI {
class AbstractChatCompletionStream;
class McpServer;
} // namespace AI

class IpcService : public ipc_gen::AbstractIpc {
public:
  IpcService(ipc_gen::RpcTransport &transport, ApplicationContext &ctx);

  ipc_gen::Result<ipc_gen::PingResponse>::Future ping() override;
  ipc_gen::Result<ipc_gen::DeeplinkResponse>::Future deeplink(ipc_gen::DeeplinkRequest req) override;
  ipc_gen::Result<ipc_gen::DescribeResponse>::Future describe() override;
  ipc_gen::Result<ipc_gen::LaunchAppResponse>::Future launchApp(ipc_gen::LaunchAppRequest req) override;
  ipc_gen::Result<ipc_gen::ListCommandsResponse>::Future listCommands() override;
  ipc_gen::Result<ipc_gen::LaunchCommandResponse>::Future
  launchCommand(ipc_gen::LaunchCommandRequest req) override;
  ipc_gen::Result<ipc_gen::DMenuResponse>::Future dmenu(ipc_gen::DMenuRequest req) override;
  ipc_gen::Result<void>::Future browserInit(ipc_gen::BrowserInitRequest req) override;
  ipc_gen::Result<void>::Future browserTabsChanged(std::vector<ipc_gen::BrowserTabInfo> tabs) override;
  ipc_gen::Result<void>::Future browserPermissionsChanged(std::vector<std::string> permissions) override;
  ipc_gen::Result<std::vector<ipc_gen::FileResult>>::Future fsQuery(std::string q,
                                                                    ipc_gen::FsQueryParams params) override;
  ipc_gen::Result<ipc_gen::AiListModelsResponse>::Future aiListModels() override;
  ipc_gen::Result<ipc_gen::AiAskResponse>::Future aiAsk(ipc_gen::AiAskRequest req) override;
  ipc_gen::Result<void>::Future mcpSend(std::string message) override;

  // set once the client identified itself as a browser extension
  const std::optional<ipc_gen::BrowserInitRequest> &browser() const { return m_browser; }

private:
  static constexpr int BROWSER_PROTOCOL_VERSION = 2;

  void releaseCompletion(const std::string &id);

  ApplicationContext &m_ctx;
  std::optional<ipc_gen::BrowserInitRequest> m_browser;
  int m_completionIdx = 0;
  // Dropped with the connection, which aborts whatever is still streaming
  std::unordered_map<std::string, std::shared_ptr<AI::AbstractChatCompletionStream>> m_completions;
  AI::McpServer *m_mcp = nullptr;
};

/**
 * A client connected to the IPC server. Each one gets its own RPC stack, so that replies, events and calls
 * naturally go to it.
 */
class IpcConnection : public QObject {
public:
  IpcConnection(QLocalSocket *socket, ApplicationContext &ctx, QObject *parent = nullptr);

  QLocalSocket *socket() const { return m_socket; }
  IpcService *service() const { return m_service; }

  void handleRead();

private:
  struct SocketTransport : public ipc_gen::AbstractTransport {
    explicit SocketTransport(QLocalSocket *socket) : socket(socket) {}
    void send(std::string_view data) override;

    QLocalSocket *socket;
  };

  QLocalSocket *m_socket;
  QByteArray m_frame;
  SocketTransport m_transport;
  ipc_gen::RpcTransport m_rpc;
  // owned by m_server through Qt parenting
  IpcService *m_service;
  ipc_gen::Server m_server;
};

class IpcCommandServer : public QObject {
public:
  IpcCommandServer(ApplicationContext *ctx, QObject *parent = nullptr);
  bool start(const std::string &name);

private:
  void handleConnection();
  void handleDisconnection(IpcConnection *connection);

  QLocalServer m_server;
  ApplicationContext &m_ctx;
};

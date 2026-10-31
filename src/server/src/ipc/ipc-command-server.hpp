#pragma once
#include "common/context.hpp"
#include <QLocalSocket>
#include <QDebug>
#include <qlocalserver.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "generated/ipc-server.hpp"
#include "common/qt.hpp"

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

  // set once the client identified itself as a browser extension
  const std::optional<ipc_gen::BrowserInitRequest> &browser() const { return m_browser; }

private:
  static constexpr int BROWSER_PROTOCOL_VERSION = 2;

  ApplicationContext &m_ctx;
  std::optional<ipc_gen::BrowserInitRequest> m_browser;
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

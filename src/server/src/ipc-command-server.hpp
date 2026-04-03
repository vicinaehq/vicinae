#pragma once
#include "common/context.hpp"
#include <QLocalSocket>
#include <QFutureWatcher>
#include <cstdint>
#include <QDebug>
#include <format>
#include <qlocalserver.h>
#include <string_view>
#include "generated/ipc-server.hpp"
#include "common/qt.hpp"

struct ClientInfo {
  QLocalSocket *conn;
  struct {
    QByteArray data;
    uint32_t length;
  } frame;
  std::optional<ipc_gen::BrowserInitRequest> browser;
};

class IpcService : public ipc_gen::AbstractIpc {
public:
  IpcService(ipc_gen::RpcTransport &transport, ApplicationContext &ctx);

  ipc_gen::Result<ipc_gen::PingResponse>::Future ping() override;
  ipc_gen::Result<ipc_gen::DeeplinkResponse>::Future deeplink(ipc_gen::DeeplinkRequest req) override;
  ipc_gen::Result<ipc_gen::DescribeResponse>::Future describe() override;
  ipc_gen::Result<ipc_gen::LaunchAppResponse>::Future launchApp(ipc_gen::LaunchAppRequest req) override;
  ipc_gen::Result<ipc_gen::DMenuResponse>::Future dmenu(ipc_gen::DMenuRequest req) override;
  ipc_gen::Result<void>::Future browserInit(ipc_gen::BrowserInitRequest req) override;
  ipc_gen::Result<void>::Future browserTabsChanged(std::vector<ipc_gen::BrowserTabInfo> tabs) override;

  void setCallerInfo(ClientInfo *info) { m_caller = info; }

private:
  ApplicationContext &m_ctx;
  ClientInfo *m_caller = nullptr;
};

class IpcCommandServer : public QObject {
public:
  IpcCommandServer(ApplicationContext *ctx, QObject *parent = nullptr);
  bool start(const std::filesystem::path &localPath);

private:
  void processFrame(QLocalSocket *conn, QByteArrayView frame);
  void handleRead(QLocalSocket *conn);
  void handleDisconnection(QLocalSocket *conn);
  void handleConnection();

  QLocalServer m_server;
  std::vector<ClientInfo> m_clients;

  ApplicationContext &m_ctx;

  struct IpcTransport : public ipc_gen::AbstractTransport {
    QLocalSocket *conn = nullptr;
    void send(std::string_view data) override;
  };

  IpcTransport m_transport;
  ipc_gen::RpcTransport m_rpc;
  IpcService m_service;
  ipc_gen::Server m_ipcServer;
};

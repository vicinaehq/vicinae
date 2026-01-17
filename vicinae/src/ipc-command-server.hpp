#pragma once
#include "common.hpp"
#include "types.hpp"
#include <QLocalSocket>
#include <QFutureWatcher>
#include <cstdint>
#include <qlocalserver.h>
#include <string_view>
#include <vicinae-ipc/ipc.hpp>

using ServerSchema = ipc::RpcSchema<ipc::DMenu, ipc::Deeplink, ipc::Ping, ipc::ListApps, ipc::LaunchApp,
                                    ipc::BrowserInit, ipc::BrowserTabsChanged>;

using Watcher = QFutureWatcher<ServerSchema::ResponseVariant>;

struct ClientInfo {
  QLocalSocket *conn;
  struct {
    QByteArray data;
    uint32_t length;
  } frame;
  std::vector<QObjectUniquePtr<Watcher>> m_pending;
  std::optional<ipc::BrowserInit::Request> browser;

  void sendMessage(std::string_view message) {
    uint32_t size = message.size();
    conn->write(reinterpret_cast<const char *>(&size), sizeof(size));
    conn->write(message.data(), message.size());
  }
};

struct IpcContext {
  using CallerContext = ClientInfo;
  struct GlobalContext {
    ApplicationContext *app = nullptr;
  };
};

template <ipc::IsRpcSchema SchemaT, typename Context> class RpcServer {
public:
  struct ContextHandle {
    Context::GlobalContext *global = nullptr;
    Context::CallerContext *caller = nullptr;
    std::string method;
  };

  using Schema = SchemaT;

  RpcServer(Context::GlobalContext ctx) : m_ctx(ctx) {}

  using HRet = std::expected<PromiseLike<typename SchemaT::ResponseVariant>, std::string>;
  using Handler = std::function<HRet(const typename SchemaT::RequestVariant &req, ContextHandle ctx)>;
  using MiddlewareHandler = std::function<std::optional<std::string>(
      const typename SchemaT::RequestVariant &request, ContextHandle ctx)>;

  template <ipc::InSchema<Schema> T>
  void route(std::function<std::expected<PromiseLike<typename T::Response>, std::string>(
                 const typename T::Request &reqData, ContextHandle ctx)>
                 fn) {
    std::string method = T::key;

    m_handlers[method] = [fn](const SchemaT::RequestVariant &req, ContextHandle ctx) -> HRet {
      if (auto actualReq = std::get_if<typename T::Request>(&req)) {
        std::expected<PromiseLike<typename T::Response>, std::string> res = fn(*actualReq, ctx);

        if (!res) { return std::unexpected(res.error()); }

        if (auto future = std::get_if<QFuture<typename T::Response>>(&res.value())) {
          return HRet(future->then(
              [](const T::Response &response) { return typename Schema::ResponseVariant(response); }));
        } else {
          return std::get<typename T::Response>(res.value());
        }
      } else {
        return std::unexpected("Mismatched request type");
      }
    };
  }

  void middleware(MiddlewareHandler fn) { m_middlewares.emplace_back(fn); }

  HRet call(const SchemaT::Request &req, typename Context::CallerContext &callerCtx) {
    ContextHandle handle(&m_ctx, &callerCtx, req.method);

    if (auto it = m_handlers.find(req.method); it != m_handlers.end()) {
      try {
        for (const auto &handler : m_middlewares) {
          if (const auto error = handler(req.data, handle)) { return std::unexpected(error.value()); }
        }
      } catch (const std::exception &e) { return std::unexpected(e.what()); }

      return it->second(req.data, handle);
    }

    return std::unexpected("No handler for method");
  }

private:
  Context::GlobalContext m_ctx;
  std::unordered_map<std::string, Handler> m_handlers;
  std::vector<MiddlewareHandler> m_middlewares;
};

/**
 * IPC server used to dispatch CLI commands and communicate with browser extensions.
 * Integrates with the main thread event loop.
 */
class IpcCommandServer : public QObject {
public:
  IpcCommandServer(ApplicationContext *ctx, QWidget *parent = nullptr);
  bool start(const std::filesystem::path &localPath);

private:
  void processFrame(QLocalSocket *conn, QByteArrayView frame);
  void handleRead(QLocalSocket *conn);
  void handleDisconnection(QLocalSocket *conn);
  void handleConnection();
  std::optional<QLocalSocket *> getConn();

  QLocalServer m_server;
  std::vector<ClientInfo> m_clients;

  RpcServer<ServerSchema, IpcContext> m_rpc;
  ApplicationContext &m_ctx;
};

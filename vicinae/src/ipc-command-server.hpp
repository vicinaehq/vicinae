#pragma once
#include "common/context.hpp"
#include <QLocalSocket>
#include <QFutureWatcher>
#include <cstdint>
#include <QDebug>
#include <format>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qlocalserver.h>
#include <string_view>
#include <vicinae-ipc/ipc.hpp>
#include "common/qt.hpp"
#include "types.hpp"

using ServerSchema = ipc::RpcSchema<ipc::DMenu, ipc::Deeplink, ipc::Ping, ipc::LaunchApp, ipc::BrowserInit,
                                    ipc::BrowserTabsChanged>;

using Watcher = QFutureWatcher<glz::raw_json>;

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

  using HRet = std::expected<PromiseLike<glz::raw_json>, std::string>;
  using Handler = std::function<HRet(const glz::raw_json &req, ContextHandle ctx)>;
  using MiddlewareHandler =
      std::function<std::optional<std::string>(const typename Schema::Request &request, ContextHandle ctx)>;

  template <ipc::InSchema<Schema> T>
  void route(std::function<std::expected<PromiseLike<typename T::Response>, std::string>(
                 const typename T::Request &reqData, ContextHandle ctx)>
                 fn) {
    std::string method = T::key;

    m_handlers[method] = [fn](const glz::raw_json &data, ContextHandle ctx) -> HRet {
      typename T::Request req;

      if (const auto error = glz::read_json(req, data.str)) {
        return std::unexpected(std::format("Failed to read request data: {}", glz::format_error(error)));
      }

      std::expected<PromiseLike<typename T::Response>, std::string> res = fn(req, ctx);

      if (!res) { return std::unexpected(res.error()); }

      if (auto future = std::get_if<QFuture<typename T::Response>>(&res.value())) {
        return HRet(future->then([](const T::Response &response) -> glz::raw_json {
          std::string json;
          if (const auto error = glz::write_json(response, json)) {
            qDebug() << std::format("Failed to read request data: {}", glz::format_error(error));
          }
          return json;
        }));
      } else {
        std::string json;
        if (const auto error = glz::write_json(std::get<typename T::Response>(res.value()), json)) {
          qDebug() << std::format("Failed to read request data: {}", glz::format_error(error));
        }
        return json;
      }
    };
  }

  void middleware(MiddlewareHandler fn) { m_middlewares.emplace_back(fn); }

  HRet call(const SchemaT::Request &req, typename Context::CallerContext &callerCtx) {
    ContextHandle handle(&m_ctx, &callerCtx, req.method);

    if (auto it = m_handlers.find(req.method); it != m_handlers.end()) {
      try {
        for (const auto &handler : m_middlewares) {
          if (const auto error = handler(req, handle)) { return std::unexpected(error.value()); }
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

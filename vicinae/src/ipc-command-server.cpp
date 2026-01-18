#include "ipc-command-server.hpp"
#include "ipc-command-handler.hpp"
#include "services/browser-extension-service.hpp"
#include "ui/dmenu-view/dmenu-view.hpp"
#include "utils.hpp"
#include "vicinae-ipc/ipc.hpp"
#include <functional>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/write.hpp>
#include <netinet/in.h>
#include <qlogging.h>
#include <variant>
#include <glaze/rpc/registry.hpp>
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "navigation-controller.hpp"
#include "version.h"

IpcCommandServer::IpcCommandServer(ApplicationContext *ctx, QWidget *parent)
    : QObject(parent), m_ctx(*ctx), m_rpc(IpcContext::GlobalContext{.app = ctx}) {
  using Ctx = decltype(m_rpc)::ContextHandle;

  connect(
      ctx->services->browserExtension(), &BrowserExtensionService::tabActionRequested, this,
      [this](std::string_view browserId, int tabId, BrowserExtensionService::TabAction action) {
        auto it = std::ranges::find_if(
            m_clients, [&](auto &&client) { return client.browser && client.browser->id == browserId; });

        if (it == m_clients.end()) {
          qWarning() << "focus tab requested but there is no browser extension client currently connected";
          return;
        }

        ipc::RpcClient<ipc::RpcSchema<ipc::FocusTab, ipc::CloseBrowserTab>> client;
        std::string json;

        switch (action) {
        case BrowserExtensionService::TabAction::Focus:
          json = client.notify<ipc::FocusTab>({.tabId = tabId});
          break;
        case BrowserExtensionService::TabAction::Close:
          json = client.notify<ipc::CloseBrowserTab>({.tabId = tabId});
          break;
        }

        qDebug() << "send to browser" << json;

        it->sendMessage(json);
      });

  m_rpc.middleware([](const decltype(m_rpc)::Schema::Request &req, Ctx info) -> std::optional<std::string> {
    constexpr static const auto BROWSER_METHODS = std::array{ipc::BrowserTabsChanged::key};

    if (std::ranges::contains(BROWSER_METHODS, req.method) && !info.caller->browser) {
      return "Only browser extensions can do this";
    }

    return {};
  });

  m_rpc.route<ipc::Ping>([&](const ipc::Ping::Request &req, Ctx ctx) {
    return ipc::Ping::Response(VICINAE_GIT_TAG, QApplication::applicationPid());
  });

  m_rpc.route<ipc::BrowserInit>([this](const ipc::BrowserInit::Request &init, Ctx context) {
    context.caller->browser = init;
    m_ctx.services->browserExtension()->registerBrowser({.id = init.id, .name = init.name});
    return ipc::BrowserInit::Response();
  });

  m_rpc.route<ipc::LaunchApp>([](const ipc::LaunchApp::Request &req,
                                 Ctx ctx) -> std::expected<ipc::LaunchApp::Response, std::string> {
    auto appDb = ctx.global->app->services->appDb();
    auto wm = ctx.global->app->services->windowManager();
    auto app = appDb->findById(req.appId.c_str());

    if (!app) { return std::unexpected("No app with id"); }

    if (!req.newInstance) {
      if (auto wins = wm->findAppWindows(*app); !wins.empty()) {
        auto &win = wins.front();
        wm->provider()->focusWindowSync(*win);
        return ipc::LaunchApp::Response({.focusedWindowTitle = win->title().toStdString()});
      }
    }

    std::vector<QString> args = Utils::toQStringVec(req.args);

    if (!appDb->launch(*app, args)) {
      return std::unexpected(std::format("Failed to launch app with id {}", req.appId));
    }

    return ipc::LaunchApp::Response();
  });

  m_rpc.route<ipc::BrowserTabsChanged>(
      [](const ipc::BrowserTabsChanged::Request &req,
         Ctx ctx) -> std::expected<ipc::BrowserTabsChanged::Response, std::string> {
        qDebug() << "set" << req.size() << "browser tabs";
        if (!ctx.caller->browser) return std::unexpected("setTabs only permitted for browsers");
        ctx.global->app->services->browserExtension()->setTabs(ctx.caller->browser->id, req);
        return ipc::BrowserTabsChanged::Response();
      });

  m_rpc.route<ipc::DMenu>([](ipc::DMenu::Request request, Ctx ctx) {
    using Watcher = QFutureWatcher<ipc::DMenu::Response>;
    static constexpr const int DMENU_SMALL_WIDTH_THRESHOLD = 500;
    auto &m_ctx = *ctx.global->app;

    auto &nav = m_ctx.navigation;
    auto &cfg = m_ctx.services->config()->value();

    QPromise<ipc::DMenu::Response> promise;
    auto future = promise.future();

    if (request.width.has_value() && request.width.value() < DMENU_SMALL_WIDTH_THRESHOLD) {
      qInfo() << "dmenu: disabling quicklook and footer because width is too low";
      request.noFooter = true;
      request.noQuickLook = true;
    }

    auto view = new DMenu::View(request);
    auto watcher = new Watcher;

    watcher->setFuture(future);

    QObject::connect(watcher, &Watcher::canceled, [nav = nav.get()]() { nav->closeWindow(); });
    QObject::connect(watcher, &Watcher::finished, [watcher]() { watcher->deleteLater(); });
    QObject::connect(view, &DMenu::View::selected,
                     [promise = std::move(promise)](const QString &text) mutable {
                       promise.addResult(ipc::DMenu::Response(text.toStdString()));
                       promise.finish();
                     });

    nav->popToRoot({.clearSearch = false});
    nav->pushView(view);
    nav->setInstantDismiss(true);

    if (request.width || request.height) {
      int w = request.width.value_or(cfg.launcherWindow.size.width);
      int h = request.height.value_or(cfg.launcherWindow.size.height);
      nav->requestWindowSize(QSize(w, h));
    }

    nav->showWindow();

    return future;
  });

  m_rpc.route<ipc::Deeplink>(
      [](const ipc::Deeplink::Request &req, Ctx ctx) -> std::expected<ipc::Deeplink::Response, std::string> {
        IpcCommandHandler handler(*ctx.global->app);
        QUrl url(req.url.c_str());

        if (!url.isValid()) { return std::unexpected("Not a valid URL"); }

        const auto res = handler.handleUrl(QUrl(req.url.c_str()));

        if (!res) return std::unexpected(res.error());

        return ipc::Deeplink::Response();
      });
}

void IpcCommandServer::processFrame(QLocalSocket *conn, QByteArrayView frame) {

  auto clientInfoIt =
      std::ranges::find_if(m_clients, [conn](const ClientInfo &info) { return info.conn == conn; });

  auto respond = [](QLocalSocket *conn, ServerSchema::Response res) {
    std::string buf;

    if (const auto error = glz::write_json(res, buf)) {
      qCritical() << "Failed to serialize ipc response";
      return;
    }

    qDebug() << "IPC Response" << buf;

    uint32_t size = buf.size();
    conn->write(reinterpret_cast<const char *>(&size), sizeof(size));
    conn->write(reinterpret_cast<const char *>(buf.data()), buf.size());
  };

  if (clientInfoIt == m_clients.end()) return;

  qDebug() << "IPC Request" << frame.toByteArray().toStdString();

  const auto parseResult = ServerSchema::parseRequest(frame.toByteArray().toStdString());

  if (!parseResult) {
    qWarning() << "Failed to parse";
    return;
  }

  const auto &request = parseResult.value();

  auto call = m_rpc.call(request, *clientInfoIt);

  if (!request.id) { return; }

  if (!call) {
    respond(conn, request.makeErrorResponse({.code = 0, .message = call.error()}));
    return;
  }

  const auto &value = call.value();

  if (auto future = std::get_if<QFuture<glz::raw_json>>(&value)) {
    auto watcher = QObjectUniquePtr<Watcher>(new Watcher);

    watcher->setFuture(*future);
    // TODO: we must check that connection still exist
    connect(watcher.get(), &Watcher::finished, this, [respond, watcher = watcher.get(), request, conn]() {
      if (watcher->isCanceled()) { return; }

      std::string packet;
      auto response = request.makeResponse(watcher->result());

      if (const auto error = glz::write_json(response, packet)) {
        qDebug() << "Failed to write response" << glz::format_error(error);
      }

      respond(conn, response);
    });
    clientInfoIt->m_pending.emplace_back(std::move(watcher));

    return;
  }

  respond(conn, request.makeResponse(std::get<glz::raw_json>(value)));
}

void IpcCommandServer::handleRead(QLocalSocket *conn) {
  auto it = std::ranges::find_if(m_clients, [conn](const ClientInfo &info) { return info.conn == conn; });

  if (it == m_clients.end()) {
    qWarning() << "CommandServer::handleRead: could not find client info";
    conn->disconnect();
    return;
  }

  while (conn->bytesAvailable() > 0) {
    it->frame.data.append(conn->readAll());

    while (it->frame.data.size() >= sizeof(uint32_t)) {
      uint32_t length = *reinterpret_cast<uint32_t *>(it->frame.data.data());
      bool isComplete = it->frame.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = QByteArrayView(it->frame.data).sliced(sizeof(uint32_t), length);

      processFrame(conn, packet);

      it->frame.data = it->frame.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

void IpcCommandServer::handleDisconnection(QLocalSocket *conn) {
  auto it = std::ranges::find_if(m_clients, [conn](const ClientInfo &info) { return info.conn == conn; });

  for (const auto &watcher : it->m_pending) {
    if (!watcher->isFinished()) { watcher->cancel(); }
  }

  if (it->browser) { m_ctx.services->browserExtension()->unregisterBrowser(it->browser->id); }

  m_clients.erase(it);
  conn->deleteLater();
}

void IpcCommandServer::handleConnection() {
  QLocalSocket *conn = m_server.nextPendingConnection();

  qDebug() << "ipc command server received new connection";

  m_clients.push_back({.conn = conn});
  connect(conn, &QLocalSocket::disconnected, this, [this, conn]() { handleDisconnection(conn); });
  connect(conn, &QLocalSocket::readyRead, this, [this, conn]() { handleRead(conn); });
}

bool IpcCommandServer::start(const std::filesystem::path &localPath) {

  if (std::filesystem::exists(localPath)) { std::filesystem::remove(localPath); }

  if (!m_server.listen(localPath.c_str())) {
    qDebug() << "CommandServer failed to listen" << m_server.errorString();
    return false;
  }

  connect(&m_server, &QLocalServer::newConnection, this, &IpcCommandServer::handleConnection);

  qDebug() << "Server started, listening on:" << localPath.c_str();

  return true;
}

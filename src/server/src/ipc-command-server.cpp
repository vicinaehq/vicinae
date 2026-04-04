#include "ipc-command-server.hpp"
#include "ipc-command-handler.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/browser-extension-service.hpp"
#include "qml/dmenu-view-host.hpp"
#include "utils.hpp"
#include <qlogging.h>
#include <utility>
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "navigation-controller.hpp"
#include "generated/version.h"

// IpcTransport

void IpcCommandServer::IpcTransport::send(std::string_view data) {
  if (!conn) {
    qWarning() << "IPC transport send called with null conn!";
    return;
  }
  uint32_t size = data.size();
  conn->write(reinterpret_cast<const char *>(&size), sizeof(size));
  conn->write(data.data(), data.size());
}

// IpcService

IpcService::IpcService(ipc_gen::RpcTransport &transport, ApplicationContext &ctx)
    : ipc_gen::AbstractIpc(transport), m_ctx(ctx) {}

ipc_gen::Result<ipc_gen::PingResponse>::Future IpcService::ping() {
  return ipc_gen::Result<ipc_gen::PingResponse>::ok(
      {.version = VICINAE_GIT_TAG, .pid = static_cast<int>(QCoreApplication::applicationPid())});
}

ipc_gen::Result<ipc_gen::DeeplinkResponse>::Future IpcService::deeplink(ipc_gen::DeeplinkRequest req) {
  IpcCommandHandler handler(m_ctx);
  QUrl const url(req.url.c_str());

  if (!url.isValid()) return ipc_gen::Result<ipc_gen::DeeplinkResponse>::fail("Not a valid URL");

  const auto res = handler.handleUrl(url);
  if (!res) return ipc_gen::Result<ipc_gen::DeeplinkResponse>::fail(res.error());

  return ipc_gen::Result<ipc_gen::DeeplinkResponse>::ok({});
}

ipc_gen::Result<ipc_gen::DescribeResponse>::Future IpcService::describe() {
  return ipc_gen::Result<ipc_gen::DescribeResponse>::ok(
      {.open = m_ctx.navigation->isWindowOpened(),
       .entrypoint = m_ctx.navigation->activeCommand()->uniqueId()});
}

ipc_gen::Result<ipc_gen::LaunchAppResponse>::Future IpcService::launchApp(ipc_gen::LaunchAppRequest req) {
  auto appDb = m_ctx.services->appDb();
  auto wm = m_ctx.services->windowManager();
  auto app = appDb->findById(req.appId.c_str());

  if (!app) return ipc_gen::Result<ipc_gen::LaunchAppResponse>::fail("No app with id");

  if (!req.newInstance) {
    if (auto wins = wm->findAppWindows(*app); !wins.empty()) {
      auto &win = wins.front();
      wm->provider()->focusWindowSync(*win);
      return ipc_gen::Result<ipc_gen::LaunchAppResponse>::ok(
          {.focusedWindowTitle = win->title().toStdString()});
    }
  }

  std::vector<QString> const args = Utils::toQStringVec(req.args);

  if (!appDb->launch(*app, args))
    return ipc_gen::Result<ipc_gen::LaunchAppResponse>::fail(
        std::format("Failed to launch app with id {}", req.appId));

  return ipc_gen::Result<ipc_gen::LaunchAppResponse>::ok({});
}

ipc_gen::Result<ipc_gen::DMenuResponse>::Future IpcService::dmenu(ipc_gen::DMenuRequest request) {
  static constexpr const int DMENU_SMALL_WIDTH_THRESHOLD = 500;
  auto &nav = m_ctx.navigation;
  auto &cfg = m_ctx.services->config()->value();

  QPromise<ipc_gen::Result<ipc_gen::DMenuResponse>::Type> promise;
  auto future = promise.future();

  if (request.width.has_value() && request.width.value() < DMENU_SMALL_WIDTH_THRESHOLD) {
    qInfo() << "dmenu: disabling quicklook and footer because width is too low";
    request.noFooter = true;
    request.noQuickLook = true;
  }

  // Convert to the DMenu-specific request type used by the view host
  ipc_gen::DMenuRequest viewReq = std::move(request);
  auto view = new DMenuViewHost(viewReq);

  using Watcher = QFutureWatcher<ipc_gen::Result<ipc_gen::DMenuResponse>::Type>;
  auto watcher = new Watcher;
  watcher->setFuture(future);

  QObject::connect(watcher, &Watcher::canceled, [nav = nav.get()]() { nav->closeWindow(); });
  QObject::connect(watcher, &Watcher::finished, [watcher]() { watcher->deleteLater(); });
  QObject::connect(view, &DMenuViewHost::selected,
                   [promise = std::move(promise)](const QString &text) mutable {
                     promise.addResult(ipc_gen::DMenuResponse{.output = text.toStdString()});
                     promise.finish();
                   });

  nav->popToRoot({.clearSearch = false});
  nav->pushView(view);
  nav->setInstantDismiss(true);

  if (request.width || request.height) {
    int const w = request.width.value_or(cfg.launcherWindow.size.width);
    int const h = request.height.value_or(cfg.launcherWindow.size.height);
    nav->requestWindowSize(QSize(w, h));
  }

  nav->showWindow();

  return future;
}

ipc_gen::Result<void>::Future IpcService::browserInit(ipc_gen::BrowserInitRequest req) {
  if (!m_caller) return ipc_gen::Result<void>::fail("No caller context");

  m_caller->browser = req;
  m_ctx.services->browserExtension()->registerBrowser({.id = req.id, .name = req.name});

  return ipc_gen::Result<void>::ok();
}

ipc_gen::Result<void>::Future IpcService::browserTabsChanged(std::vector<ipc_gen::BrowserTabInfo> tabs) {
  if (!m_caller || !m_caller->browser)
    return ipc_gen::Result<void>::fail("Only browser extensions can do this");

  m_ctx.services->browserExtension()->setTabs(m_caller->browser->id, tabs);

  return ipc_gen::Result<void>::ok();
}

// IpcCommandServer

IpcCommandServer::IpcCommandServer(ApplicationContext *ctx, QObject *parent)
    : QObject(parent), m_ctx(*ctx), m_rpc(m_transport), m_service(m_rpc, *ctx),
      m_ipcServer(m_rpc, &m_service) {

  connect(
      ctx->services->browserExtension(), &BrowserExtensionService::tabActionRequested, this,
      [this](std::string_view browserId, int tabId, BrowserExtensionService::TabAction action) {
        auto it = std::ranges::find_if(
            m_clients, [&](auto &&client) { return client.browser && client.browser->id == browserId; });

        if (it == m_clients.end()) {
          qWarning() << "focus tab requested but there is no browser extension client currently connected";
          return;
        }

        m_transport.conn = it->conn;

        switch (action) {
        case BrowserExtensionService::TabAction::Focus:
          m_service.emitfocusTab({.tabId = tabId});
          break;
        case BrowserExtensionService::TabAction::Close:
          m_service.emitcloseTab({.tabId = tabId});
          break;
        }

        m_transport.conn = nullptr;
      });
}

void IpcCommandServer::processFrame(QLocalSocket *conn, QByteArrayView frame) {
  auto clientInfoIt =
      std::ranges::find_if(m_clients, [conn](const ClientInfo &info) { return info.conn == conn; });

  if (clientInfoIt == m_clients.end()) return;

  m_transport.conn = conn;
  m_service.setCallerInfo(&(*clientInfoIt));

  qDebug() << "IPC incoming:" << frame.toByteArray();

  m_ipcServer.route({frame.data(), static_cast<size_t>(frame.size())});
  m_service.setCallerInfo(nullptr);
  m_transport.conn = nullptr;
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

    while (std::cmp_greater_equal(it->frame.data.size(), sizeof(uint32_t))) {
      uint32_t const length = *reinterpret_cast<uint32_t *>(it->frame.data.data());
      bool const isComplete = it->frame.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = QByteArrayView(it->frame.data).sliced(sizeof(uint32_t), length);

      processFrame(conn, packet);

      it->frame.data = it->frame.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

void IpcCommandServer::handleDisconnection(QLocalSocket *conn) {
  auto it = std::ranges::find_if(m_clients, [conn](const ClientInfo &info) { return info.conn == conn; });

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

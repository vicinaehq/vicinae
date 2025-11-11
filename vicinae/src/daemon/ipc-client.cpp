#include "ipc-client.hpp"
#include "proto/daemon.pb.h"
#include "ui/dmenu-view/dmenu-view.hpp"
#include "vicinae.hpp"
#include <chrono>
#include <iostream>
#include <qlocalsocket.h>
#include <random>
#include <stdexcept>

namespace Daemon = proto::ext::daemon;

class FailedToConnectException : public std::exception {
  const char *what() const noexcept override {
    return "Failed to connect to vicinae server. You should first start the vicinae "
           "server using the `vicinae server` command.";
  }
};

void DaemonIpcClient::writeRequest(const Daemon::Request &req) {
  std::string data;
  QByteArray message;
  QDataStream dataStream(&message, QIODevice::WriteOnly);

  req.SerializeToString(&data);
  dataStream << QByteArray(data.data(), data.size());
  m_conn.write(message);
  m_conn.waitForBytesWritten(1000);
}

void DaemonIpcClient::launchApp(const std::string &id, const std::vector<std::string> &args,
                                bool newInstance) {
  Daemon::Request req;
  auto launchReq = new proto::ext::daemon::LaunchAppRequest;

  launchReq->set_app_id(id);
  launchReq->set_new_instance(newInstance);

  for (const auto &arg : args) {
    launchReq->add_args(arg);
  }

  req.set_allocated_launch_app(launchReq);
  auto res = request(req);
  auto launchRes = res.launch_app();

  if (auto str = launchRes.error(); !str.empty()) { throw std::runtime_error(launchRes.error()); }
  if (auto focused = launchRes.focused_window_title(); !focused.empty()) {
    std::cerr << "Focused existing window: " << std::quoted(focused)
              << "\nPass --new if you want to spawn up a new instance every time." << std::endl;
  }
}

std::vector<proto::ext::daemon::AppInfo> DaemonIpcClient::listApps(bool withActions) {
  Daemon::Request req;
  auto listReq = new proto::ext::daemon::ListAppsRequest;

  listReq->set_with_actions(withActions);

  req.set_allocated_list_apps(listReq);
  auto res = request(req);
  auto listRes = res.list_apps();

  std::vector<proto::ext::daemon::AppInfo> apps;
  apps.reserve(listRes.apps_size());

  for (const auto &app : listRes.apps()) {
    apps.push_back(app);
  }

  return apps;
}

void DaemonIpcClient::toggle(const DaemonIpcClient::ToggleSettings &settings) {
  QUrl url;
  QUrlQuery query;

  if (settings.query) { query.addQueryItem("fallbackText", settings.query->c_str()); }

  url.setScheme(Omnicast::APP_SCHEME);
  url.setHost("toggle");
  url.setQuery(query);

  if (auto res = deeplink(url); !res) {
    throw std::runtime_error("Failed to toggle: " + res.error().toStdString());
  }
}

bool DaemonIpcClient::open(const OpenSettings &settings) {
  QUrl url;
  QUrlQuery query;

  if (settings.query) { query.addQueryItem("fallbackText", settings.query->c_str()); }

  url.setScheme(Omnicast::APP_SCHEME);
  url.setHost("open");
  url.setQuery(query);

  return deeplink(url).has_value();
}

bool DaemonIpcClient::close() {
  QUrl url;
  url.setScheme(Omnicast::APP_SCHEME);
  url.setHost("close");
  return deeplink(url).has_value();
}

proto::ext::daemon::Response DaemonIpcClient::request(const proto::ext::daemon::Request &req) {
  using namespace std::chrono_literals;
  constexpr const size_t timeout = std::chrono::duration_cast<std::chrono::milliseconds>(1min).count();

  if (m_conn.state() != QLocalSocket::LocalSocketState::ConnectedState) { connectOrThrow(); }

  writeRequest(req);
  if (!m_conn.waitForReadyRead(timeout)) { throw std::runtime_error("DMenu request timed out"); }

  auto buffer = m_conn.readAll();
  Daemon::Response res;

  if (!res.ParseFromArray(buffer.data(), buffer.size())) {
    throw std::runtime_error("failed to parse response");
  }

  return res;
}

bool DaemonIpcClient::ping() {
  proto::ext::daemon::Request req;
  auto pingReq = new proto::ext::daemon::PingRequest;

  req.set_allocated_ping(pingReq);

  return request(req).payload_case() == proto::ext::daemon::Response::kPing;
}

std::string DaemonIpcClient::dmenu(const DMenu::Payload &payload) {
  Daemon::Request req;
  req.set_allocated_dmenu(new proto::ext::daemon::DmenuRequest(payload.toProto()));
  auto res = request(req);
  return res.dmenu().output();
}

tl::expected<void, QString> DaemonIpcClient::deeplink(const QUrl &url) {
  proto::ext::daemon::Request req;
  auto urlReq = new Daemon::UrlRequest();

  urlReq->set_url(url.toString().toStdString());
  req.set_allocated_url(urlReq);
  auto res = request(req);

  if (auto error = res.url().error(); !error.empty()) { return tl::unexpected(error.c_str()); }
  return {};
}

void DaemonIpcClient::connectOrThrow() {
  if (!connect()) throw FailedToConnectException();
}

bool DaemonIpcClient::connect() { return m_conn.waitForConnected(1000); }

DaemonIpcClient::DaemonIpcClient() { m_conn.connectToServer(Omnicast::commandSocketPath().c_str()); }

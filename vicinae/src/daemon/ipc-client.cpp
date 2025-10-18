#include "ipc-client.hpp"
#include "proto/daemon.pb.h"
#include "vicinae.hpp"
#include <qlocalsocket.h>
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

void DaemonIpcClient::toggle() {
  QUrl url;

  url.setScheme(Omnicast::APP_SCHEME);
  url.setHost("toggle");
  if (auto res = deeplink(url); !res) {
    throw std::runtime_error("Failed to toggle: " + res.error().toStdString());
  }
}

proto::ext::daemon::Response DaemonIpcClient::request(const proto::ext::daemon::Request &req) {
  if (m_conn.state() != QLocalSocket::LocalSocketState::ConnectedState) { connectOrThrow(); }

  writeRequest(req);
  m_conn.waitForReadyRead();

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

std::string DaemonIpcClient::dmenu(DMenuListView::DmenuPayload payload) {
  Daemon::Request req;
  auto dmenuReq = new proto::ext::daemon::DmenuRequest;

  dmenuReq->set_navigation_title(payload.navigationTitle);
  dmenuReq->set_raw_content(payload.raw);
  dmenuReq->set_placeholder(payload.placeholder);
  dmenuReq->set_section_title(payload.sectionTitle);
  dmenuReq->set_no_section(payload.noSection);
  req.set_allocated_dmenu(dmenuReq);

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

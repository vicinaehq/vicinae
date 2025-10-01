#include "ipc-client.hpp"
#include "proto/daemon.pb.h"
#include "vicinae.hpp"

void DaemonIpcClient::writeRequest(const proto::ext::daemon::Request &req) {
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
  sendDeeplink(url);
}

std::optional<std::string> DaemonIpcClient::dmenu(DmenuPayload payload) {
  auto req = new proto::ext::daemon::Request;
  auto dmenuReq = new proto::ext::daemon::DmenuRequest;

  dmenuReq->set_navigation_title(payload.navigationTitle);
  dmenuReq->set_raw_content(payload.raw);
  dmenuReq->set_placeholder(payload.placeholder);
  dmenuReq->set_section_title(payload.sectionTitle);
  dmenuReq->set_no_section(payload.noSection);
  req->set_allocated_dmenu(dmenuReq);
  writeRequest(*req);

  m_conn.waitForReadyRead();
  auto buffer = m_conn.readAll();

  proto::ext::daemon::Response res;

  if (!res.ParseFromArray(buffer.data(), buffer.size())) {
    std::cerr << "failed to parse response";
    return {};
  }

  std::string out = res.dmenu().output();

  if (out.empty()) return {};

  return out;
}

void DaemonIpcClient::sendDeeplink(const QUrl &url) {
  proto::ext::daemon::Request req;
  auto urlReq = new proto::ext::daemon::UrlRequest();

  urlReq->set_url(url.toString().toStdString());
  req.set_allocated_url(urlReq);
  writeRequest(req);
}

bool DaemonIpcClient::connect() { return m_conn.waitForConnected(1000); }

DaemonIpcClient::DaemonIpcClient() { m_conn.connectToServer(Omnicast::commandSocketPath().c_str()); }

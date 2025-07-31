#include "ipc-client.hpp"
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
  proto::ext::daemon::Request req;

  req.set_allocated_toggle(new proto::ext::daemon::ToggleRequest);
  writeRequest(req);
}

void DaemonIpcClient::ping() {
  proto::ext::daemon::Request req;

  req.set_allocated_ping(new proto::ext::daemon::PingRequest);
  writeRequest(req);
}

void DaemonIpcClient::passUrl(const QUrl &url) {
  proto::ext::daemon::Request req;
  auto urlReq = new proto::ext::daemon::UrlRequest();

  urlReq->set_url(url.toString().toStdString());
  req.set_allocated_url(urlReq);
  writeRequest(req);
}

bool DaemonIpcClient::connect() { return m_conn.waitForConnected(1000); }

DaemonIpcClient::DaemonIpcClient() { m_conn.connectToServer(Omnicast::commandSocketPath().c_str()); }

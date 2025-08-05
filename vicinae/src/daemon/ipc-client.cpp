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
  QUrl url;

  url.setScheme(Omnicast::APP_SCHEME);
  url.setHost("toggle");
  passUrl(url);
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

#include "ipc-command-server.hpp"
#include "common.hpp"
#include "proto/daemon.pb.h"
#include <qfuturewatcher.h>
#include <qlocalsocket.h>
#include <qlogging.h>

void IpcCommandServer::processFrame(QLocalSocket *conn, QByteArrayView frame) {
  auto clientInfoIt = std::find_if(_clients.begin(), _clients.end(),
                                   [conn](const ClientInfo &info) { return info.conn == conn; });

  if (clientInfoIt == _clients.end()) return;

  proto::ext::daemon::Request req;

  req.ParseFromString(frame.toByteArray().toStdString());

  if (!_handler) {
    qWarning() << "no handler was configured";
    return;
  }

  auto handlerResult = _handler->handleCommand(req);

  if (auto future = std::get_if<QFuture<proto::ext::daemon::Response *>>(&handlerResult)) {
    auto watcher = QObjectUniquePtr<Watcher>(new Watcher);

    watcher->setFuture(*future);
    connect(watcher.get(), &Watcher::finished, this, [watcher = watcher.get(), conn]() {
      if (watcher->isCanceled()) { return; }

      std::string packet;
      auto result = watcher->result();

      result->SerializeToString(&packet);
      conn->write(packet.data(), packet.size());
    });
    clientInfoIt->m_pending.emplace_back(std::move(watcher));

    return;
  }

  auto result = std::get<proto::ext::daemon::Response *>(handlerResult);
  std::string packet;

  result->SerializeToString(&packet);
  conn->write(packet.data(), packet.size());
}

void IpcCommandServer::handleRead(QLocalSocket *conn) {
  auto it = std::find_if(_clients.begin(), _clients.end(),
                         [conn](const ClientInfo &info) { return info.conn == conn; });

  if (it == _clients.end()) {
    qWarning() << "CommandServer::handleRead: could not find client info";
    conn->disconnect();
    return;
  }

  while (conn->bytesAvailable() > 0) {
    it->frame.data.append(conn->readAll());

    while (it->frame.data.size() >= sizeof(uint32_t)) {
      uint32_t length = ntohl(*reinterpret_cast<uint32_t *>(it->frame.data.data()));
      bool isComplete = it->frame.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = QByteArrayView(it->frame.data).sliced(sizeof(uint32_t), length);

      processFrame(conn, packet);

      it->frame.data = it->frame.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

void IpcCommandServer::handleDisconnection(QLocalSocket *conn) {
  auto it = std::find_if(_clients.begin(), _clients.end(),
                         [conn](const ClientInfo &info) { return info.conn == conn; });

  for (const auto &watcher : it->m_pending) {
    if (!watcher->isFinished()) { watcher->cancel(); }
  }

  _clients.erase(it);
  conn->deleteLater();
}

void IpcCommandServer::handleConnection() {
  QLocalSocket *conn = _server->nextPendingConnection();

  _clients.push_back({.conn = conn});
  connect(conn, &QLocalSocket::disconnected, this, [this, conn]() { handleDisconnection(conn); });
  connect(conn, &QLocalSocket::readyRead, this, [this, conn]() { handleRead(conn); });
}

bool IpcCommandServer::start(const std::filesystem::path &localPath) {
  if (std::filesystem::exists(localPath)) { std::filesystem::remove(localPath); }

  if (!_server->listen(localPath.c_str())) {
    qDebug() << "CommandServer failed to listen" << _server->errorString();
    return false;
  }

  connect(_server, &QLocalServer::newConnection, this, &IpcCommandServer::handleConnection);

  qDebug() << "Server started, listening on:" << localPath.c_str();

  return true;
}

void IpcCommandServer::setHandler(ICommandHandler *handler) { _handler = handler; }

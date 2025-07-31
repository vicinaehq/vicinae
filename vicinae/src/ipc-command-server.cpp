#include "ipc-command-server.hpp"
#include "proto/daemon.pb.h"
#include <qlogging.h>

void IpcCommandServer::writeResponse(QLocalSocket *conn, const Proto::Variant &data) {
  Proto::Marshaler marshaler;
  std::vector<uint8_t> frame(marshaler.marshalSized(data));

  conn->write(reinterpret_cast<const char *>(frame.data()), frame.size());
}

void IpcCommandServer::writeError(QLocalSocket *conn, const CommandError &error) {
  writeResponse(conn, Proto::Array{Proto::Int(CommandErrorStatus), error.error});
}

void IpcCommandServer::writeSuccess(QLocalSocket *conn, const CommandResponse &res) {
  writeResponse(conn, Proto::Array{Proto::Int(CommandOkStatus), res});
}

void IpcCommandServer::processFrame(QLocalSocket *conn, QByteArrayView frame) {

  proto::ext::daemon::Request req;

  qDebug() << "processing frame";

  req.ParseFromString(frame.toByteArray().toStdString());

  if (!_handler) {
    qCritical() << "no handler was configured";
    return;
  }

  auto handlerResult = _handler->handleCommand(req);
  std::string packet;

  handlerResult->SerializeToString(&packet);
  conn->write(packet.data(), packet.size());
}

void IpcCommandServer::handleRead(QLocalSocket *conn) {
  auto it = std::find_if(_clients.begin(), _clients.end(),
                         [conn](const ClientInfo &info) { return info.conn == conn; });

  if (it == _clients.end()) {
    qDebug() << "CommandServer::handleRead: could not find client info";
    conn->disconnect();
    return;
  }

  while (conn->bytesAvailable() > 0) {
    it->frame.data.append(conn->readAll());

    while (it->frame.data.size() >= sizeof(uint32_t)) {
      uint32_t length = ntohl(*reinterpret_cast<uint32_t *>(it->frame.data.data()));
      bool isComplete = it->frame.data.size() - sizeof(uint32_t) >= length;

      qDebug() << "read data, expected size" << length << "got" << it->frame.data.size();

      if (!isComplete) break;

      auto packet = QByteArrayView(it->frame.data).sliced(sizeof(uint32_t), length);

      processFrame(conn, packet);

      it->frame.data = it->frame.data.sliced(sizeof(uint32_t) + length);

      qDebug() << "data after processing" << it->frame.data.size();
    }
  }
}

void IpcCommandServer::handleDisconnection(QLocalSocket *conn) {
  auto it = std::find_if(_clients.begin(), _clients.end(),
                         [conn](const ClientInfo &info) { return info.conn == conn; });

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

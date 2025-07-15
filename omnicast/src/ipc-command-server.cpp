#include "ipc-command-server.hpp"

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
  Proto::Marshaler marshaler;
  std::vector<uint8_t> packet(frame.begin(), frame.end());
  auto result = marshaler.unmarshal<Proto::Array>(packet);

  if (auto err = std::get_if<Proto::Marshaler::Error>(&result)) {
    qDebug() << "Failed message unmarshaling, message discarded" << err->message;
    return;
  }

  auto message = std::get<Proto::Array>(result);

  if (message.size() != 2) {
    qDebug() << "Invalid message. Expected 2 arguments got" << message.size();
    return;
  }

  CommandMessage cmd{.type = message[0].asString(), .params = message[1]};

  if (!_handler) {
    writeError(conn, {.error = "No handler configured on the server side"});
    return;
  }

  auto handlerResult = _handler->handleCommand(cmd);

  if (auto error = std::get_if<CommandError>(&handlerResult)) {
    writeError(conn, *error);
  } else if (auto res = std::get_if<CommandResponse>(&handlerResult)) {
    writeSuccess(conn, *res);
  }
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

      if (!isComplete) break;

      auto packet = QByteArrayView(it->frame.data).sliced(sizeof(uint32_t), length);

      processFrame(conn, packet);

      it->frame.data = it->frame.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

void IpcCommandServer::handleDisconnection(QLocalSocket *conn) {
  auto it = std::remove_if(_clients.begin(), _clients.end(),
                           [conn](const ClientInfo &info) { return info.conn == conn; });

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

#pragma once
#include "proto.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <netinet/in.h>
#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qwidget.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

struct CommandMessage {
  std::string type;
  Proto::Variant params;
};

struct ClientInfo {
  QLocalSocket *conn;
  struct {
    QByteArray data;
    uint32_t length;
  } frame;
};

using CommandResponse = Proto::Variant;

struct CommandError {
  std::string error;
};

enum CommandResponseStatus {
  CommandOkStatus,
  CommandErrorStatus,
};

class ICommandHandler {
public:
  virtual std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) = 0;
};

class CommandServer : public QObject {
  ICommandHandler *_handler;
  QLocalServer *_server;
  std::vector<ClientInfo> _clients;

  void writeResponse(QLocalSocket *conn, const Proto::Variant &data) {
    Proto::Marshaler marshaler;
    std::vector<uint8_t> frame(marshaler.marshalSized(data));

    conn->write(reinterpret_cast<const char *>(frame.data()), frame.size());
  }

  void writeError(QLocalSocket *conn, const CommandError &error) {
    writeResponse(conn, Proto::Array{Proto::Int(CommandErrorStatus), error.error});
  }

  void writeSuccess(QLocalSocket *conn, const CommandResponse &res) {
    writeResponse(conn, Proto::Array{Proto::Int(CommandOkStatus), res});
  }

  void processFrame(QLocalSocket *conn, QByteArrayView frame) {
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

  void handleRead(QLocalSocket *conn) {
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

  void handleDisconnection(QLocalSocket *conn) {
    auto it = std::remove_if(_clients.begin(), _clients.end(),
                             [conn](const ClientInfo &info) { return info.conn == conn; });

    conn->deleteLater();
  }

  void handleConnection() {
    QLocalSocket *conn = _server->nextPendingConnection();

    _clients.push_back({.conn = conn});
    connect(conn, &QLocalSocket::disconnected, this, [this, conn]() { handleDisconnection(conn); });
    connect(conn, &QLocalSocket::readyRead, this, [this, conn]() { handleRead(conn); });
  }

public:
  CommandServer(QWidget *parent = nullptr) : QObject(parent), _server(new QLocalServer(this)) {}
  ~CommandServer() {}

  bool start(const std::filesystem::path &localPath) {
    if (std::filesystem::exists(localPath)) { std::filesystem::remove(localPath); }

    if (!_server->listen(localPath.c_str())) {
      qDebug() << "CommandServer failed to listen" << _server->errorString();
      return false;
    }

    connect(_server, &QLocalServer::newConnection, this, &CommandServer::handleConnection);

    qDebug() << "Server started, listening on:" << localPath.c_str();

    return true;
  }

  void setHandler(ICommandHandler *handler) { _handler = handler; }
};

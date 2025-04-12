#pragma once
#include "proto.hpp"
#include <cerrno>
#include <cstdint>
#include <memory>
#include <netinet/in.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
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
  QSocketNotifier *notifier;
  size_t messageSize;
  std::vector<uint8_t> buffer;
  char tmpbuf[8096];

  ClientInfo(QSocketNotifier *notif) : notifier(notif), messageSize(0) {}
  ~ClientInfo() {
    int fd = notifier->socket();

    delete notifier;
    close(fd);
  }
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
  qintptr _serverFd;
  QSocketNotifier *_notifier;
  ICommandHandler *_handler;
  std::unordered_map<int, std::unique_ptr<ClientInfo>> _clients;

  void writeResponse(int fd, const Proto::Variant &data) {
    Proto::Marshaler marshaler;
    std::vector<uint8_t> frame;
    auto message = marshaler.marshal(data);
    auto length = message.size();

    frame.push_back((length >> 24) & 0xFF);
    frame.push_back((length >> 16) & 0xFF);
    frame.push_back((length >> 8) & 0xFF);
    frame.push_back(length & 0xFF);
    frame.insert(frame.end(), message.begin(), message.end());
    send(fd, frame.data(), frame.size(), 0);
  }

  void writeError(int fd, const CommandError &error) {
    writeResponse(fd, Proto::Array{Proto::Int(CommandErrorStatus), error.error});
  }

  void writeSuccess(int fd, const CommandResponse &res) {
    writeResponse(fd, Proto::Array{Proto::Int(CommandOkStatus), res});
  }

  void handleClientData(int clientFd) {
    auto it = _clients.find(clientFd);

    if (it == _clients.end()) { return; }

    auto &client = it->second;
    int rc;

    while ((rc = recv(client->notifier->socket(), client->tmpbuf, sizeof(client->tmpbuf), 0)) > 0) {
      client->buffer.insert(client->buffer.end(), client->tmpbuf, client->tmpbuf + rc);
    }

    if (client->messageSize == 0 && client->buffer.size() > sizeof(uint32_t)) {
      client->messageSize = ntohl(*reinterpret_cast<uint32_t *>(&client->buffer[0]));
      qDebug() << "got message with size" << client->messageSize;
      client->buffer.erase(client->buffer.begin(), client->buffer.begin() + sizeof(uint32_t));
    }

    if (client->messageSize > 0 && client->buffer.size() >= client->messageSize) {
      Proto::Marshaler marshaler;
      auto result = marshaler.unmarshal<Proto::Array>(
          {client->buffer.begin(), client->buffer.begin() + client->messageSize});

      client->buffer.erase(client->buffer.begin(), client->buffer.begin() + client->messageSize);

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
        writeError(client->notifier->socket(), {.error = "No handler configured on the server side"});
        return;
      }

      auto handlerResult = _handler->handleCommand(cmd);

      if (auto error = std::get_if<CommandError>(&handlerResult)) {
        qDebug() << "write error";
        writeError(client->notifier->socket(), *error);
      } else if (auto res = std::get_if<CommandResponse>(&handlerResult)) {
        qDebug() << "write success";
        writeSuccess(client->notifier->socket(), *res);
      }

      client->messageSize = 0;

      //_clients.erase(client->notifier->socket());
    }
  }

  void handleNewConnection() {
    struct sockaddr_un clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);

    if (clientFd < 0) {
      qDebug() << "Failed to accept client connection:" << strerror(errno);
      return;
    }

    // Set client socket to non-blocking mode
    int flags = fcntl(clientFd, F_GETFL, 0);

    if (flags < 0 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0) {
      qDebug() << "Failed to set client socket to non-blocking mode:" << strerror(errno);
      close(clientFd);
      return;
    }

    qDebug() << "New client connected, fd:" << clientFd;

    QSocketNotifier *notifier = new QSocketNotifier(clientFd, QSocketNotifier::Read, this);

    connect(notifier, &QSocketNotifier::activated, this,
            [this, clientFd]() { this->handleClientData(clientFd); });

    _clients.insert({clientFd, std::make_unique<ClientInfo>(notifier)});
  }

public:
  CommandServer(QWidget *parent = nullptr) : QObject(parent) {}
  ~CommandServer() {
    delete _notifier;
    close(_serverFd);
  }

  bool start(const QString &localPath) {
    _serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_serverFd < 0) {
      qDebug() << "Failed to create socket:" << strerror(errno);
      return false;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(_serverFd, F_GETFL, 0);
    if (flags < 0 || fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK) < 0) {
      qDebug() << "Failed to set socket to non-blocking mode:" << strerror(errno);
      close(_serverFd);
      _serverFd = -1;
      return false;
    }

    // Remove any existing socket file
    unlink(localPath.toUtf8().constData());

    // Setup address structure
    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, localPath.toUtf8().constData(), sizeof(serverAddr.sun_path) - 1);

    // Bind the socket
    if (bind(_serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
      qDebug() << "Failed to bind socket:" << strerror(errno);
      close(_serverFd);
      _serverFd = -1;
      return false;
    }

    // Start listening
    if (listen(_serverFd, 5) < 0) {
      qDebug() << "Failed to listen on socket:" << strerror(errno);
      close(_serverFd);
      _serverFd = -1;
      return false;
    }

    // Create a QSocketNotifier for the server socket
    _notifier = new QSocketNotifier(_serverFd, QSocketNotifier::Read, this);

    connect(_notifier, &QSocketNotifier::activated, this, &CommandServer::handleNewConnection);

    qDebug() << "Server started, listening on:" << localPath;
    return true;
  }

  void setHandler(ICommandHandler *handler) { _handler = handler; }
};

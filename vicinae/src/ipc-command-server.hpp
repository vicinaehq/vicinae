#pragma once
#include "proto/daemon.pb.h"
#include <cstdint>
#include <filesystem>
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
};

struct ClientInfo {
  QLocalSocket *conn;
  struct {
    QByteArray data;
    uint32_t length;
  } frame;
};

struct CommandError {
  std::string error;
};

enum CommandResponseStatus {
  CommandOkStatus,
  CommandErrorStatus,
};

class ICommandHandler {
public:
  virtual proto::ext::daemon::Response *handleCommand(const proto::ext::daemon::Request &request) = 0;
};

class IpcCommandServer : public QObject {
  ICommandHandler *_handler;
  QLocalServer *_server;
  std::vector<ClientInfo> _clients;

  void processFrame(QLocalSocket *conn, QByteArrayView frame);
  void handleRead(QLocalSocket *conn);
  void handleDisconnection(QLocalSocket *conn);
  void handleConnection();

public:
  IpcCommandServer(QWidget *parent = nullptr) : QObject(parent), _server(new QLocalServer(this)) {}
  bool start(const std::filesystem::path &localPath);

  void setHandler(ICommandHandler *handler);
};

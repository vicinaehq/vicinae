#pragma once
#include "common.hpp"
#include "proto/daemon.pb.h"
#include "types.hpp"
#include <cstdint>
#include <filesystem>
#include <netinet/in.h>
#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qfuturewatcher.h>
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

using Watcher = QFutureWatcher<proto::ext::daemon::Response *>;

struct ClientInfo {
  QLocalSocket *conn;
  struct {
    QByteArray data;
    uint32_t length;
  } frame;
  std::vector<QObjectUniquePtr<Watcher>> m_pending;
  std::optional<proto::ext::daemon::ClientType> type;
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
  virtual PromiseLike<proto::ext::daemon::Response *>
  handleCommand(const proto::ext::daemon::Request &request) = 0;
  virtual ~ICommandHandler() = default;
};

class IpcCommandServer : public QObject {

public:
  IpcCommandServer(QWidget *parent = nullptr) : QObject(parent), _server(new QLocalServer(this)) {}
  bool start(const std::filesystem::path &localPath);

  void setHandler(ICommandHandler *handler);

private:
  void processFrame(QLocalSocket *conn, QByteArrayView frame);
  void handleRead(QLocalSocket *conn);
  void handleDisconnection(QLocalSocket *conn);
  void handleConnection();
  std::optional<QLocalSocket *> getConn();

  ICommandHandler *_handler;
  QLocalServer *_server;
  std::vector<ClientInfo> _clients;
};

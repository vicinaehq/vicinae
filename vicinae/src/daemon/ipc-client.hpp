#pragma once

/**
 * Establishes a connection to the running Vicinae daemon to perform commands.
 * Used to handle deeplinks, window visibility toggling, and much more...
 */
#include <QUrl>
#include "proto/daemon.pb.h"
#include <qlocalsocket.h>
#include <qobject.h>
#include <qstringview.h>
#include <QIODevice>

class DaemonIpcClient {
  QLocalSocket m_conn;

  void writeRequest(const proto::ext::daemon::Request &req);

public:
  void toggle();
  void ping();
  void passUrl(const QUrl &url);
  bool connect();

  DaemonIpcClient();
};

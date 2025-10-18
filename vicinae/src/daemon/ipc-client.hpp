#pragma once

/**
 * Establishes a connection to the running Vicinae daemon to perform commands.
 * Used to handle deeplinks, window visibility toggling, and much more...
 */
#include <QUrl>
#include "proto/daemon.pb.h"
#include "ui/dmenu-view/dmenu-view.hpp"
#include <qlocalsocket.h>
#include <qobject.h>
#include <qstringview.h>
#include <QIODevice>

class DaemonIpcClient {
public:
  DaemonIpcClient();

  /**
   * Returns the selected item text, if any.
   * Closing the launcher window or exiting from the dmenu view using
   * backspace will return std::nullopt.
   */
  std::string dmenu(DMenuListView::DmenuPayload payload);
  void toggle();
  tl::expected<void, QString> deeplink(const QUrl &url);
  bool connect();
  void connectOrThrow();
  bool ping();

private:
  void writeRequest(const proto::ext::daemon::Request &req);
  proto::ext::daemon::Response request(const proto::ext::daemon::Request &req);

  QLocalSocket m_conn;
};

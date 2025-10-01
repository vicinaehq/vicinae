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
public:
  struct DmenuPayload {
    std::string raw;
    std::string navigationTitle;
    std::string placeholder;
    std::string sectionTitle;
    bool noSection;
  };

  /**
   * Returns the selected item text, if any.
   * Closing the launcher window or exiting from the dmenu view using
   * backspace will return std::nullopt.
   */
  std::optional<std::string> dmenu(DmenuPayload payload);

  void toggle();
  void sendDeeplink(const QUrl &url);
  bool connect();

  DaemonIpcClient();

private:
  QLocalSocket m_conn;
  void writeRequest(const proto::ext::daemon::Request &req);
};

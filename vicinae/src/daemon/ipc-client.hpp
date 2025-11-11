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
  struct OpenSettings {
    std::optional<std::string> query;
  };
  struct ToggleSettings {
    std::optional<std::string> query;
  };

  DaemonIpcClient();

  /**
   * Returns the selected item text, if any.
   * Closing the launcher window or exiting from the dmenu view using
   * backspace will return std::nullopt.
   */
  std::string dmenu(const DMenu::Payload &payload);
  void toggle(const ToggleSettings &settings = {});
  bool open(const OpenSettings &settings = {});
  bool close();
  tl::expected<void, QString> deeplink(const QUrl &url);
  bool connect();
  void connectOrThrow();
  bool ping();
  void launchApp(const std::string &id, const std::vector<std::string> &args = {}, bool newInstance = false);
  std::vector<proto::ext::daemon::AppInfo> listApps(bool withActions = false);

private:
  void writeRequest(const proto::ext::daemon::Request &req);
  proto::ext::daemon::Response request(const proto::ext::daemon::Request &req);

  QLocalSocket m_conn;
};

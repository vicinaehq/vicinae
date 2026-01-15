#pragma once
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qstringview.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <qdebug.h>
#include <unistd.h>
#include <expected>

namespace Hyprland {
class Controller : public QObject {
public:
  static QByteArray oneshot(std::string_view command);
  QByteArray start(const std::string &command);
};
}; // namespace Hyprland

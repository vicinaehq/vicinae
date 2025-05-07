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

class Hyprctl : public QObject {
public:
  static QByteArray oneshot(std::string_view command) { return Hyprctl().start(std::string(command)); }

  Hyprctl() { QLocalSocket client; }
  ~Hyprctl() {}

  QByteArray start(const std::string &command) {
    char _buf[1 << 8];
    auto his = getenv("HYPRLAND_INSTANCE_SIGNATURE");

    if (!his) { qWarning() << "Hyprctl::execute() failed: HYPRLAND_INSTANCE_SIGNATURE is not set"; }

    std::filesystem::path rundir = "/tmp";

    if (auto p = getenv("XDG_RUNTIME_DIR")) rundir = p;

    std::filesystem::path sockPath = rundir / "hypr" / his / ".socket.sock";
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sock < 0) {
      qWarning() << "Hyprctl::execute() failed: socket() =>" << strerror(errno);
      return {};
    }

    struct sockaddr_un serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, sockPath.c_str(), sizeof(serverAddr.sun_path) - 1);

    if (::connect(sock, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
      qWarning() << "Hyprctl::execute() failed: connect() =>" << strerror(errno);
      close(sock);
      return {};
    }

    if (send(sock, command.data(), command.size(), 0) <= 0) {
      qWarning() << "Hyprctl::execute() failed: send() =>" << strerror(errno);
      close(sock);
      return {};
    }

    int rc = 0;
    QByteArray data;

    while ((rc = recv(sock, _buf, sizeof(_buf), 0)) > 0) {
      data += QByteArrayView{_buf, rc};
    }

    close(sock);

    if (rc == -1) return {};

    return data;
  }
};

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
#include <glaze/glaze.hpp>
#include <optional>

namespace Hyprland {
class Controller : public QObject {
public:
  static QByteArray oneshot(std::string_view command);

  template <typename T> static std::optional<T> oneshot(std::string_view command) {
    constexpr glz::opts opts{.error_on_unknown_keys = false};
    const QByteArray response = oneshot(command);
    auto view = std::string_view(response.constData(), static_cast<std::size_t>(response.size()));
    T value{};
    if (glz::read<opts>(value, view)) {
      qWarning() << "Hyprctl: failed to parse reply for"
                 << QString::fromUtf8(command.data(), static_cast<qsizetype>(command.size())) << ":"
                 << response;
      return std::nullopt;
    }
    return value;
  }

  QByteArray start(const std::string &command);
};
}; // namespace Hyprland

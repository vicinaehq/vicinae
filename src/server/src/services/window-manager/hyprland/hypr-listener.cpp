#include <unistd.h>
#include <algorithm>
#include <array>
#include <string_view>
#include "hypr-listener.hpp"
#include "utils.hpp"
#include <qlogging.h>
#include <filesystem>
#include <qsocketnotifier.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <QDebug>
#include <ranges>

using namespace Hyprland;

namespace fs = std::filesystem;

EventListener::EventListener() {
  connect(m_notifier, &QSocketNotifier::activated, this, &EventListener::handleRead);
}

bool EventListener::start() {
  auto his = getenv("HYPRLAND_INSTANCE_SIGNATURE");

  if (!his) { qWarning() << "Hyprctl::execute() failed: HYPRLAND_INSTANCE_SIGNATURE is not set"; }

  fs::path rundir = "/tmp";

  if (auto p = getenv("XDG_RUNTIME_DIR")) rundir = p;

  fs::path const sockPath = rundir / "hypr" / his / ".socket2.sock";
  int const sock = socket(AF_UNIX, SOCK_STREAM, 0);

  if (sock < 0) {
    qWarning() << "Hyprctl::execute() failed: socket() =>" << strerror(errno);
    return {};
  }

  struct sockaddr_un serverAddr;

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sun_family = AF_UNIX;
  strncpy(serverAddr.sun_path, sockPath.c_str(), sizeof(serverAddr.sun_path) - 1);

  if (::connect(sock, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
    qWarning() << "Hyprland::EventListener failed: connect() =>" << strerror(errno);
    close(sock);
    return false;
  }

  m_notifier->setSocket(sock);
  m_notifier->setEnabled(true);

  return true;
}

namespace {

constexpr auto WORKSPACE_EVENTS = std::to_array<std::string_view>(
    {"workspace", "workspacev2", "focusedmon", "focusedmonv2", "createworkspace", "createworkspacev2",
     "destroyworkspace", "destroyworkspacev2", "renameworkspace", "moveworkspace", "moveworkspacev2",
     "movewindow", "movewindowv2", "fullscreen", "urgent"});

} // namespace

void EventListener::processEvent(const std::string &event) {
  auto ss = ranges_to<std::vector>(std::views::split(event, std::string_view(">>")));

  if (ss.size() != 2) {
    qWarning() << "Hyprland event socket sent a malformed invalid event" << event.c_str();
    return;
  }

  auto name = std::string_view(ss[0].begin(), ss[0].end());
  [[maybe_unused]] auto value = std::string_view(ss[1].begin(), ss[1].end());

  if (name == "openwindow") {
    emit openwindow("", "", "", "");
  } else if (name == "closewindow") {
    emit closewindow("");
  } else if (name == "activewindow" || name == "activewindowv2") {
    emit activewindowchanged();
  } else if (name == "windowtitle" || name == "windowtitlev2") {
    emit windowtitlechanged();
  } else if (std::ranges::contains(WORKSPACE_EVENTS, name)) {
    emit workspacechanged();
  }
}

void EventListener::handleRead() {
  int const fd = m_notifier->socket();
  int rc = 0;

  while ((rc = recv(fd, &m_buf, m_buf.size(), MSG_NOSIGNAL | MSG_DONTWAIT)) > 0) {
    m_message += {m_buf.data(), static_cast<size_t>(rc)};
    auto pos = std::string::npos;

    while ((pos = m_message.find('\n')) != std::string::npos) {
      processEvent(m_message.substr(0, pos));
      m_message = m_message.substr(pos + 1);
    }
  }
}

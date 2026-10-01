#include "niri.hpp"
#include "utils/environment.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <glaze/glaze.hpp>
#include <map>
#include <qlogging.h>
#include <qstring.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <variant>

namespace Niri::ipc {

struct Window {
  WindowHandle id = 0;
  std::optional<std::string> title;
  std::optional<std::string> appId;
  std::optional<int> pid;
  std::optional<WorkspaceHandle> workspaceId;
  bool isFocused = false;
  std::optional<FocusTimestamp> focusTimestamp;
};

struct Workspace {
  WorkspaceHandle id = 0;
  std::optional<std::string> name;
  std::optional<std::string> output;
  bool isActive = false;
  bool isFocused = false;
};

struct WindowsChanged {
  std::vector<Window> windows;
};

struct WindowOpenedOrChanged {
  Window window;
};

struct WindowClosed {
  WindowHandle id = 0;
};

struct WindowFocusChanged {
  std::optional<WindowHandle> id;
};

struct WindowFocusTimestampChanged {
  WindowHandle id = 0;
  std::optional<FocusTimestamp> focusTimestamp;
};

struct WorkspacesChanged {
  std::vector<Workspace> workspaces;
};

struct WorkspaceActivated {
  WorkspaceHandle id = 0;
  bool focused = false;
};

// niri uses externally tagged enums: each event is an object with a single
// PascalCase key. We parse that envelope as a raw-json map and only then
// parse the payloads we model, so unknown event kinds are plain no-ops.
using EventEnvelope = std::map<std::string, glz::raw_json>;

struct OkReply {
  glz::generic ok;
};

struct ErrorReply {
  std::string err;
};

using Reply = std::variant<OkReply, ErrorReply>;

struct WindowIdArg {
  WindowHandle id = 0;
};

struct FocusWindow {
  WindowIdArg focusWindow;
};

struct CloseWindow {
  WindowIdArg closeWindow;
};

struct FullscreenWindow {
  WindowIdArg fullscreenWindow;
};

struct ToggleWindowFloating {
  WindowIdArg toggleWindowFloating;
};

struct ToggleOverview {
  std::map<std::string_view, std::uint64_t> toggleOverview;
};

using Action = std::variant<FocusWindow, CloseWindow, FullscreenWindow, ToggleWindowFloating, ToggleOverview>;

struct ActionRequest {
  Action action;
};

} // namespace Niri::ipc

template <> struct glz::meta<Niri::ipc::Window> : glz::snake_case {};
template <> struct glz::meta<Niri::ipc::Workspace> : glz::snake_case {};
template <> struct glz::meta<Niri::ipc::WindowFocusTimestampChanged> : glz::snake_case {};

template <> struct glz::meta<Niri::ipc::OkReply> : glz::pascal_case {};
template <> struct glz::meta<Niri::ipc::ErrorReply> : glz::pascal_case {};

template <> struct glz::meta<Niri::ipc::FocusWindow> : glz::pascal_case {};
template <> struct glz::meta<Niri::ipc::CloseWindow> : glz::pascal_case {};
template <> struct glz::meta<Niri::ipc::FullscreenWindow> : glz::pascal_case {};
template <> struct glz::meta<Niri::ipc::ToggleWindowFloating> : glz::pascal_case {};
template <> struct glz::meta<Niri::ipc::ToggleOverview> : glz::pascal_case {};
template <> struct glz::meta<Niri::ipc::ActionRequest> : glz::pascal_case {};

namespace Niri {

namespace {

using namespace std::chrono_literals;

constexpr glz::opts PARSE_OPTS{.error_on_unknown_keys = false};
constexpr auto WINDOWS_CHANGED_THROTTLE_INTERVAL = 500ms;

template <class T> std::optional<T> parsePayload(const glz::raw_json &raw) {
  T value{};
  if (glz::read<PARSE_OPTS>(value, raw.str)) {
    qWarning() << "Niri::WindowManager: failed to parse event payload:" << QString::fromStdString(raw.str);
    return std::nullopt;
  }
  return value;
}

QString toQString(const std::optional<std::string> &value) {
  return value ? QString::fromStdString(*value) : QString();
}

std::string niriSocketPath() {
  const char *path = std::getenv("NIRI_SOCKET");
  return path ? path : std::string();
}

std::optional<int> connectSocket(const std::string &path) {
  if (path.empty()) {
    qWarning() << "Niri::WindowManager: NIRI_SOCKET is not set";
    return std::nullopt;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    qWarning() << "Niri::WindowManager: socket() failed:" << std::strerror(errno);
    return std::nullopt;
  }

  struct sockaddr_un addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  if (path.size() >= sizeof(addr.sun_path)) {
    qWarning() << "Niri::WindowManager: NIRI_SOCKET path is too long";
    close(fd);
    return std::nullopt;
  }

  std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

  if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    qWarning() << "Niri::WindowManager: connect() failed:" << std::strerror(errno);
    close(fd);
    return std::nullopt;
  }

  return fd;
}

bool sendAll(int fd, std::string_view payload) {
  std::size_t sent = 0;
  while (sent < payload.size()) {
    ssize_t const rc = send(fd, payload.data() + sent, payload.size() - sent, MSG_NOSIGNAL);
    if (rc <= 0) {
      qWarning() << "Niri::WindowManager: send() failed:" << std::strerror(errno);
      return false;
    }
    sent += static_cast<std::size_t>(rc);
  }

  return true;
}

std::optional<std::string> recvLineBlocking(int fd, std::string &buffer) {
  while (true) {
    if (auto idx = buffer.find('\n'); idx != std::string::npos) {
      std::string line = buffer.substr(0, idx);
      buffer.erase(0, idx + 1);
      return line;
    }

    std::array<char, 1 << 12> tmp{};
    ssize_t const rc = recv(fd, tmp.data(), tmp.size(), 0);
    if (rc <= 0) {
      if (rc < 0) { qWarning() << "Niri::WindowManager: recv() failed:" << std::strerror(errno); }
      return std::nullopt;
    }

    buffer.append(tmp.data(), static_cast<std::size_t>(rc));
  }
}

bool isHandled(const glz::generic &value) {
  if (value.is_string()) { return value.get_string() == "Handled"; }
  return value.contains("Handled");
}

std::optional<glz::generic> parseReply(const std::string &line) {
  ipc::Reply reply;
  if (glz::read<PARSE_OPTS>(reply, line)) {
    qWarning() << "Niri::WindowManager: malformed reply:" << QString::fromStdString(line);
    return std::nullopt;
  }

  if (auto *error = std::get_if<ipc::ErrorReply>(&reply)) {
    qWarning() << "Niri::WindowManager: request failed:" << QString::fromStdString(error->err);
    return std::nullopt;
  }

  return std::move(std::get<ipc::OkReply>(reply).ok);
}

std::optional<glz::generic> sendRequest(std::string_view rawRequest) {
  auto fd = connectSocket(niriSocketPath());
  if (!fd.has_value()) { return std::nullopt; }

  std::string payload{rawRequest};
  payload += '\n';

  if (!sendAll(*fd, payload)) {
    close(*fd);
    return std::nullopt;
  }

  std::string buffer;
  auto line = recvLineBlocking(*fd, buffer);
  close(*fd);
  if (!line.has_value()) { return std::nullopt; }

  return parseReply(*line);
}

bool sendAction(const ipc::Action &action) {
  auto json = glz::write_json(ipc::ActionRequest{action});
  if (!json.has_value()) { return false; }

  auto reply = sendRequest(*json);
  return reply.has_value() && isHandled(*reply);
}

} // namespace

void Window::apply(const ipc::Window &window) {
  m_id = window.id;
  m_title = toQString(window.title);
  m_wmClass = toQString(window.appId);
  m_pid = window.pid;
  // wm-router currently assumes workspace() always has a value.
  m_workspaceId = window.workspaceId ? QString::number(*window.workspaceId) : QString();
  m_focused = window.isFocused;
  m_focusTimestamp = window.focusTimestamp;
}

void Workspace::apply(const ipc::Workspace &workspace) {
  m_id = workspace.id;
  m_name = toQString(workspace.name);
  m_monitor = toQString(workspace.output);
  m_active = workspace.isActive;
  m_focused = workspace.isFocused;
}

WindowManager::WindowManager() {
  m_eventNotifier.setEnabled(false);
  connect(&m_eventNotifier, &QSocketNotifier::activated, this,
          [this](QSocketDescriptor, QSocketNotifier::Type) { handleEventSocketReadable(); });

  m_windowsChangedThrottle.setSingleShot(true);
  m_windowsChangedThrottle.setInterval(WINDOWS_CHANGED_THROTTLE_INTERVAL);
  connect(&m_windowsChangedThrottle, &QTimer::timeout, this, [this]() {
    if (!m_pendingWindowsChanged) { return; }
    m_pendingWindowsChanged = false;
    m_windowsChangedThrottle.start();
    emit windowsChanged();
  });
}

WindowManager::~WindowManager() {
  if (m_eventFd >= 0) {
    close(m_eventFd);
    m_eventFd = -1;
  }
}

AbstractWindowManager::WindowList WindowManager::listWindowsSync() const { return m_windows; }

AbstractWindowManager::WorkspaceList WindowManager::listWorkspaces() const { return m_workspaces; }

AbstractWindowManager::WindowPtr WindowManager::getFocusedWindowSync() const {
  for (const auto &window : m_windows) {
    if (std::static_pointer_cast<Window>(window)->isFocused()) { return window; }
  }

  return nullptr;
}

AbstractWindowManager::WorkspacePtr WindowManager::getActiveWorkspace() const {
  for (const auto &workspace : m_workspaces) {
    if (std::static_pointer_cast<Workspace>(workspace)->isFocused()) { return workspace; }
  }

  for (const auto &workspace : m_workspaces) {
    if (std::static_pointer_cast<Workspace>(workspace)->isActive()) { return workspace; }
  }

  return nullptr;
}

void WindowManager::focusWindowSync(const AbstractWindow &window) const {
  auto id = parseWindowHandle(window.id());
  if (!id.has_value()) { return; }

  if (!sendAction(ipc::FocusWindow{{*id}})) {
    qWarning() << "Niri::WindowManager: failed to focus window" << window.id();
    return;
  }

  emit windowsChanged();
}

bool WindowManager::closeWindow(const AbstractWindow &window) const {
  auto id = parseWindowHandle(window.id());
  if (!id.has_value()) { return false; }

  if (!sendAction(ipc::CloseWindow{{*id}})) { return false; }

  emit windowsChanged();
  return true;
}

bool WindowManager::ping() const { return sendRequest("\"Version\"").has_value(); }

bool WindowManager::isActivatable() const {
  if (!Environment::isWaylandSession()) { return false; }
  auto socket = niriSocketPath();
  if (socket.empty()) { return false; }
  return std::filesystem::exists(socket);
}

void WindowManager::start() {
  if (!connectEventStream()) {
    qWarning() << "Niri::WindowManager: failed to connect event stream";
    return;
  }
}

bool WindowManager::connectEventStream() {
  if (m_eventFd >= 0) return true;

  auto fd = connectSocket(niriSocketPath());
  if (!fd.has_value()) { return false; }

  if (!sendAll(*fd, "\"EventStream\"\n")) {
    close(*fd);
    return false;
  }

  auto line = recvLineBlocking(*fd, m_eventBuffer);
  if (!line.has_value()) {
    close(*fd);
    return false;
  }

  auto reply = parseReply(*line);
  if (!reply.has_value() || !isHandled(*reply)) {
    qWarning() << "Niri::WindowManager: EventStream subscription failed";
    close(*fd);
    return false;
  }

  m_eventFd = *fd;
  m_eventNotifier.setSocket(m_eventFd);
  m_eventNotifier.setEnabled(true);

  // Process any event lines that may have already arrived with the subscription reply.
  drainEventBuffer();

  return true;
}

void WindowManager::handleEventSocketReadable() {
  if (m_eventFd < 0) { return; }

  bool closed = false;
  std::array<char, 1 << 14> tmp{};
  while (true) {
    ssize_t const rc = recv(m_eventFd, tmp.data(), tmp.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
    if (rc > 0) {
      m_eventBuffer.append(tmp.data(), static_cast<std::size_t>(rc));
      continue;
    }
    if (rc == 0) {
      closed = true;
    } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
      qWarning() << "Niri::WindowManager: event socket recv failed:" << std::strerror(errno);
      closed = true;
    }
    break;
  }

  drainEventBuffer();

  if (closed) {
    qWarning() << "Niri::WindowManager: event stream closed";
    m_eventNotifier.setEnabled(false);
    close(m_eventFd);
    m_eventFd = -1;
  }
}

void WindowManager::drainEventBuffer() {
  std::size_t idx = 0;
  while ((idx = m_eventBuffer.find('\n')) != std::string::npos) {
    std::string const line = m_eventBuffer.substr(0, idx);
    m_eventBuffer.erase(0, idx + 1);
    processEventLine(line);
  }
}

void WindowManager::processEventLine(const std::string &line) {
  ipc::EventEnvelope envelope;
  if (glz::read<PARSE_OPTS>(envelope, line)) { return; }

  bool changed = false;

  for (const auto &[kind, payload] : envelope) {
    if (kind == "WindowsChanged") {
      if (auto ev = parsePayload<ipc::WindowsChanged>(payload)) {
        updateWindows(ev->windows);
        changed = true;
      }
    } else if (kind == "WindowOpenedOrChanged") {
      if (auto ev = parsePayload<ipc::WindowOpenedOrChanged>(payload)) {
        upsertWindow(ev->window);
        changed = true;
      }
    } else if (kind == "WindowClosed") {
      if (auto ev = parsePayload<ipc::WindowClosed>(payload)) {
        removeWindow(ev->id);
        changed = true;
      }
    } else if (kind == "WindowFocusChanged") {
      if (auto ev = parsePayload<ipc::WindowFocusChanged>(payload)) {
        setFocusedWindow(ev->id);
        changed = true;
      }
    } else if (kind == "WindowFocusTimestampChanged") {
      if (auto ev = parsePayload<ipc::WindowFocusTimestampChanged>(payload)) {
        setWindowFocusTimestamp(ev->id, ev->focusTimestamp);
        changed = true;
      }
    } else if (kind == "WorkspacesChanged") {
      if (auto ev = parsePayload<ipc::WorkspacesChanged>(payload)) {
        updateWorkspaces(ev->workspaces);
        changed = true;
      }
    } else if (kind == "WorkspaceActivated") {
      if (auto ev = parsePayload<ipc::WorkspaceActivated>(payload)) {
        setActiveWorkspace(ev->id, ev->focused);
        changed = true;
      }
    }
  }

  if (changed) { scheduleWindowsChanged(); }
}

void WindowManager::scheduleWindowsChanged() {
  if (m_windowsChangedThrottle.isActive()) {
    m_pendingWindowsChanged = true;
    return;
  }

  m_windowsChangedThrottle.start();
  emit windowsChanged();
}

void WindowManager::updateWindows(const std::vector<ipc::Window> &windows) {
  WindowList newList;
  newList.reserve(windows.size());

  for (const auto &windowData : windows) {
    auto window = std::make_shared<Window>();
    window->apply(windowData);
    newList.emplace_back(std::move(window));
  }

  m_windows = std::move(newList);
  sortWindowsByFocusTimestamp();
}

void WindowManager::upsertWindow(const ipc::Window &windowData) {
  auto pred = [&](const auto &window) {
    return std::static_pointer_cast<Window>(window)->handle() == windowData.id;
  };
  auto it = std::ranges::find_if(m_windows, pred);

  std::shared_ptr<Window> window;
  if (it != m_windows.end()) {
    window = std::static_pointer_cast<Window>(*it);
    window->apply(windowData);
  } else {
    window = std::make_shared<Window>();
    window->apply(windowData);
    m_windows.emplace_back(window);
  }

  if (window->isFocused()) { setFocusedWindow(window->handle()); }
  sortWindowsByFocusTimestamp();
}

void WindowManager::removeWindow(WindowHandle handle) {
  auto pred = [&](const auto &window) {
    return std::static_pointer_cast<Window>(window)->handle() == handle;
  };
  std::erase_if(m_windows, pred);
  sortWindowsByFocusTimestamp();
}

bool WindowManager::toggleFullscreen(const AbstractWindow &window) {
  auto id = parseWindowHandle(window.id());
  if (!id.has_value()) { return false; }

  if (!sendAction(ipc::FullscreenWindow{{*id}})) {
    qWarning() << "Niri::WindowManager: failed to fullscreen window" << window.id();
    return false;
  }

  emit windowsChanged();
  return true;
}

bool WindowManager::toggleFloating(const AbstractWindow &window) {
  auto id = parseWindowHandle(window.id());
  if (!id.has_value()) { return false; }

  if (!sendAction(ipc::ToggleWindowFloating{{*id}})) {
    qWarning() << "Niri::WindowManager: failed to toggle floating for window" << window.id();
    return false;
  }

  emit windowsChanged();
  return true;
}

bool WindowManager::toggleOverview() {
  if (!sendAction(ipc::ToggleOverview{})) {
    qWarning() << "Niri::WindowManager: failed to toggle overview";
    return false;
  }

  emit windowsChanged();
  return true;
}

void WindowManager::setFocusedWindow(std::optional<WindowHandle> handle) {
  // A null handle means focus moved to nothing; keep the last focused window
  // marked so focus handoff detection keeps working.
  if (!handle.has_value()) return;

  bool changed = false;
  for (const auto &window : m_windows) {
    auto niriWindow = std::static_pointer_cast<Window>(window);
    bool const focused = niriWindow->handle() == *handle;
    if (niriWindow->isFocused() != focused) {
      niriWindow->setFocused(focused);
      changed = true;
    }
  }

  if (changed) { emit focusChanged(); }
}

void WindowManager::setWindowFocusTimestamp(WindowHandle handle,
                                            std::optional<ipc::FocusTimestamp> focusTimestamp) {
  for (const auto &window : m_windows) {
    auto niriWindow = std::static_pointer_cast<Window>(window);
    if (niriWindow->handle() == handle) {
      niriWindow->setFocusTimestamp(focusTimestamp);
      break;
    }
  }

  sortWindowsByFocusTimestamp();
}

void WindowManager::sortWindowsByFocusTimestamp() {
  std::ranges::stable_sort(m_windows, [](const auto &lhs, const auto &rhs) {
    auto lw = std::static_pointer_cast<Window>(lhs);
    auto rw = std::static_pointer_cast<Window>(rhs);
    auto lts = lw->focusTimestamp();
    auto rts = rw->focusTimestamp();

    if (lts.has_value() != rts.has_value()) { return lts.has_value(); }
    if (lts.has_value() && rts.has_value()) {
      if (lts->secs != rts->secs) { return lts->secs > rts->secs; }
      if (lts->nanos != rts->nanos) { return lts->nanos > rts->nanos; }
    }
    return lw->handle() > rw->handle();
  });
}

void WindowManager::updateWorkspaces(const std::vector<ipc::Workspace> &workspaces) {
  WorkspaceList newList;
  newList.reserve(workspaces.size());

  for (const auto &workspaceData : workspaces) {
    auto workspace = std::make_shared<Workspace>();
    workspace->apply(workspaceData);
    newList.emplace_back(std::move(workspace));
  }

  m_workspaces = std::move(newList);
}

void WindowManager::setActiveWorkspace(WorkspaceHandle handle, bool focused) {
  QString targetMonitor;
  for (const auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (niriWorkspace->handle() == handle) {
      targetMonitor = niriWorkspace->monitor().value_or(QString());
      break;
    }
  }

  for (const auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (!targetMonitor.isEmpty() && niriWorkspace->monitor() == targetMonitor) {
      niriWorkspace->setActive(false);
    }
    if (focused) { niriWorkspace->setFocused(false); }
  }

  for (const auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (niriWorkspace->handle() == handle) {
      niriWorkspace->setActive(true);
      if (focused) { niriWorkspace->setFocused(true); }
      break;
    }
  }
}

std::optional<WindowHandle> WindowManager::parseWindowHandle(const QString &id) {
  bool ok = false;
  auto value = id.toULongLong(&ok);
  if (!ok) {
    qWarning() << "Niri::WindowManager: invalid window id" << id;
    return std::nullopt;
  }
  return value;
}

} // namespace Niri

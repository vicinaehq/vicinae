#include "mango.hpp"
#include "utils/environment.hpp"
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <glaze/glaze.hpp>
#include <optional>
#include <qlogging.h>
#include <qstring.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace Mango::ipc {

struct Client {
  std::uint32_t id = 0;
  int pid = 0;
  std::string title;
  std::string appid;
  std::string monitor;
  std::vector<int> tags;
  bool isFullscreen = false;
  bool isGlobal = false;
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
};

struct AllClients {
  std::vector<Client> clients;
};

struct Tag {
  int index = 0;
  std::string layout;
};

struct AllTagsEntry {
  std::string monitor;
  std::vector<Tag> tags;
};

struct AllTags {
  std::vector<AllTagsEntry> allTags;
};

struct Monitor {
  std::string name;
  bool active = false;
  std::vector<int> activeTags;
};

struct AllMonitors {
  std::vector<Monitor> monitors;
};

} // namespace Mango::ipc

template <> struct glz::meta<Mango::ipc::Client> : glz::snake_case {};
template <> struct glz::meta<Mango::ipc::AllClients> : glz::snake_case {};
template <> struct glz::meta<Mango::ipc::Tag> : glz::snake_case {};
template <> struct glz::meta<Mango::ipc::AllTagsEntry> : glz::snake_case {};
template <> struct glz::meta<Mango::ipc::AllTags> : glz::snake_case {};
template <> struct glz::meta<Mango::ipc::Monitor> : glz::snake_case {};
template <> struct glz::meta<Mango::ipc::AllMonitors> : glz::snake_case {};

namespace Mango {

namespace {

constexpr glz::opts PARSE_OPTS{.error_on_unknown_keys = false};

std::string socketPath() {
  const char *path = std::getenv("MANGO_INSTANCE_SIGNATURE");
  return path ? path : std::string();
}

std::optional<int> connectSocket() {
  const auto path = socketPath();
  if (path.empty()) {
    qWarning() << "Mango::WindowManager: MANGO_INSTANCE_SIGNATURE is not set";
    return std::nullopt;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    qWarning() << "Mango::WindowManager: socket() failed:" << std::strerror(errno);
    return std::nullopt;
  }

  struct sockaddr_un addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  if (path.size() >= sizeof(addr.sun_path)) {
    qWarning() << "Mango::WindowManager: socket path is too long";
    close(fd);
    return std::nullopt;
  }

  std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

  if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    qWarning() << "Mango::WindowManager: connect() failed:" << std::strerror(errno);
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
      qWarning() << "Mango::WindowManager: send() failed:" << std::strerror(errno);
      return false;
    }
    sent += static_cast<std::size_t>(rc);
  }

  return true;
}

std::optional<std::string> oneshot(std::string_view command) {
  auto fd = connectSocket();
  if (!fd.has_value()) return std::nullopt;

  std::string payload(command);
  payload += '\n';

  if (!sendAll(*fd, payload)) {
    close(*fd);
    return std::nullopt;
  }

  std::string response;
  std::array<char, 1 << 12> buf{};
  while (true) {
    ssize_t const rc = recv(*fd, buf.data(), buf.size(), 0);
    if (rc > 0) {
      response.append(buf.data(), static_cast<std::size_t>(rc));
      continue;
    }
    if (rc == 0) break;
    if (errno == EINTR) continue;
    qWarning() << "Mango::WindowManager: recv() failed:" << std::strerror(errno);
    close(*fd);
    return std::nullopt;
  }

  close(*fd);
  return response;
}

template <typename T> std::optional<T> oneshot(std::string_view command) {
  auto response = oneshot(command);
  if (!response.has_value()) return std::nullopt;

  T value{};
  if (glz::read<PARSE_OPTS>(value, *response)) {
    qWarning() << "Mango::WindowManager: failed to parse reply for"
               << QString::fromUtf8(command.data(), static_cast<qsizetype>(command.size())) << ":"
               << QString::fromStdString(*response);
    return std::nullopt;
  }

  return value;
}

bool dispatchSuccess(std::string_view args) {
  std::string command = "dispatch ";
  command += args;
  auto response = oneshot(command);
  if (!response.has_value()) return false;
  return response->find("\"success\":true") != std::string::npos;
}

std::optional<int> connectWatch(std::string_view command) {
  auto fd = connectSocket();
  if (!fd.has_value()) return std::nullopt;

  std::string payload(command);
  payload += '\n';

  if (!sendAll(*fd, payload)) {
    close(*fd);
    return std::nullopt;
  }

  return fd;
}

std::string windowTarget(const AbstractWindowManager::AbstractWindow &window) {
  return window.id().toStdString();
}

} // namespace

void Window::apply(const ipc::Client &client) {
  m_id = client.id;
  m_title = QString::fromStdString(client.title);
  m_wmClass = QString::fromStdString(client.appid);
  m_pid = client.pid > 0 ? std::optional<int>(client.pid) : std::nullopt;
  m_workspace = (!client.monitor.empty() && !client.tags.empty())
                    ? std::optional<QString>(QString::fromStdString(client.monitor) + ":" +
                                             QString::number(client.tags.front()))
                    : std::nullopt;
  m_bounds = {.x = client.x, .y = client.y, .width = client.width, .height = client.height};
  m_fullScreen = client.isFullscreen;
  m_sticky = client.isGlobal;
}

WindowManager::WindowManager() {
  m_clientsNotifier.setEnabled(false);
  m_focusNotifier.setEnabled(false);

  connect(&m_clientsNotifier, &QSocketNotifier::activated, this,
          [this](QSocketDescriptor, QSocketNotifier::Type) { handleClientsReadable(); });
  connect(&m_focusNotifier, &QSocketNotifier::activated, this,
          [this](QSocketDescriptor, QSocketNotifier::Type) { handleFocusReadable(); });
}

WindowManager::~WindowManager() {
  if (m_clientsFd >= 0) close(m_clientsFd);
  if (m_focusFd >= 0) close(m_focusFd);
}

AbstractWindowManager::WindowList WindowManager::listWindowsSync() const {
  auto parsed = oneshot<ipc::AllClients>("get all-clients");
  if (!parsed.has_value()) return {};

  WindowList windows;
  windows.reserve(parsed->clients.size());

  for (const auto &client : parsed->clients) {
    auto window = std::make_shared<Window>();
    window->apply(client);
    windows.emplace_back(std::move(window));
  }

  return windows;
}

AbstractWindowManager::WindowPtr WindowManager::getFocusedWindowSync() const {
  auto client = oneshot<ipc::Client>("get focusing-client");
  if (!client.has_value() || client->id == 0) return nullptr;

  auto window = std::make_shared<Window>();
  window->apply(*client);
  return window;
}

void WindowManager::focusWindowSync(const AbstractWindow &window) const {
  if (!dispatchSuccess("focusid client," + windowTarget(window))) {
    qWarning() << "Mango::WindowManager: failed to focus window" << window.id();
    return;
  }

  emit windowsChanged();
}

bool WindowManager::closeWindow(const AbstractWindow &window) const {
  if (!dispatchSuccess("killclient client," + windowTarget(window))) return false;

  emit windowsChanged();
  return true;
}

bool WindowManager::toggleFullscreen(const AbstractWindow &window) {
  if (!dispatchSuccess("togglefullscreen client," + windowTarget(window))) return false;

  emit windowsChanged();
  return true;
}

bool WindowManager::toggleFloating(const AbstractWindow &window) {
  if (!dispatchSuccess("togglefloating client," + windowTarget(window))) return false;

  emit windowsChanged();
  return true;
}

bool WindowManager::setSticky(const AbstractWindow &window, bool sticky) const {
  if (window.sticky() == sticky) return true;

  if (!dispatchSuccess("toggleglobal client," + windowTarget(window))) return false;

  emit windowsChanged();
  return true;
}

bool WindowManager::toggleOverview() {
  if (!dispatchSuccess("toggleoverview")) return false;

  emit windowsChanged();
  return true;
}

AbstractWindowManager::WorkspaceList WindowManager::listWorkspaces() const {
  auto parsed = oneshot<ipc::AllTags>("get all-tags");
  if (!parsed.has_value()) return {};

  WorkspaceList workspaces;
  for (const auto &entry : parsed->allTags) {
    for (const auto &tag : entry.tags) {
      auto id = QString::fromStdString(entry.monitor) + ":" + QString::number(tag.index);
      workspaces.emplace_back(
          std::make_shared<Workspace>(id, QString::number(tag.index), QString::fromStdString(entry.monitor)));
    }
  }

  return workspaces;
}

AbstractWindowManager::WorkspacePtr WindowManager::getActiveWorkspace() const {
  auto parsed = oneshot<ipc::AllMonitors>("get all-monitors");
  if (!parsed.has_value()) return nullptr;

  for (const auto &monitor : parsed->monitors) {
    if (!monitor.active || monitor.activeTags.empty()) continue;
    auto id = QString::fromStdString(monitor.name) + ":" + QString::number(monitor.activeTags.front());
    return std::make_shared<Workspace>(id, QString::number(monitor.activeTags.front()),
                                       QString::fromStdString(monitor.name));
  }

  return nullptr;
}

namespace {

std::optional<std::pair<std::string, std::string>> splitWorkspace(const QString &workspaceId) {
  auto idx = workspaceId.lastIndexOf(':');
  if (idx <= 0) return std::nullopt;
  auto monitor = workspaceId.left(idx);
  auto tag = workspaceId.mid(idx + 1);
  if (monitor.isEmpty() || tag.isEmpty()) return std::nullopt;
  return std::make_pair(monitor.toStdString(), tag.toStdString());
}

} // namespace

bool WindowManager::moveToWorkspace(const AbstractWindow &window, const QString &workspaceId) const {
  auto parts = splitWorkspace(workspaceId);
  if (!parts.has_value()) return false;

  // `tagcrossmon` moves the window to the target tag, crossing monitors when needed.
  if (!dispatchSuccess("tagcrossmon," + parts->second + "," + parts->first + " client," +
                       windowTarget(window))) {
    return false;
  }

  emit windowsChanged();
  return true;
}

void WindowManager::focusWorkspaceSync(const AbstractWorkspace &workspace) const {
  activateWorkspace(workspace.id());
}

bool WindowManager::activateWorkspace(const QString &workspaceId) const {
  auto parts = splitWorkspace(workspaceId);
  if (!parts.has_value()) return false;

  if (!dispatchSuccess("viewcrossmon," + parts->second + "," + parts->first)) { return false; }

  emit windowsChanged();
  return true;
}

bool WindowManager::ping() const { return oneshot("get version").has_value(); }

bool WindowManager::isActivatable() const {
  if (!Environment::isWaylandSession()) return false;
  auto path = socketPath();
  if (path.empty()) return false;
  return std::filesystem::exists(path);
}

void WindowManager::start() {
  if (m_clientsFd < 0) {
    if (auto fd = connectWatch("watch all-clients"); fd.has_value()) {
      m_clientsFd = *fd;
      m_clientsNotifier.setSocket(m_clientsFd);
      m_clientsNotifier.setEnabled(true);
    }
  }

  if (m_focusFd < 0) {
    if (auto fd = connectWatch("watch focusing-client"); fd.has_value()) {
      m_focusFd = *fd;
      m_focusNotifier.setSocket(m_focusFd);
      m_focusNotifier.setEnabled(true);
    }
  }
}

void WindowManager::handleClientsReadable() {
  if (m_clientsFd < 0) return;

  bool gotData = false;
  std::array<char, 1 << 12> buf{};
  while (true) {
    ssize_t const rc = recv(m_clientsFd, buf.data(), buf.size(), MSG_DONTWAIT);
    if (rc > 0) {
      gotData = true;
      continue;
    }
    if (rc == 0) {
      qWarning() << "Mango::WindowManager: clients watch stream closed";
      m_clientsNotifier.setEnabled(false);
      close(m_clientsFd);
      m_clientsFd = -1;
      return;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
    qWarning() << "Mango::WindowManager: clients watch recv failed:" << std::strerror(errno);
    m_clientsNotifier.setEnabled(false);
    close(m_clientsFd);
    m_clientsFd = -1;
    return;
  }

  if (gotData) emit windowsChanged();
}

void WindowManager::handleFocusReadable() {
  if (m_focusFd < 0) return;

  bool gotData = false;
  std::array<char, 1 << 12> buf{};
  while (true) {
    ssize_t const rc = recv(m_focusFd, buf.data(), buf.size(), MSG_DONTWAIT);
    if (rc > 0) {
      gotData = true;
      continue;
    }
    if (rc == 0) {
      qWarning() << "Mango::WindowManager: focus watch stream closed";
      m_focusNotifier.setEnabled(false);
      close(m_focusFd);
      m_focusFd = -1;
      return;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
    qWarning() << "Mango::WindowManager: focus watch recv failed:" << std::strerror(errno);
    m_focusNotifier.setEnabled(false);
    close(m_focusFd);
    m_focusFd = -1;
    return;
  }

  if (gotData) emit focusChanged();
}

} // namespace Mango

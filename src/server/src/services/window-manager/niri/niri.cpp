#include "niri.hpp"
#include "utils/environment.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace Niri {

namespace {

std::optional<int> connectSocket(const QString &path) {
  if (path.isEmpty()) {
    qWarning() << "Niri::WindowManager: NIRI_SOCKET is not set";
    return std::nullopt;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    qWarning() << "Niri::WindowManager: socket() failed:" << strerror(errno);
    return std::nullopt;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;

  QByteArray utf8Path = path.toUtf8();
  if (utf8Path.size() >= static_cast<int>(sizeof(addr.sun_path))) {
    qWarning() << "Niri::WindowManager: NIRI_SOCKET path is too long";
    close(fd);
    return std::nullopt;
  }

  strncpy(addr.sun_path, utf8Path.constData(), sizeof(addr.sun_path) - 1);

  if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    qWarning() << "Niri::WindowManager: connect() failed:" << strerror(errno);
    close(fd);
    return std::nullopt;
  }

  return fd;
}

bool sendAll(int fd, const QByteArray &payload) {
  ssize_t sent = 0;
  while (sent < payload.size()) {
    ssize_t rc = send(fd, payload.constData() + sent, payload.size() - sent, MSG_NOSIGNAL);
    if (rc <= 0) {
      qWarning() << "Niri::WindowManager: send() failed:" << strerror(errno);
      return false;
    }
    sent += rc;
  }

  return true;
}

std::optional<QString> recvLineBlocking(int fd, QString &buffer) {
  while (true) {
    int idx = buffer.indexOf('\n');
    if (idx >= 0) {
      QString line = buffer.left(idx);
      buffer.remove(0, idx + 1);
      return line;
    }

    std::array<char, 1 << 12> tmp{};
    ssize_t rc = recv(fd, tmp.data(), tmp.size(), 0);
    if (rc <= 0) {
      if (rc < 0) { qWarning() << "Niri::WindowManager: recv() failed:" << strerror(errno); }
      return std::nullopt;
    }

    buffer += QString::fromUtf8(tmp.data(), rc);
  }
}

std::optional<QJsonValue> parseReply(const QString &line) {
  auto doc = QJsonDocument::fromJson(line.toUtf8());
  if (!doc.isObject()) {
    qWarning() << "Niri::WindowManager: malformed reply:" << line;
    return std::nullopt;
  }

  auto obj = doc.object();
  if (obj.contains("Err")) {
    qWarning() << "Niri::WindowManager: request failed:" << obj.value("Err");
    return std::nullopt;
  }

  if (!obj.contains("Ok")) {
    qWarning() << "Niri::WindowManager: reply missing Ok field";
    return std::nullopt;
  }

  return obj.value("Ok");
}

std::optional<QJsonValue> parseVariant(const QJsonValue &okValue, const char *variant) {
  if (okValue.isString()) {
    if (okValue.toString() == variant) { return QJsonValue(); }
    return std::nullopt;
  }

  if (!okValue.isObject()) { return std::nullopt; }

  auto obj = okValue.toObject();
  if (!obj.contains(variant)) { return std::nullopt; }

  return obj.value(variant);
}

std::optional<Window::FocusTimestamp> parseFocusTimestamp(const QJsonValue &value) {
  if (!value.isObject()) { return std::nullopt; }

  auto obj = value.toObject();
  if (!obj.contains("secs") || !obj.contains("nanos")) { return std::nullopt; }

  Window::FocusTimestamp ts;
  ts.secs = static_cast<int64_t>(obj.value("secs").toInteger());
  ts.nanos = static_cast<int32_t>(obj.value("nanos").toInt());
  return ts;
}

} // namespace

void Window::updateFromJson(const QJsonObject &json) {
  m_id = QString::number(static_cast<qulonglong>(json.value("id").toInteger()));
  m_title = json.value("title").toString();
  m_wmClass = json.value("app_id").toString();

  if (json.contains("pid") && !json.value("pid").isNull()) {
    m_pid = json.value("pid").toInt();
  } else {
    m_pid = std::nullopt;
  }

  if (json.contains("workspace_id") && !json.value("workspace_id").isNull()) {
    m_workspaceId = QString::number(static_cast<qulonglong>(json.value("workspace_id").toInteger()));
  } else {
    // wm-router currently assumes workspace() always has a value.
    m_workspaceId = QString();
  }

  if (json.contains("is_focused")) { m_focused = json.value("is_focused").toBool(); }
  if (json.contains("focus_timestamp")) {
    m_focusTimestamp = parseFocusTimestamp(json.value("focus_timestamp"));
  }
}

void Workspace::updateFromJson(const QJsonObject &json) {
  m_id = QString::number(static_cast<qulonglong>(json.value("id").toInteger()));
  m_name = json.value("name").toString();
  m_monitor = json.value("output").toString();

  if (json.contains("is_active")) { m_active = json.value("is_active").toBool(); }
  if (json.contains("is_focused")) { m_focused = json.value("is_focused").toBool(); }
}

WindowManager::WindowManager()
    : m_eventNotifier(new QSocketNotifier(QSocketNotifier::Type::Read, this)) {
  m_eventNotifier->setEnabled(false);
  connect(m_eventNotifier, &QSocketNotifier::activated, this,
          [this](QSocketDescriptor, QSocketNotifier::Type) { handleEventSocketReadable(); });
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
    auto niriWindow = std::static_pointer_cast<Window>(window);
    if (niriWindow->isFocused()) { return window; }
  }

  return nullptr;
}

AbstractWindowManager::WorkspacePtr WindowManager::getActiveWorkspace() const {
  for (const auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (niriWorkspace->isFocused()) { return workspace; }
  }

  for (const auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (niriWorkspace->isActive()) { return workspace; }
  }

  return nullptr;
}

void WindowManager::focusWindowSync(const AbstractWindow &window) const {
  auto id = parseWindowId(window.id());
  if (!id.has_value()) { return; }

  QJsonObject payload;
  payload.insert("id", static_cast<qint64>(*id));
  auto res = sendActionRequest("FocusWindow", payload);

  if (!res.has_value()) {
    qWarning() << "Niri::WindowManager: failed to focus window" << window.id();
    return;
  }

  emit windowsChanged();
}

bool WindowManager::closeWindow(const AbstractWindow &window) const {
  auto id = parseWindowId(window.id());
  if (!id.has_value()) { return false; }

  QJsonObject payload;
  payload.insert("id", static_cast<qint64>(*id));
  auto res = sendActionRequest("CloseWindow", payload);

  if (!res.has_value()) { return false; }

  emit windowsChanged();
  return true;
}

bool WindowManager::supportsPaste() const { return false; }

bool WindowManager::pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) {
  return false;
}

bool WindowManager::ping() const { return sendUnitRequest("Version").has_value(); }

bool WindowManager::isActivatable() const {
  if (!Environment::isWaylandSession()) { return false; }
  auto socket = niriSocketPath();
  if (socket.isEmpty()) { return false; }
  return std::filesystem::exists(socket.toStdString());
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

  const QByteArray eventReq("\"EventStream\"\n");
  if (!sendAll(*fd, eventReq)) {
    close(*fd);
    return false;
  }

  auto line = recvLineBlocking(*fd, m_eventBuffer);
  if (!line.has_value()) {
    close(*fd);
    return false;
  }

  auto reply = parseReply(*line);
  if (!reply.has_value() || !parseVariant(*reply, "Handled").has_value()) {
    qWarning() << "Niri::WindowManager: EventStream subscription failed";
    close(*fd);
    return false;
  }

  m_eventFd = *fd;
  m_eventNotifier->setSocket(m_eventFd);
  m_eventNotifier->setEnabled(true);

  // Process any event lines that may have already arrived with the subscription reply.
  while (true) {
    int idx = m_eventBuffer.indexOf('\n');
    if (idx < 0) { break; }
    QString eventLine = m_eventBuffer.left(idx);
    m_eventBuffer.remove(0, idx + 1);
    processEventLine(eventLine);
  }

  return true;
}

void WindowManager::handleEventSocketReadable() {
  if (m_eventFd < 0) { return; }

  std::array<char, 1 << 14> tmp{};
  while (true) {
    ssize_t rc = recv(m_eventFd, tmp.data(), tmp.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
    if (rc <= 0) {
      if (rc < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        qWarning() << "Niri::WindowManager: event socket recv failed:" << strerror(errno);
      }
      break;
    }

    m_eventBuffer += QString::fromUtf8(tmp.data(), rc);
  }

  while (true) {
    int idx = m_eventBuffer.indexOf('\n');
    if (idx < 0) { break; }
    QString line = m_eventBuffer.left(idx);
    m_eventBuffer.remove(0, idx + 1);
    processEventLine(line);
  }
}

void WindowManager::processEventLine(const QString &line) {
  auto doc = QJsonDocument::fromJson(line.toUtf8());
  if (!doc.isObject()) { return; }

  auto event = doc.object();
  bool changed = false;

  if (event.contains("WindowsChanged")) {
    auto payload = event.value("WindowsChanged").toObject();
    updateWindowsFromArray(payload.value("windows").toArray());
    changed = true;
  } else if (event.contains("WindowOpenedOrChanged")) {
    auto payload = event.value("WindowOpenedOrChanged").toObject();
    upsertWindow(payload.value("window").toObject());
    changed = true;
  } else if (event.contains("WindowClosed")) {
    auto payload = event.value("WindowClosed").toObject();
    removeWindow(QString::number(static_cast<qulonglong>(payload.value("id").toInteger())));
    changed = true;
  } else if (event.contains("WindowFocusChanged")) {
    auto payload = event.value("WindowFocusChanged").toObject();
    if (payload.contains("id") && !payload.value("id").isNull()) {
      setFocusedWindow(QString::number(static_cast<qulonglong>(payload.value("id").toInteger())));
    } else {
      setFocusedWindow(QString());
    }
    changed = true;
  } else if (event.contains("WindowFocusTimestampChanged")) {
    auto payload = event.value("WindowFocusTimestampChanged").toObject();
    QString id = QString::number(static_cast<qulonglong>(payload.value("id").toInteger()));
    setWindowFocusTimestamp(id, payload.value("focus_timestamp"));
    changed = true;
  } else if (event.contains("WorkspacesChanged")) {
    auto payload = event.value("WorkspacesChanged").toObject();
    updateWorkspacesFromArray(payload.value("workspaces").toArray());
    changed = true;
  } else if (event.contains("WorkspaceActivated")) {
    auto payload = event.value("WorkspaceActivated").toObject();
    setActiveWorkspace(QString::number(static_cast<qulonglong>(payload.value("id").toInteger())),
                       payload.value("focused").toBool());
    changed = true;
  }

  if (changed) { emit windowsChanged(); }
}

void WindowManager::updateWindowsFromArray(const QJsonArray &windows) {
  WindowList newList;
  newList.reserve(windows.size());

  for (const auto &it : windows) {
    if (!it.isObject()) { continue; }
    auto window = std::make_shared<Window>();
    window->updateFromJson(it.toObject());
    newList.emplace_back(window);
  }

  m_windows = std::move(newList);
  sortWindowsByFocusTimestamp();
}

void WindowManager::upsertWindow(const QJsonObject &windowJson) {
  QString id = QString::number(static_cast<qulonglong>(windowJson.value("id").toInteger()));
  for (auto &window : m_windows) {
    auto niriWindow = std::static_pointer_cast<Window>(window);
    if (niriWindow->id() == id) {
      niriWindow->updateFromJson(windowJson);
      if (niriWindow->isFocused()) { setFocusedWindow(id); }
      sortWindowsByFocusTimestamp();
      return;
    }
  }

  auto window = std::make_shared<Window>();
  window->updateFromJson(windowJson);
  if (window->isFocused()) { setFocusedWindow(window->id()); }
  m_windows.emplace_back(window);
  sortWindowsByFocusTimestamp();
}

void WindowManager::removeWindow(const QString &id) {
  auto pred = [&](const auto &window) { return window->id() == id; };
  std::erase_if(m_windows, pred);
  sortWindowsByFocusTimestamp();
}

void WindowManager::setFocusedWindow(const QString &id) {
  for (auto &window : m_windows) {
    auto niriWindow = std::static_pointer_cast<Window>(window);
    niriWindow->setFocused(!id.isEmpty() && niriWindow->id() == id);
  }
}

void WindowManager::setWindowFocusTimestamp(const QString &id, const QJsonValue &focusTimestamp) {
  auto parsed = parseFocusTimestamp(focusTimestamp);
  for (auto &window : m_windows) {
    auto niriWindow = std::static_pointer_cast<Window>(window);
    if (niriWindow->id() == id) {
      niriWindow->setFocusTimestamp(parsed);
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
    bool lok = false;
    bool rok = false;
    qulonglong lid = lw->id().toULongLong(&lok);
    qulonglong rid = rw->id().toULongLong(&rok);

    if (lts.has_value() != rts.has_value()) { return lts.has_value(); }
    if (!lts.has_value()) {
      if (lok && rok) { return lid > rid; }
      return lw->id() > rw->id();
    }

    if (lts->secs != rts->secs) { return lts->secs > rts->secs; }
    if (lts->nanos != rts->nanos) { return lts->nanos > rts->nanos; }
    if (lok && rok) { return lid > rid; }
    if (lw->id() != rw->id()) { return lw->id() > rw->id(); }
    return false;
  });
}

void WindowManager::updateWorkspacesFromArray(const QJsonArray &workspaces) {
  WorkspaceList newList;
  newList.reserve(workspaces.size());

  for (const auto &it : workspaces) {
    if (!it.isObject()) { continue; }
    auto workspace = std::make_shared<Workspace>();
    workspace->updateFromJson(it.toObject());
    newList.emplace_back(workspace);
  }

  m_workspaces = std::move(newList);
}

void WindowManager::setActiveWorkspace(const QString &id, bool focused) {
  QString targetMonitor;
  for (auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (niriWorkspace->id() == id) {
      targetMonitor = niriWorkspace->monitor();
      break;
    }
  }

  for (auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (!targetMonitor.isEmpty() && niriWorkspace->monitor() == targetMonitor) {
      niriWorkspace->setActive(false);
    }
    if (focused) { niriWorkspace->setFocused(false); }
  }

  for (auto &workspace : m_workspaces) {
    auto niriWorkspace = std::static_pointer_cast<Workspace>(workspace);
    if (niriWorkspace->id() == id) {
      niriWorkspace->setActive(true);
      if (focused) { niriWorkspace->setFocused(true); }
      break;
    }
  }
}

std::optional<QJsonValue> WindowManager::sendRequest(const QString &rawRequest) const {
  auto fd = connectSocket(niriSocketPath());
  if (!fd.has_value()) { return std::nullopt; }

  QString buffer;
  QByteArray payload = rawRequest.toUtf8();
  payload += '\n';

  if (!sendAll(*fd, payload)) {
    close(*fd);
    return std::nullopt;
  }

  auto line = recvLineBlocking(*fd, buffer);
  close(*fd);
  if (!line.has_value()) { return std::nullopt; }

  return parseReply(*line);
}

std::optional<QJsonValue> WindowManager::sendActionRequest(const QString &actionName,
                                                               const QJsonObject &payload) const {
  QJsonObject actionObj;
  actionObj.insert(actionName, payload);

  QJsonObject reqObj;
  reqObj.insert("Action", actionObj);

  QString req = QString::fromUtf8(QJsonDocument(reqObj).toJson(QJsonDocument::Compact));
  auto reply = sendRequest(req);
  if (!reply.has_value()) { return std::nullopt; }

  return parseVariant(*reply, "Handled");
}

std::optional<QJsonValue> WindowManager::sendUnitRequest(const char *name) const {
  QString req = QString("\"%1\"").arg(name);
  return sendRequest(req);
}

std::optional<uint64_t> WindowManager::parseWindowId(const QString &id) {
  bool ok = false;
  auto value = id.toULongLong(&ok);
  if (!ok) {
    qWarning() << "Niri::WindowManager: invalid window id" << id;
    return std::nullopt;
  }
  return value;
}

QString WindowManager::niriSocketPath() { return qEnvironmentVariable("NIRI_SOCKET"); }

} // namespace Niri

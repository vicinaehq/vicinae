#include <format>
#include <glaze/glaze.hpp>
#include <QCoreApplication>
#include <qprocess.h>
#include <string_view>
#include <vector>
#include "hyprland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-ipc.hpp"
#include "services/window-manager/hyprland/hypr-workspace.hpp"
#include "services/window-manager/hyprland/hyprctl.hpp"
#include "vicinae.hpp"

using Hyprctl = Hyprland::Controller;
namespace ipc = Hyprland::ipc;

namespace {

constexpr glz::opts PARSE_OPTS{.error_on_unknown_keys = false};

bool dispatchLua(std::string_view expr) { return Hyprctl::oneshot(std::format("dispatch {}", expr)) == "ok"; }

std::string windowTarget(const AbstractWindowManager::AbstractWindow &window) {
  return std::format("address:{}", window.id().toStdString());
}

template <class T> std::optional<T> parseReply(const QByteArray &response) {
  T value{};
  auto view = std::string_view(response.constData(), static_cast<std::size_t>(response.size()));
  if (glz::read<PARSE_OPTS>(value, view)) {
    qWarning() << "Hyprland: failed to parse hyprctl reply:" << response;
    return std::nullopt;
  }
  return value;
}

} // namespace

HyprlandWindowManager::HyprlandWindowManager() {
  connect(&m_ev, &Hyprland::EventListener::openwindow, this, [this]() { emit windowsChanged(); });
  connect(&m_ev, &Hyprland::EventListener::closewindow, this, [this]() { emit windowsChanged(); });
  connect(&m_ev, &Hyprland::EventListener::activewindowchanged, this, [this]() { emit focusChanged(); });
}

QString HyprlandWindowManager::id() const { return "hyprland"; }
QString HyprlandWindowManager::displayName() const { return "Hyprland"; }

AbstractWindowManager::WindowList HyprlandWindowManager::listWindowsSync() const {
  auto clients = parseReply<std::vector<ipc::Window>>(Hyprctl::oneshot("-j/clients"));
  if (!clients.has_value()) { return {}; }

  WindowList windows;
  windows.reserve(clients->size());

  for (const auto &client : *clients) {
    windows.emplace_back(std::make_shared<HyprlandWindow>(client));
  }

  return windows;
}

AbstractWindowManager::WindowPtr HyprlandWindowManager::getFrontmostWindowSync() const {
  auto clients = parseReply<std::vector<ipc::Window>>(Hyprctl::oneshot("-j/clients"));
  auto activeWs = parseReply<ipc::Workspace>(Hyprctl::oneshot("-j/activeworkspace"));
  if (!clients.has_value() || !activeWs.has_value()) { return nullptr; }

  const auto ownPid = QCoreApplication::applicationPid();
  const ipc::Window *frontmost = nullptr;

  for (const auto &client : *clients) {
    if (client.pid == ownPid || client.workspace.id != activeWs->id) continue;
    if (client.focusHistoryId < 0) continue;
    if (!frontmost || client.focusHistoryId < frontmost->focusHistoryId) frontmost = &client;
  }

  if (!frontmost) return nullptr;
  return std::make_shared<HyprlandWindow>(*frontmost);
}

AbstractWindowManager::WindowPtr HyprlandWindowManager::getFocusedWindowSync() const {
  auto active = parseReply<ipc::Window>(Hyprctl::oneshot("-j/activewindow"));
  if (!active.has_value() || active->address.empty()) { return nullptr; }

  return std::make_shared<HyprlandWindow>(*active);
}

void HyprlandWindowManager::focusWindowSync(const AbstractWindow &window) const {
  dispatchLua(std::format(R"(hl.dsp.focus({{ window = "{}" }}))", windowTarget(window)));
}

void HyprlandWindowManager::focusWorkspaceSync(const AbstractWorkspace &workspace) const {
  dispatchLua(std::format(R"(hl.dsp.focus({{ workspace = "{}" }}))", workspace.id().toStdString()));
}

bool HyprlandWindowManager::closeWindow(const AbstractWindow &window) const {
  if (!dispatchLua(std::format(R"(hl.dsp.window.close({{ window = "{}" }}))", windowTarget(window)))) {
    return false;
  }

  emit windowsChanged();
  return true;
}

bool HyprlandWindowManager::toggleFullscreen(const AbstractWindow &window) {
  if (!dispatchLua(std::format(R"(hl.dsp.window.fullscreen({{ action = "toggle", window = "{}" }}))",
                               windowTarget(window)))) {
    return false;
  }

  emit windowsChanged();
  return true;
}

bool HyprlandWindowManager::toggleFloating(const AbstractWindow &window) {
  if (!dispatchLua(std::format(R"(hl.dsp.window.float({{ action = "toggle", window = "{}" }}))",
                               windowTarget(window)))) {
    return false;
  }

  emit windowsChanged();
  return true;
}

bool HyprlandWindowManager::isActivatable() const {
  return QProcessEnvironment::systemEnvironment().contains("HYPRLAND_INSTANCE_SIGNATURE");
}

bool HyprlandWindowManager::ping() const {
  // XXX - Implement actual ping
  return true;
}

bool HyprlandWindowManager::hasWorkspaces() const { return true; }

AbstractWindowManager::WorkspacePtr HyprlandWindowManager::getActiveWorkspace() const {
  auto active = parseReply<ipc::Workspace>(Hyprctl::oneshot("-j/activeworkspace"));
  if (!active.has_value()) { return nullptr; }

  return std::make_shared<Hyprland::Workspace>(*active);
}

AbstractWindowManager::WorkspaceList HyprlandWindowManager::listWorkspaces() const {
  auto parsed = parseReply<std::vector<ipc::Workspace>>(Hyprctl::oneshot("-j/workspaces"));
  if (!parsed.has_value()) { return {}; }

  WorkspaceList workspaces;
  workspaces.reserve(parsed->size());

  for (const auto &workspace : *parsed) {
    workspaces.emplace_back(std::make_shared<Hyprland::Workspace>(workspace));
  }

  return workspaces;
}

void HyprlandWindowManager::start() { m_ev.start(); }

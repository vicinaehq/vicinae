#include <format>
#include "hyprland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-workspace.hpp"
#include "services/window-manager/hyprland/hyprctl.hpp"
#include "vicinae.hpp"

using Hyprctl = Hyprland::Controller;

HyprlandWindowManager::HyprlandWindowManager() {
  connect(&m_ev, &Hyprland::EventListener::openwindow, this, [this]() { emit windowsChanged(); });
  connect(&m_ev, &Hyprland::EventListener::closewindow, this, [this]() { emit windowsChanged(); });
  connect(&m_ev, &Hyprland::EventListener::activewindowchanged, this, [this]() { emit focusChanged(); });
}

QString HyprlandWindowManager::id() const { return "hyprland"; }
QString HyprlandWindowManager::displayName() const { return "Hyprland"; }

AbstractWindowManager::WindowList HyprlandWindowManager::listWindowsSync() const {
  auto response = Hyprctl::oneshot("-j/clients");
  auto json = QJsonDocument::fromJson(response);
  WindowList windows;

  for (const auto &item : json.array()) {
    windows.emplace_back(std::make_shared<HyprlandWindow>(item.toObject()));
  }

  return windows;
}


AbstractWindowManager::WindowPtr HyprlandWindowManager::getFocusedWindowSync() const {
  auto response = Hyprctl::oneshot("-j/activewindow");
  auto json = QJsonDocument::fromJson(response);

  if (json.isEmpty()) { return nullptr; }

  return std::make_shared<HyprlandWindow>(json.object());
}

void HyprlandWindowManager::focusWindowSync(const AbstractWindow &window) const {
  auto addr = window.id().toStdString();
  Hyprctl::oneshot(std::format(
      "[[BATCH]]dispatch focuswindow address:{0}"
      ";eval hl.dispatch(hl.dsp.focus({{window=\"address:{0}\"}}))",
      addr));
}

bool HyprlandWindowManager::closeWindow(const AbstractWindow &window) const {
  auto addr = window.id().toStdString();
  Hyprctl::oneshot(std::format(
      "[[BATCH]]dispatch closewindow address:{0}"
      ";eval hl.dispatch(hl.dsp.window.close({{window=\"address:{0}\"}}))",
      addr));
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
  auto response = Hyprctl::oneshot("-j/activeworkspace");
  auto json = QJsonDocument::fromJson(response);

  if (json.isEmpty()) { return nullptr; }

  return std::make_shared<Hyprland::Workspace>(json.object());
}

AbstractWindowManager::WorkspaceList HyprlandWindowManager::listWorkspaces() const {
  auto response = Hyprctl::oneshot("-j/workspaces");
  auto json = QJsonDocument::fromJson(response);
  WorkspaceList workspaces;

  for (const auto &item : json.array()) {
    workspaces.emplace_back(std::make_shared<Hyprland::Workspace>(item.toObject()));
  }

  return workspaces;
}

void HyprlandWindowManager::start() { m_ev.start(); }

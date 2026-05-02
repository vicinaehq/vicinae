#include <format>
#include <QThread>
#include "hyprland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-workspace.hpp"
#include "services/window-manager/hyprland/hyprctl.hpp"
#include "vicinae.hpp"

using Hyprctl = Hyprland::Controller;

namespace {
bool isScrollingLayout() {
  auto response = Hyprctl::oneshot("-j/getoption general:layout");
  auto json = QJsonDocument::fromJson(response);

  if (!json.isObject()) return false;

  return json.object().value("str").toString() == "scrolling";
}

bool waitUntilFocused(QStringView address, int timeoutMs = 200) {
  constexpr int pollIntervalMs = 10;

  for (int elapsed = 0; elapsed <= timeoutMs; elapsed += pollIntervalMs) {
    auto response = Hyprctl::oneshot("-j/activewindow");
    auto json = QJsonDocument::fromJson(response);

    if (json.isObject() && json.object().value("address").toString() == address) return true;

    QThread::msleep(pollIntervalMs);
  }

  return false;
}
} // namespace

HyprlandWindowManager::HyprlandWindowManager() {
  connect(&m_ev, &Hyprland::EventListener::openwindow, this, [this]() { emit windowsChanged(); });
  connect(&m_ev, &Hyprland::EventListener::closewindow, this, [this]() { emit windowsChanged(); });
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

void HyprlandWindowManager::applyLayerRule(std::string_view rule) {
  Hyprctl::oneshot(std::format("keyword layerrule {}, {}", rule, Omnicast::LAYER_SCOPE));
}

void HyprlandWindowManager::applyLayerRules() {
  if (m_dimAround) { applyLayerRule("dimaround"); }
}

bool HyprlandWindowManager::setDimAround(bool value) {
  m_dimAround = value;
  applyLayerRules();
  return false;
}

AbstractWindowManager::WindowPtr HyprlandWindowManager::getFocusedWindowSync() const {
  auto response = Hyprctl::oneshot("-j/activewindow");
  auto json = QJsonDocument::fromJson(response);

  if (json.isEmpty()) { return nullptr; }

  return std::make_shared<HyprlandWindow>(json.object());
}

void HyprlandWindowManager::focusWindowSync(const AbstractWindow &window) const {
  const auto activeWorkspace = getActiveWorkspace();
  const auto targetWorkspace = window.workspace();
  const bool sameWorkspace = activeWorkspace && targetWorkspace && activeWorkspace->id() == *targetWorkspace;

  Hyprctl::oneshot(std::format("dispatch focuswindow address:{}", window.id().toStdString()));

  if (!sameWorkspace || !isScrollingLayout()) return;

  if (!waitUntilFocused(window.id())) return;

  Hyprctl::oneshot("dispatch layoutmsg center");
}

bool HyprlandWindowManager::closeWindow(const AbstractWindow &window) const {
  Hyprctl::oneshot(std::format("dispatch closewindow address:{}", window.id().toStdString()));
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

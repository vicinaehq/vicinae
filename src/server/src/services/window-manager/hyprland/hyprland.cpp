#include <format>
#include "hyprland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-workspace.hpp"
#include "services/window-manager/hyprland/hyprctl.hpp"
#include "lib/wayland/virtual-keyboard.hpp"
#include "vicinae.hpp"
#include "wayland/globals.hpp"

using Hyprctl = Hyprland::Controller;

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

bool HyprlandWindowManager::pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) {
  using VK = Wayland::VirtualKeyboard;
  Wayland::VirtualKeyboard kb;

  if (!kb.isAvailable()) { return false; }

  if (app && app->isTerminalEmulator()) {
    return kb.sendKeySequence(XKB_KEY_v, VK::MOD_CTRL | VK::MOD_SHIFT);
  }

  return kb.sendKeySequence(XKB_KEY_v, VK::MOD_CTRL);
}

AbstractWindowManager::WindowPtr HyprlandWindowManager::getFocusedWindowSync() const {
  auto response = Hyprctl::oneshot("-j/activewindow");
  auto json = QJsonDocument::fromJson(response);

  if (json.isEmpty()) { return nullptr; }

  return std::make_shared<HyprlandWindow>(json.object());
}

bool HyprlandWindowManager::supportsPaste() const { return Wayland::Globals::virtualKeyboardManager(); }

void HyprlandWindowManager::focusWindowSync(const AbstractWindow &window) const {
  Hyprctl::oneshot(std::format("dispatch focuswindow address:{}", window.id().toStdString()));
}

bool HyprlandWindowManager::closeWindow(const AbstractWindow &window) const {
  Hyprctl::oneshot(std::format("dispatch closewindow address:{}", window.id().toStdString()));
  emit windowsChanged();

  return true;
}

bool HyprlandWindowManager::isActivatable() const {
  bool isWayland = QGuiApplication::platformName() == "wayland";

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

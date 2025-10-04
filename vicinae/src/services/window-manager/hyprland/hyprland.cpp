#include "hyprland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-workspace.hpp"
#include "services/window-manager/hyprland/hyprctl.hpp"

using Hyprctl = Hyprland::Controller;

QString HyprlandWindowManager::stringifyModifiers(QFlags<Qt::KeyboardModifier> mods) {
  QList<QString> smods;

  if (mods.testFlag(Qt::KeyboardModifier::ControlModifier)) smods.emplace_back("CONTROL");
  if (mods.testFlag(Qt::KeyboardModifier::ShiftModifier)) smods.emplace_back("SHIFT");
  if (mods.testFlag(Qt::KeyboardModifier::MetaModifier)) smods.emplace_back("SUPER");
  if (mods.testFlag(Qt::KeyboardModifier::AltModifier)) smods.emplace_back("ALT");

  return smods.join('&');
}

HyprlandWindowManager::HyprlandWindowManager() {
  connect(&m_ev, &Hyprland::EventListener::openwindow, this, [this]() { emit windowsChanged(); });
  connect(&m_ev, &Hyprland::EventListener::closewindow, this, [this]() { emit windowsChanged(); });
}

QString HyprlandWindowManager::stringifyKey(Qt::Key key) const { return QKeySequence(key).toString(); }
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

bool HyprlandWindowManager::supportsInputForwarding() const { return true; }

bool HyprlandWindowManager::sendShortcutSync(const AbstractWindow &window,
                                             const Keyboard::Shortcut &shortcut) {
  auto cmd = QString("dispatch sendshortcut %1,%2,address:%3")
                 .arg(stringifyModifiers(shortcut.mods()))
                 .arg(stringifyKey(shortcut.key()))
                 .arg(window.id());

  // qWarning() << "send dispatcher" << cmd;
  Hyprctl::oneshot(cmd.toStdString());

  return true;
}

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

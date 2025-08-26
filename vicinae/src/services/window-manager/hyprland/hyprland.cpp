#include "hyprland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hyprctl.hpp"
#include <ranges>

HyprlandWindow::HyprlandWindow(const QJsonObject &json) {
  m_id = json.value("address").toString();
  m_title = json.value("title").toString();
  m_wmClass = json.value("class").toString();
}

QString HyprlandWindowManager::stringifyModifiers(QFlags<Qt::KeyboardModifier> mods) {
  QList<QString> smods;

  if (mods.testFlag(Qt::KeyboardModifier::ControlModifier)) smods.emplace_back("CONTROL");
  if (mods.testFlag(Qt::KeyboardModifier::ShiftModifier)) smods.emplace_back("SHIFT");
  if (mods.testFlag(Qt::KeyboardModifier::MetaModifier)) smods.emplace_back("SUPER");
  if (mods.testFlag(Qt::KeyboardModifier::AltModifier)) smods.emplace_back("ALT");

  return smods.join('&');
}

QString HyprlandWindowManager::stringifyKey(Qt::Key key) const { return QKeySequence(key).toString(); }
QString HyprlandWindowManager::id() const { return "hyprland"; }
QString HyprlandWindowManager::displayName() const { return "Hyprland"; }

AbstractWindowManager::WindowList HyprlandWindowManager::listWindowsSync() const {
  auto response = Hyprctl::oneshot("-j/clients");
  auto json = QJsonDocument::fromJson(response);
  auto windows = json.array() |
                 std::views::transform([](const QJsonValue &value) -> std::shared_ptr<AbstractWindow> {
                   return std::make_shared<HyprlandWindow>(value.toObject());
                 }) |
                 std::ranges::to<std::vector>();

  return windows;
}

AbstractWindowManager::WindowPtr HyprlandWindowManager::getFocusedWindowSync() const {
  auto response = Hyprctl::oneshot("-j/activewindow");
  auto json = QJsonDocument::fromJson(response);

  if (json.isEmpty()) { return nullptr; }

  return std::make_shared<HyprlandWindow>(json.object());
}

bool HyprlandWindowManager::supportsInputForwarding() const { return true; }

bool HyprlandWindowManager::sendShortcutSync(const AbstractWindow &window, const KeyboardShortcut &shortcut) {
  auto cmd = QString("dispatch sendshortcut %1,%2,address:%3")
                 .arg(stringifyModifiers(shortcut.modifiers))
                 .arg(stringifyKey(shortcut.key))
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

  return true;
}

bool HyprlandWindowManager::isActivatable() const {
  bool isWayland = QGuiApplication::platformName() == "wayland";

  return isWayland && QProcessEnvironment::systemEnvironment().contains("HYPRLAND_INSTANCE_SIGNATURE");
}

bool HyprlandWindowManager::ping() const {
  // XXX - Implement actual ping
  return true;
}

void HyprlandWindowManager::start() const {}

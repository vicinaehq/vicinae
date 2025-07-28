#pragma once
#include "wm/hyprland/hyprctl.hpp"
#include "wm/window-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <lib/xkbcommon-utils.hpp>
#include <format>
#include <memory>
#include <numeric>
#include <qapplication.h>
#include <qfuture.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qkeysequence.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <QJsonArray>
#include <QJsonObject>
#include <qpromise.h>
#include <qstringview.h>
#include <ranges>
#include <qtcoreexports.h>

class HyprlandWindow : public AbstractWindowManager::Window {
public:
  HyprlandWindow(const QString &id, const QString &title, const QString &wmClass) {
    _id = id;
    _title = title;
    _wmClass = wmClass;
  }
};

class HyprlandWindowManager : public AbstractWindowManager {
  QString stringifyModifiers(QFlags<Qt::KeyboardModifier> mods) {
    QList<QString> smods;

    if (mods.testFlag(Qt::KeyboardModifier::ControlModifier)) smods.emplace_back("CONTROL");
    if (mods.testFlag(Qt::KeyboardModifier::ShiftModifier)) smods.emplace_back("SHIFT");
    if (mods.testFlag(Qt::KeyboardModifier::MetaModifier)) smods.emplace_back("SUPER");
    if (mods.testFlag(Qt::KeyboardModifier::AltModifier)) smods.emplace_back("ALT");

    return smods.join('&');
  }

  QString stringifyKey(Qt::Key key) const { return QKeySequence(key).toString(); }

  std::shared_ptr<HyprlandWindow> deserializeWindow(const QJsonObject &obj) const {
    auto id = obj.value("address").toString();
    auto title = obj.value("title").toString();
    auto wmClass = obj.value("class").toString();

    return std::make_shared<HyprlandWindow>(id, title, wmClass);
  }

public:
  WindowList listWindowsSync() const override {
    auto response = Hyprctl::oneshot("-j/clients");
    auto json = QJsonDocument::fromJson(response);
    auto windows = json.array() | std::views::transform([&](const QJsonValue &value) {
                     return std::static_pointer_cast<Window>(deserializeWindow(value.toObject()));
                   }) |
                   std::ranges::to<std::vector>();

    return windows;
  }

  WorkspaceList listWorkspacesSync() const override { return {}; }

  void moveToWorkspaceSync(const Window &window, const Workspace &workspace) override {}

  std::shared_ptr<Window> getActiveWindowSync() const override {
    auto response = Hyprctl::oneshot("-j/activewindow");
    auto json = QJsonDocument::fromJson(response);

    if (json.isEmpty()) { return std::shared_ptr<Window>(); }

    auto obj = json.object();
    auto id = obj.value("address").toString();
    auto title = obj.value("title").toString();
    auto wmClass = obj.value("class").toString();

    return std::make_shared<Window>(HyprlandWindow(id, title, wmClass));
  }

  bool sendShortcutSync(const Window &window, const KeyboardShortcut &shortcut) override {
    auto cmd = QString("dispatch sendshortcut %1,%2,address:%3")
                   .arg(stringifyModifiers(shortcut.modifiers))
                   .arg(stringifyKey(shortcut.key))
                   .arg(window.id());

    qWarning() << "send dispatcher" << cmd;
    Hyprctl::oneshot(cmd.toStdString());

    return true;
  }

  void focusWindowSync(const Window &window) const override {
    Hyprctl::oneshot(std::format("dispatch focuswindow address:{}", window.id().toStdString()));
  }

  bool isActivatable() const override {
    bool isWayland = QGuiApplication::platformName() == "wayland";

    return isWayland && QProcessEnvironment::systemEnvironment().contains("HYPRLAND_INSTANCE_SIGNATURE");
  }

  bool ping() const override {
    // XXX - Implement actual ping
    return true;
  }

  QString name() const override { return "Hyprland"; }

  void start() const override {
    // initialize hypr ipc
  }
};

#pragma once
#include "wm/hyprland/hyprctl.hpp"
#include "wm/window-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <format>
#include <qapplication.h>
#include <qfuture.h>
#include <qjsondocument.h>
#include <qprocess.h>
#include <QJsonArray>
#include <QJsonObject>
#include <qpromise.h>
#include <qstringview.h>
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
public:
  WindowList listWindowsSync() const override {
    auto response = Hyprctl::oneshot("-j/clients");
    auto json = QJsonDocument::fromJson(response);
    WindowList list;
    auto arr = json.array();

    list.reserve(arr.size());

    for (const auto &window : arr) {
      auto obj = window.toObject();
      auto id = obj.value("address").toString();
      auto title = obj.value("title").toString();
      auto wmClass = obj.value("class").toString();

      list.emplace_back(std::make_shared<HyprlandWindow>(id, title, wmClass));
    }

    return list;
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

  void focusWindowSync(const Window &window) const override {
    Hyprctl::oneshot(std::format("dispatch focuswindow address:{}", window.id().toStdString()));
  }

  bool isActivatable() const override {
    bool isWayland = QGuiApplication::platformName() == "wayland";

    return isWayland && QProcessEnvironment::systemEnvironment().contains("HYPRLAND_INSTANCE_SIGNATURE");
  }

  bool ping() const override {}

  QString name() const override { return "Hyprland"; }

  void start() const override {
    // initialize hypr ipc
  }
};

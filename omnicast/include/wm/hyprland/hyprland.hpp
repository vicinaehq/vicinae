#pragma once
#include "wm/hyprland/hyprctl.hpp"
#include "wm/window-manager.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <format>
#include <qfuture.h>
#include <qjsondocument.h>
#include <qprocess.h>
#include <QJsonArray>
#include <QJsonObject>
#include <qpromise.h>
#include <qstringview.h>

class HyprlandWindow : public AbstractWindowManager::Window {
public:
  HyprlandWindow(const QString &id, const QString &title, const QString &wmClass) {
    _id = id;
    _title = title;
    _wmClass = wmClass;
  }
};

class HyprlandWindowManager : public AbstractWindowManager::AbstractWindowManager {
  QFuture<WindowList> listWindows() const override {
    return QtConcurrent::run([this]() {
      Hyprctl ctl;
      auto response = ctl.start("-j/clients");
      auto json = QJsonDocument::fromJson(response);
      WindowList list;

      for (const auto &window : json.array()) {
        auto obj = window.toObject();
        auto id = obj.value("address").toString();
        auto title = obj.value("title").toString();
        auto wmClass = obj.value("class").toString();

        list.push_back(std::make_shared<HyprlandWindow>(id, title, wmClass));
      }

      return list;
    });
  }

  QFuture<WorkspaceList> listWorkspaces() const override { return {}; }

  void moveToWorkspace(const Window &window, const Workspace &workspace) override {}

  QFuture<std::shared_ptr<Window>> getActiveWindow() override {
    return QtConcurrent::run([this]() {
      Hyprctl ctl;
      auto response = ctl.start("-j/clients");
      auto json = QJsonDocument::fromJson(response);

      if (json.isEmpty()) { return std::shared_ptr<Window>(); }

      auto obj = json.object();
      auto id = obj.value("address").toString();
      auto title = obj.value("title").toString();
      auto wmClass = obj.value("class").toString();

      return std::make_shared<Window>(HyprlandWindow(id, title, wmClass));
    });
  }

  QFuture<void> focus(const Window &window) const override {
    return QtConcurrent::run([this, window]() {
      Hyprctl ctl;

      ctl.start(std::format("dispatch focuswindow address:{}", window.id().toStdString()));
    });
  }

  bool isActivatable() const override {
    return QProcessEnvironment::systemEnvironment().contains("HYPRLAND_INSTANCE_SIGNATURE");
  }

  void start() const override {
    // initialize hypr ipc
  }
};

#pragma once
#include "generated/tsapi.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"

class ExtWindowManagementService : public tsapi::AbstractWindowManagement {
  using Void = tsapi::Result<void>;

public:
  ExtWindowManagementService(tsapi::RpcTransport &transport, WindowManager &wm, AppService &app)
      : AbstractWindowManagement(transport), m_wm(wm), m_app(app) {}

  tsapi::Result<bool>::Future focusWindow(const std::string &winId) override {
    auto win = m_wm.findWindowById(QString::fromStdString(winId));
    if (!win) return tsapi::Result<bool>::ok(false);

    m_wm.provider()->focusWindowSync(*win);
    return tsapi::Result<bool>::ok(true);
  }

  tsapi::Result<tsapi::Window>::Future getActiveWindow() override {
    auto activeWindow = m_wm.getFocusedWindow();
    if (!activeWindow) return tsapi::Result<tsapi::Window>::fail("No active window");

    auto result = serializeWindow(*activeWindow);
    result.active = true;

    if (auto app = m_app.findByClass(activeWindow->wmClass())) { result.app = toTsapiApp(*app); }

    return tsapi::Result<tsapi::Window>::ok(std::move(result));
  }

  tsapi::Result<tsapi::Workspace>::Future getActiveWorkspace() override {
    auto active = m_wm.provider()->getActiveWorkspace();
    if (!active) return tsapi::Result<tsapi::Workspace>::fail("No active workspace");

    return tsapi::Result<tsapi::Workspace>::ok({
        .id = active->id().toStdString(),
        .name = active->name().toStdString(),
        .active = true,
        .fullscreen = active->hasFullScreen(),
        .monitor = active->monitor().toStdString(),
    });
  }

  tsapi::Result<std::vector<tsapi::Window>>::Future
  getWindows(const std::optional<std::string> &workspaceId) override {
    auto activeWin = m_wm.provider()->getFocusedWindowSync();
    auto windows = m_wm.provider()->listWindowsSync();
    std::vector<tsapi::Window> result;
    result.reserve(windows.size());

    for (const auto &win : windows) {
      if (workspaceId && win->workspace().value_or("") != QString::fromStdString(*workspaceId)) continue;

      auto serialized = serializeWindow(*win);
      serialized.active = activeWin && activeWin->id() == win->id();

      if (auto app = m_app.findByClass(win->wmClass())) { serialized.app = toTsapiApp(*app); }

      result.emplace_back(std::move(serialized));
    }

    return tsapi::Result<std::vector<tsapi::Window>>::ok(std::move(result));
  }

  tsapi::Result<std::vector<tsapi::Screen>>::Future getScreens() override {
    auto screens = m_wm.provider()->listScreensSync();
    std::vector<tsapi::Screen> result;
    result.reserve(screens.size());

    for (const auto &screen : screens) {
      tsapi::Screen sc{
          .name = screen.name.toStdString(),
          .model = screen.model.toStdString(),
          .make = screen.manufacturer.toStdString(),
          .bounds = {.x = screen.bounds.x(),
                     .y = screen.bounds.y(),
                     .width = screen.bounds.width(),
                     .height = screen.bounds.height()},
      };
      if (screen.serial) sc.serial = screen.serial->toStdString();
      result.emplace_back(std::move(sc));
    }

    return tsapi::Result<std::vector<tsapi::Screen>>::ok(std::move(result));
  }

  tsapi::Result<std::vector<tsapi::Workspace>>::Future getWorkspaces() override {
    auto activeWorkspace = m_wm.provider()->getActiveWorkspace();
    auto workspaces = m_wm.provider()->listWorkspaces();
    std::vector<tsapi::Workspace> result;
    result.reserve(workspaces.size());

    for (const auto &workspace : workspaces) {
      bool const isActive = activeWorkspace && activeWorkspace->id() == workspace->id();
      result.emplace_back(tsapi::Workspace{
          .id = workspace->id().toStdString(),
          .name = workspace->name().toStdString(),
          .active = isActive,
          .fullscreen = workspace->hasFullScreen(),
          .monitor = workspace->monitor().toStdString(),
      });
    }

    return tsapi::Result<std::vector<tsapi::Workspace>>::ok(std::move(result));
  }

  tsapi::Result<bool>::Future setWindowBounds(const std::string &winId, const tsapi::Rect &bounds) override {
    return tsapi::Result<bool>::fail("Not implemented");
  }

private:
  static tsapi::Window serializeWindow(AbstractWindowManager::AbstractWindow &win) {
    tsapi::Window result;
    result.id = win.id().toStdString();
    result.title = win.title().toStdString();
    if (auto ws = win.workspace()) result.workspaceId = ws->toStdString();
    result.fullscreen = win.fullScreen();

    if (auto b = win.bounds()) {
      result.bounds = {.x = static_cast<int>(b->x),
                       .y = static_cast<int>(b->y),
                       .width = static_cast<int>(b->width),
                       .height = static_cast<int>(b->height)};
    }

    return result;
  }

  static tsapi::Application toTsapiApp(const ::AbstractApplication &app) {
    return {.id = app.id().toStdString(),
            .name = app.displayName().toStdString(),
            .icon = app.iconUrl().name().toStdString(),
            .path = app.path().string()};
  }

  WindowManager &m_wm;
  AppService &m_app;
};

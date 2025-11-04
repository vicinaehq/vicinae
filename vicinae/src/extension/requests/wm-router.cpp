#include "wm-router.hpp"
#include "proto/application.pb.h"
#include "proto/wm.pb.h"
#include <stdexcept>

namespace wm = proto::ext::wm;
namespace ext = proto::ext::extension;

WindowManagementRouter::WindowManagementRouter(WindowManager &wm, AppService &app) : m_wm(wm), m_app(app) {}

PromiseLike<ext::Response *> WindowManagementRouter::route(const wm::Request &req) {
  if (req.payload_case() != wm::Request::kPing && !m_wm.provider()->ping()) {
    throw std::runtime_error("Window management is unavailable in this environment");
  }

  switch (req.payload_case()) {
  case wm::Request::kPing:
    return wrapResponse(ping(req.ping()));
  case wm::Request::kGetActiveWindow:
    return wrapResponse(getActiveWindow(req.get_active_window()));
  case wm::Request::kGetWorkspaces:
    return wrapResponse(getWorkspaces(req.get_workspaces()));
  case wm::Request::kGetActiveWorkspace:
    return wrapResponse(getActiveWorkspace(req.get_active_workspace()));
  case wm::Request::kGetWindows:
    return wrapResponse(getWindows(req.get_windows()));
  case wm::Request::kGetScreens:
    return wrapResponse(getScreens(req.get_screens()));
  case wm::Request::kSetWindowBounds:
    throw std::runtime_error("Not implemented");
    // TODO: implement
    break;
  default:
    break;
  }

  return nullptr;
}

proto::ext::wm::Response *WindowManagementRouter::getScreens(const proto::ext::wm::GetScreensRequest &req) {
  auto res = new wm::Response;
  auto wmRes = new wm::GetScreensResponse;

  res->set_allocated_get_screens(wmRes);

  for (const auto &screen : m_wm.provider()->listScreensSync()) {
    auto sc = wmRes->add_screens();
    sc->set_name(screen.name.toStdString());
    sc->set_make(screen.manufacturer.toStdString());
    sc->set_model(screen.model.toStdString());
    sc->set_x(screen.bounds.x());
    sc->set_y(screen.bounds.y());
    sc->set_width(screen.bounds.width());
    sc->set_height(screen.bounds.height());
    if (auto serial = screen.serial) { sc->set_serial(serial->toStdString()); }
  }

  return res;
}

proto::ext::wm::Response *WindowManagementRouter::getWindows(const proto::ext::wm::GetWindowsRequest &req) {
  auto activeWorkspace = m_wm.provider()->getActiveWorkspace();
  auto activeWin = m_wm.provider()->getFocusedWindowSync();
  auto res = new wm::Response;
  auto winRes = new wm::GetWindowsResponse;

  res->set_allocated_get_windows(winRes);

  for (const auto &win : m_wm.provider()->listWindowsSync()) {
    qDebug() << "wid" << win->workspace() << "vs" << req.workspace_id();
    if (req.has_workspace_id() && win->workspace().value_or("") != req.workspace_id().c_str()) { continue; }

    bool isActive = activeWin && activeWin->id() == win->id();
    auto serializedWin = winRes->add_windows();

    initWindow(*win, *serializedWin);
    serializedWin->set_active(isActive);

    if (auto app = m_app.findByClass(win->wmClass())) {
      auto sapp = new proto::ext::application::Application;
      sapp->set_id(app->id().toStdString());
      sapp->set_name(app->displayName().toStdString());
      sapp->set_icon(app->iconUrl().name().toStdString());
      serializedWin->set_allocated_app(sapp);
    }
  }

  return res;
}

proto::ext::wm::Response *
WindowManagementRouter::getWorkspaces(const proto::ext::wm::GetWorkspacesRequest &req) {
  auto activeWorkspace = m_wm.provider()->getActiveWorkspace();
  auto res = new wm::Response;
  auto winRes = new wm::GetWorkspacesResponse;

  res->set_allocated_get_workspaces(winRes);

  for (const auto &workspace : m_wm.provider()->listWorkspaces()) {
    bool isActive = activeWorkspace && activeWorkspace->id() == workspace->id();
    auto serializedWorkspace = winRes->add_workspaces();

    initWorkspace(*workspace, *serializedWorkspace);
    serializedWorkspace->set_active(isActive);
  }

  return res;
}

proto::ext::wm::Response *
WindowManagementRouter::getActiveWindow(const proto::ext::wm::GetActiveWindowRequest &req) {
  auto activeWindow = m_wm.getFocusedWindow();

  if (!activeWindow) { throw std::runtime_error("No active window"); }

  auto res = new wm::Response;
  auto winRes = new wm::GetActiveWindowResponse;
  auto win = serializeWindow(*activeWindow);

  win->set_active(true);

  if (auto app = m_app.findByClass(activeWindow->wmClass())) {
    auto sapp = new proto::ext::application::Application;
    sapp->set_id(app->id().toStdString());
    sapp->set_name(app->displayName().toStdString());
    sapp->set_icon(app->iconUrl().name().toStdString());
    win->set_allocated_app(sapp);
  }

  res->set_allocated_get_active_window(winRes);
  winRes->set_allocated_window(win);

  return res;
}

proto::ext::wm::Response *
WindowManagementRouter::getActiveWorkspace(const proto::ext::wm::GetActiveWorkspaceRequest &req) {
  auto activeWorkspace = m_wm.provider()->getActiveWorkspace();

  if (!activeWorkspace) {
    throw std::runtime_error("No active workspace. The current window manager might not support workspaces.");
  }

  auto res = new wm::Response;
  auto resData = new wm::GetActiveWorkspaceResponse;
  auto workspace = new wm::Workspace;

  res->set_allocated_get_active_workspace(resData);
  resData->set_allocated_workspace(workspace);
  workspace->set_id(activeWorkspace->id().toStdString());
  workspace->set_active(true);
  workspace->set_name(activeWorkspace->name().toStdString());
  workspace->set_fullscreen(activeWorkspace->hasFullScreen());
  workspace->set_monitor(activeWorkspace->monitor().toStdString());

  return res;
}

void WindowManagementRouter::initWindow(AbstractWindowManager::AbstractWindow &win,
                                        proto::ext::wm::Window &obj) {
  obj.set_id(win.id().toStdString());
  obj.set_workspace_id(win.workspace()->toStdString());
  obj.set_fullscreen(win.fullScreen());

  if (auto bounds = win.bounds()) {
    obj.set_x(bounds->x);
    obj.set_y(bounds->y);
    obj.set_width(bounds->width);
    obj.set_height(bounds->height);
  }
}

void WindowManagementRouter::initWorkspace(AbstractWindowManager::AbstractWorkspace &workspace,
                                           proto::ext::wm::Workspace &obj) {
  obj.set_id(workspace.id().toStdString());
  obj.set_name(workspace.name().toStdString());
  obj.set_fullscreen(workspace.hasFullScreen());
  obj.set_monitor(workspace.monitor().toStdString());
}

proto::ext::wm::Window *WindowManagementRouter::serializeWindow(AbstractWindowManager::AbstractWindow &win) {
  auto serialized = new wm::Window;

  initWindow(win, *serialized);

  return serialized;
}

proto::ext::wm::Response *WindowManagementRouter::ping(const proto::ext::wm::PingRequest &req) {
  auto res = new wm::Response;
  auto pingRes = new wm::PingResponse;

  pingRes->set_ok(m_wm.provider()->ping());

  return res;
}

proto::ext::extension::Response *WindowManagementRouter::wrapResponse(proto::ext::wm::Response *wmRes) {
  auto res = new proto::ext::extension::Response;
  auto data = new proto::ext::extension::ResponseData;

  data->set_allocated_wm(wmRes);
  res->set_allocated_data(data);
  return res;
}

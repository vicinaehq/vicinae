#pragma once
#include "proto/extension.pb.h"
#include "proto/wm.pb.h"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "types.hpp"
#include <qobject.h>

class WindowManagementRouter : public QObject {
public:
  PromiseLike<proto::ext::extension::Response *> route(const proto::ext::wm::Request &req);
  WindowManagementRouter(WindowManager &wm, AppService &app);

private:
  proto::ext::wm::Response *getActiveWindow(const proto::ext::wm::GetActiveWindowRequest &req);
  proto::ext::wm::Response *getActiveWorkspace(const proto::ext::wm::GetActiveWorkspaceRequest &req);
  proto::ext::wm::Response *getWindows(const proto::ext::wm::GetWindowsRequest &req);
  proto::ext::wm::Response *getScreens(const proto::ext::wm::GetScreensRequest &req);
  proto::ext::wm::Response *getWorkspaces(const proto::ext::wm::GetWorkspacesRequest &req);
  proto::ext::wm::Response *ping(const proto::ext::wm::PingRequest &req);
  proto::ext::wm::Response *focusWindow(const proto::ext::wm::FocusWindowRequest &req);

  static proto::ext::extension::Response *wrapResponse(proto::ext::wm::Response *wmRes);
  proto::ext::wm::Window *serializeWindow(AbstractWindowManager::AbstractWindow &win);
  void initWindow(AbstractWindowManager::AbstractWindow &win, proto::ext::wm::Window &obj);

  void initWorkspace(AbstractWindowManager::AbstractWorkspace &win, proto::ext::wm::Workspace &obj);

  WindowManager &m_wm;
  AppService &m_app;
};

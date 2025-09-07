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
  proto::ext::wm::Response *ping(const proto::ext::wm::PingRequest &req);

  static proto::ext::extension::Response *wrapResponse(proto::ext::wm::Response *wmRes);
  static proto::ext::wm::Window *serializeWindow(AbstractWindowManager::AbstractWindow &win);

  WindowManager &m_wm;
  AppService &m_app;
};

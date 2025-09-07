#include "wm-router.hpp"
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
  case wm::Request::kGetActiveWorkspaceWindows:
    // TODO: implement
    break;
  case wm::Request::kSetWindowBounds:
    // TODO: implement
    break;
  default:
    break;
  }

  return nullptr;
}

proto::ext::wm::Response *
WindowManagementRouter::getActiveWindow(const proto::ext::wm::GetActiveWindowRequest &req) {
  auto activeWindow = m_wm.getFocusedWindow();

  if (!activeWindow) { throw std::runtime_error("No active window"); }

  auto res = new wm::Response;
  auto winRes = new wm::GetActiveWindowResponse;

  res->set_allocated_get_active_window(winRes);
  winRes->set_allocated_window(serializeWindow(*activeWindow));

  return res;
}

proto::ext::wm::Window *WindowManagementRouter::serializeWindow(AbstractWindowManager::AbstractWindow &win) {
  auto serialized = new wm::Window;

  serialized->set_id(win.id().toStdString());
  serialized->set_workspace_id(std::to_string(win.workspace().value_or(-1)));
  serialized->set_active(false);

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

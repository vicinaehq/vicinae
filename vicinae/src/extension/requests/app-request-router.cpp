#include "app-request-router.hpp"
#include "proto/application.pb.h"

proto::ext::application::Response *
AppRequestRouter::listApplications(const proto::ext::application::ListApplicationRequest &req) const {
  auto resData = new proto::ext::application::ListApplicationResponse;
  auto res = new proto::ext::application::Response;

  auto list = req.has_target() ? m_appDb.findOpeners(req.target().c_str()) : m_appDb.list();

  for (const auto &app : list) {
    auto protoApp = resData->add_apps();
    *protoApp = app->toProto();
  }

  res->set_allocated_list(resData);
  return res;
}

proto::ext::application::Response *
AppRequestRouter::getDefault(const proto::ext::application::GetDefaultApplicationRequest &req) const {
  auto res = new proto::ext::application::Response;
  auto resData = new proto::ext::application::GetDefaultApplicationResponse;

  res->set_allocated_get_default(resData);

  if (auto opener = m_appDb.findBestOpener(req.target().c_str())) {
    auto app = new proto::ext::application::Application(opener->toProto());
    resData->set_allocated_app(app);
  }

  return res;
}

proto::ext::application::Response *
AppRequestRouter::openApplication(const proto::ext::application::OpenApplicationRequest &req) const {
  QString target = req.target().c_str();

  if (req.has_app_id()) {
    if (auto app = m_appDb.findById(req.app_id().c_str())) {
      m_appDb.launch(*app, {target});
      return nullptr;
    }
  }

  if (auto opener = m_appDb.findBestOpener(target)) { m_appDb.launch(*opener, {target}); }

  return nullptr;
}

proto::ext::extension::Response *AppRequestRouter::route(const proto::ext::application::Request &req) {
  namespace app = proto::ext::application;

  auto wrap = [](app::Response *appRes) -> proto::ext::extension::Response * {
    auto res = new proto::ext::extension::Response;
    auto data = new proto::ext::extension::ResponseData;

    data->set_allocated_app(appRes);
    res->set_allocated_data(data);
    return res;
  };

  switch (req.payload_case()) {
  case app::Request::kList:
    return wrap(listApplications(req.list()));
  case app::Request::kOpen:
    return wrap(openApplication(req.open()));
  case app::Request::kGetDefault:
    return wrap(getDefault(req.get_default()));
  default:
    break;
  }

  return nullptr;
}

AppRequestRouter::AppRequestRouter(AppService &appDb) : m_appDb(appDb) {}

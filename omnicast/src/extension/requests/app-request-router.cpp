#include "app-request-router.hpp"
#include "proto/application.pb.h"

proto::ext::application::Response *
AppRequestRouter::listApplications(const proto::ext::application::ListApplicationRequest &req) const {
  auto resData = new proto::ext::application::ListApplicationResponse;
  auto res = new proto::ext::application::Response;

  for (const auto &app : m_appDb.list()) {
    auto protoApp = resData->add_apps();

    protoApp->set_id(app->id().toStdString());
    protoApp->set_name(app->name().toStdString());
    protoApp->set_icon(app->iconUrl().name().toStdString());
  }

  res->set_allocated_list(resData);
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
  default:
    break;
  }

  return nullptr;
}

AppRequestRouter::AppRequestRouter(AppService &appDb) : m_appDb(appDb) {}

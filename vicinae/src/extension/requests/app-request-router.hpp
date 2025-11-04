#pragma once
#include "proto/application.pb.h"
#include "proto/extension.pb.h"
#include "proto/ipc.pb.h"
#include "services/app-service/app-service.hpp"

class AppRequestRouter {
  AppService &m_appDb;

  proto::ext::application::Response *
  listApplications(const proto::ext::application::ListApplicationRequest &) const;

  proto::ext::application::Response *
  getDefault(const proto::ext::application::GetDefaultApplicationRequest &) const;

  proto::ext::application::Response *
  openApplication(const proto::ext::application::OpenApplicationRequest &) const;

  proto::ext::application::Response *
  runInTerminal(const proto::ext::application::RunInTerminalRequest &) const;

public:
  proto::ext::extension::Response *route(const proto::ext::application::Request &);

  AppRequestRouter(AppService &appDb);
};

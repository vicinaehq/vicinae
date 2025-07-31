#pragma once
#include "common.hpp"
#include "ipc-command-server.hpp"
#include "proto/daemon.pb.h"

class IpcCommandHandler : public ICommandHandler {

public:
  proto::ext::daemon::Response *handleCommand(const proto::ext::daemon::Request &message) override;

  IpcCommandHandler(ApplicationContext &ctx);

private:
  ApplicationContext &m_ctx;
};

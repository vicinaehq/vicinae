#pragma once
#include "common.hpp"
#include "expected.hpp"
#include "ipc-command-server.hpp"
#include "proto/daemon.pb.h"

class IpcCommandHandler : public ICommandHandler {

public:
  PromiseLike<proto::ext::daemon::Response *>
  handleCommand(const proto::ext::daemon::Request &message) override;
  tl::expected<void, std::string> handleUrl(const QUrl &url);

  QFuture<proto::ext::daemon::Response *> processDmenu(const proto::ext::daemon::DmenuRequest &request);

  IpcCommandHandler(ApplicationContext &ctx);

private:
  ApplicationContext &m_ctx;
};

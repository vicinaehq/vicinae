#pragma once
#include "common.hpp"
#include "ipc-command-server.hpp"

class IpcCommandHandler : public ICommandHandler {

public:
  std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) override;

  IpcCommandHandler(ApplicationContext &ctx);

private:
  ApplicationContext &m_ctx;
};

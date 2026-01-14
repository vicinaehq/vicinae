#pragma once
#include "common.hpp"
#include <expected>

class IpcCommandHandler {
public:
  std::expected<void, std::string> handleUrl(const QUrl &url);
  IpcCommandHandler(ApplicationContext &ctx);

private:
  ApplicationContext &m_ctx;
};

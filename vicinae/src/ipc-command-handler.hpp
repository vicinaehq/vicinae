#pragma once
#include "common/context.hpp"
#include <expected>
#include <QUrl>

class IpcCommandHandler {
public:
  std::expected<void, std::string> handleUrl(const QUrl &url);
  IpcCommandHandler(ApplicationContext &ctx);

private:
  ApplicationContext &m_ctx;
};

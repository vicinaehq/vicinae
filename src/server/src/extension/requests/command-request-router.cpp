#include "command-request-router.hpp"
#include "proto/command.pb.h"
#include "proto/extension.pb.h"

using Request = proto::ext::command::Request;

proto::ext::extension::Response *CommandRequestRouter::wrap(proto::ext::command::Response *cmdRes) {
  auto res = new proto::ext::extension::Response;
  auto data = new proto::ext::extension::ResponseData;
  data->set_allocated_command(cmdRes);
  res->set_allocated_data(data);
  return res;
}

proto::ext::extension::Response *CommandRequestRouter::route(const Request &req) {
  switch (req.payload_case()) {
  case Request::kUpdateCommandMetadata: {
    auto res = new proto::ext::command::Response;
    auto ack = new proto::ext::common::AckResponse;

    const auto &meta = req.update_command_metadata();

    // Set or clear subtitle override
    if (meta.has_subtitle() && !QString::fromStdString(meta.subtitle()).isEmpty()) {
      m_navigation->setSubtitleOverride(QString::fromStdString(meta.subtitle()));
    } else {
      m_navigation->setSubtitleOverride(std::nullopt);
    }

    // Refresh view to display the new subtitle
    if (m_rootManager) emit m_rootManager->itemsChanged();

    res->set_allocated_update_command_metadata(ack);
    return wrap(res);
  }
  default:
    break;
  }

  return nullptr;
}

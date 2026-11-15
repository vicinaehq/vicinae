#pragma once

#include <QByteArray>
#include <optional>
#include <string>

namespace AI {

struct ResourcePart {
  enum class Status { Available, Unavailable, Unsupported };

  std::string id;
  std::optional<std::string> name;
  std::optional<std::string> mimeType;
  std::optional<std::string> uri;
  std::optional<QByteArray> data;
  std::optional<std::string> path;
  Status status = Status::Available;
};

} // namespace AI

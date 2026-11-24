#pragma once

#include <QByteArray>
#include <format>
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

  std::string describe() const {
    const auto &location = path ? path : uri;
    return std::format("(attachment: {}{})", name.value_or("attachment"),
                       location ? std::format(" — {}", *location) : std::string{});
  }
};

} // namespace AI

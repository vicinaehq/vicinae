#pragma once

#include <QByteArray>
#include <optional>
#include <string>

namespace AI {

struct ImagePart {
  std::string mimeType;
  QByteArray base64;
  std::optional<std::string> path;
};

} // namespace AI

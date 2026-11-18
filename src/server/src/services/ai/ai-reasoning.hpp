#pragma once

#include <optional>
#include <string>
#include <vector>

namespace AI {

struct ReasoningChoice {
  std::string id;
  std::string name;
  bool operator==(const ReasoningChoice &) const = default;
};

struct ReasoningOptions {
  std::string id;
  std::vector<ReasoningChoice> choices;
  std::optional<std::string> current;
  bool operator==(const ReasoningOptions &) const = default;
};

} // namespace AI

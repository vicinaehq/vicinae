#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <QString>
#include "tool-policy.hpp"
#include "ui/image/url.hpp"

namespace AI {

class AbstractTool;

struct ToolAnnotations {
  std::optional<bool> readOnly;
  std::optional<bool> destructive;
  std::optional<bool> idempotent;
  std::optional<bool> openWorld;
};

struct ToolContribution {
  std::string id;
  QString title;
  QString description;
  ImageURL icon;
  ToolPolicy defaultPolicy = ToolPolicy::Ask;
  // Deferred tools are only sent to the model once it loads them with load_tools.
  bool deferred = true;
  // Shown in the extension's tool settings. Off for tools whose availability is managed elsewhere.
  bool listed = true;
  bool mcp = true;
  ToolAnnotations annotations;
  std::function<std::unique_ptr<AbstractTool>()> create;
};

} // namespace AI

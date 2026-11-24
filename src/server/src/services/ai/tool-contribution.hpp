#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <QString>
#include "ai-capability.hpp"
#include "tool-policy.hpp"
#include "command/preference.hpp"
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
  PreferenceList preferences;
  ToolPolicy defaultPolicy = ToolPolicy::Ask;
  // Deferred tools are only sent to the model once it loads them with load_tools.
  bool deferred = true;
  bool mcp = true;
  Capabilities requiredCapabilities = 0;
  ToolAnnotations annotations;
  std::function<std::unique_ptr<AbstractTool>()> create;
};

} // namespace AI

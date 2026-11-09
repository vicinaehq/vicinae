#pragma once

#include <functional>
#include <memory>
#include <string>
#include <QString>
#include "tool-policy.hpp"
#include "ui/image/url.hpp"

namespace AI {

class AbstractTool;

struct ToolContribution {
  std::string id;
  QString title;
  QString description;
  ImageURL icon;
  ToolPolicy defaultPolicy = ToolPolicy::Ask;
  // Deferred tools are only sent to the model once it loads them with load_tools.
  bool deferred = true;
  std::function<std::unique_ptr<AbstractTool>()> create;
};

} // namespace AI

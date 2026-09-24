#pragma once

#include <functional>
#include <memory>
#include <string>
#include <QString>
#include "ui/image/url.hpp"

namespace AI {

class AbstractTool;

struct ToolContribution {
  std::string id;
  QString title;
  QString description;
  ImageURL icon;
  std::function<std::unique_ptr<AbstractTool>()> create;
};

} // namespace AI

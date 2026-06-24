#pragma once

#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include <optional>
#include <string>

struct EmptyViewModel {
  std::string title;
  std::string description;
  std::optional<ImageLikeModel> icon;
  std::optional<ActionPannelModel> actions;
};

#pragma once

#include "extension/model/action-model.hpp"
#include "extension/model/image-model.hpp"
#include <optional>
#include <string>

struct EmptyViewModel {
  std::string title;
  std::string description;
  std::optional<ImageLikeModel> icon;
  std::optional<ActionPannelModel> actions;
};

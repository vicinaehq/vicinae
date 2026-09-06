#pragma once
#include "extension/model/color-model.hpp"
#include "extension/model/image-model.hpp"
#include <optional>
#include <string>
#include <vector>

struct TagItemModel {
  std::string text;
  std::optional<ImageLikeModel> icon;
  std::optional<ColorLike> color;
  std::string onAction;
};

struct TagListModel {
  std::string title;
  std::vector<TagItemModel> items;
};

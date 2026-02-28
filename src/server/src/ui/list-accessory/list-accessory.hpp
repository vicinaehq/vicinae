#pragma once
#include "../image/url.hpp"
#include "theme.hpp"
#include <optional>
#include <qcontainerfwd.h>

struct ListAccessory {
  QString text;
  std::optional<ColorLike> color;
  QString tooltip;
  bool fillBackground;
  std::optional<ImageURL> icon;
};

using AccessoryList = std::vector<ListAccessory>;

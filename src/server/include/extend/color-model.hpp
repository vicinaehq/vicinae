#pragma once
#include "theme.hpp"
#include <glaze/json/generic.hpp>

class ColorLikeModelParser {
public:
  ColorLike parse(const glz::generic &colorLike);
};

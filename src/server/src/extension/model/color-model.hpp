#pragma once
#include "theme/theme.hpp"
#include <qstring.h>

struct ColorStringModel {
  QString colorString;
};

struct ThemeColorModel {
  QString themeColor;
};

using ColorLikeModel = QString;

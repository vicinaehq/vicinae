#pragma once
#include "theme.hpp"
#include <qjsonobject.h>
#include <qstring.h>

struct ColorStringModel {
  QString colorString;
};

struct ThemeColorModel {
  QString themeColor;
};

using ColorLikeModel = QString;

class ColorLikeModelParser {
public:
  ColorLikeModelParser();

  ColorLike parse(const QJsonValue &colorLike);
};

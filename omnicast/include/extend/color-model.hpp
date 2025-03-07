#pragma once
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

  ColorLikeModel parse(const QJsonObject &colorLike);
};

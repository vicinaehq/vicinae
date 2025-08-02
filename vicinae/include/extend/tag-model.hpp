#pragma once
#include "extend/color-model.hpp"
#include "extend/image-model.hpp"
#include <qjsonobject.h>
#include <qstring.h>

struct TagItemModel {
  QString text;
  std::optional<ImageLikeModel> icon;
  std::optional<ColorLike> color;
  QString onAction;
};

struct TagListModel {
  QString title;
  QList<TagItemModel> items;
};

class TagListParser {
  TagItemModel parseTagItem(const QJsonObject &instance);

public:
  TagListParser();

  TagListModel parse(const QJsonObject &instance);
};

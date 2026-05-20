#pragma once
#include "extend/color-model.hpp"
#include "extend/image-model.hpp"
#include "lib/fuzzy/fuzzy-searchable.hpp"
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

struct TagPickerItemModel {
  QString title;
  QString value;
  std::optional<ImageLikeModel> icon;

public:
  static TagPickerItemModel fromJson(const QJsonObject &instance);
};

template <> struct fuzzy::FuzzySearchable<TagPickerItemModel> {
  static int score(const TagPickerItemModel &item, std::string_view query) {
    auto name = item.title.toStdString();
    return fuzzy::scoreWeighted({{name, 1.0}}, query);
  }
};

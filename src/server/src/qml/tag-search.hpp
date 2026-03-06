#pragma once
#include "extend/tag-model.hpp"
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <string_view>
#include <vector>

class TagSearch : public QObject {
  Q_OBJECT

public:
  explicit TagSearch(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE QVariantList search(const QString &query, const QVariantList &items) const {
    if (items.isEmpty()) { return {}; }

    std::vector<TagPickerItemModel> tagItems;
    tagItems.reserve(static_cast<size_t>(items.size()));

    for (const auto &item : items) {
      const QVariantMap &map = item.toMap();
      TagPickerItemModel model;
      model.title = map.value("displayName").toString();
      model.value = map.value("id").toString();
      tagItems.push_back(std::move(model));
    }

    std::vector<Scored<int>> filtered;
    auto queryUtf8 = query.toUtf8();
    fuzzy::fuzzyFilter<TagPickerItemModel>(std::span<const TagPickerItemModel>(tagItems),
                                           std::string_view(queryUtf8.constData()), filtered);

    QVariantList result;
    result.reserve(static_cast<int>(filtered.size()));
    for (const auto &scored : filtered) {
      const auto &item = tagItems[scored.data];
      QVariantMap map;
      map.insert("id", item.value);
      map.insert("displayName", item.title);
      const QVariantMap &orig = items.at(scored.data).toMap();
      if (orig.contains("iconSource")) { map.insert("iconSource", orig.value("iconSource")); }
      result.append(map);
    }

    return result;
  }
};

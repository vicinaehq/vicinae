#pragma once
#include <optional>
#include <qjsonobject.h>
#include <qstring.h>

struct PaginationModel {
  std::optional<QString> onLoadMore;
  bool hasMore;
  size_t pageSize;

  static PaginationModel fromJson(const QJsonObject &obj) {
    PaginationModel model;

    model.hasMore = obj.value("hasMore").toBool(false);
    model.pageSize = obj.value("pageSize").toInt();

    if (obj.contains("onLoadMore")) { model.onLoadMore = obj.value("onLoadMore").toString(); }

    return model;
  }
};

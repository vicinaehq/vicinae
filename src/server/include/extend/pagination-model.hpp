#pragma once
#include "extend/node-props.hpp"
#include <optional>
#include <qstring.h>

struct PaginationModel {
  std::optional<QString> onLoadMore;
  bool hasMore;
  size_t pageSize;

  static PaginationModel fromProps(const glz::generic::object_t &obj) {
    PaginationModel model;

    model.hasMore = node_props::getBool(obj, "hasMore");
    model.pageSize = static_cast<size_t>(node_props::getInt(obj, "pageSize"));

    if (auto *v = node_props::get(obj, "onLoadMore"); v && v->is_string()) {
      model.onLoadMore = QString::fromStdString(v->get_string());
    }

    return model;
  }
};

#pragma once
#include <optional>
#include "extend/model.hpp"

struct PaginationModel {
  std::optional<EventHandler> onLoadMore;
  bool hasMore = false;
};

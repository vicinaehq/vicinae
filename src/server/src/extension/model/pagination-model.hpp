#pragma once
#include "extension/model/model.hpp"

struct PaginationModel {
  EventHandler onLoadMore;
  bool hasMore = false;
};

#pragma once
#include "extend/model.hpp"

struct PaginationModel {
  EventHandler onLoadMore;
  bool hasMore = false;
};

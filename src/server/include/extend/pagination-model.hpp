#pragma once
#include <cstddef>
#include <optional>
#include <string>

struct PaginationModel {
  std::optional<std::string> onLoadMore;
  bool hasMore = false;
  size_t pageSize = 0;
};

#pragma once
#include <vector>

template <typename T> struct PaginatedResponse {
  int totalCount = 0;
  int currentPage = 0;
  int totalPages = 0;
  std::vector<T> data;
};

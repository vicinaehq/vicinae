#pragma once
#include <algorithm>
#include <span>
#include <string_view>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "fuzzy/fzf.hpp"
#include "fuzzy/scored.hpp"

namespace fuzzy::test {

inline std::vector<std::string_view> rankByQuery(std::span<const std::string_view> items,
                                                 std::string_view query) {
  const auto &matcher = fzf::threadLocalMatcher();

  std::vector<Scored<std::string_view>> scored;
  scored.reserve(items.size());

  for (auto text : items) {
    int s = matcher.fuzzy_match_v2_score_query(text, query);
    if (s > 0) { scored.push_back({.data = text, .score = s}); }
  }

  std::ranges::stable_sort(scored, std::greater{});

  std::vector<std::string_view> out;
  out.reserve(scored.size());
  for (const auto &entry : scored) {
    out.push_back(entry.data);
  }
  return out;
}

inline void expectRankedOrder(std::initializer_list<std::string_view> items, std::string_view query) {
  INFO("query: \"" << query << "\"");
  std::vector<std::string_view> expected{items};
  auto actual = rankByQuery(std::span<const std::string_view>{expected}, query);
  REQUIRE(actual == expected);
}

} // namespace fuzzy::test

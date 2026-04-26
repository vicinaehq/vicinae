#pragma once
#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>
#include "fuzzy/scored.hpp"
#include "fuzzy/fzf.hpp"

namespace fuzzy {

template <typename T> struct FuzzySearchable;

template <typename T>
concept FuzzySearchableType = requires(const T &item, std::string_view query) {
  { FuzzySearchable<T>::score(item, query) } -> std::convertible_to<int>;
};

struct WeightedField {
  std::string_view text;
  double weight;
};

namespace detail {
template <std::ranges::forward_range R>
  requires std::same_as<std::ranges::range_value_t<R>, WeightedField>
inline int scoreFields(R &&fields, std::string_view query) {
  if (query.empty()) return 0;

  auto ws = fields | std::views::transform([](const WeightedField &f) {
              return fzf::WeightedString{f.text, static_cast<float>(f.weight)};
            });

  return fzf::threadLocalMatcher().fuzzy_match_v2_score_query(ws, query);
}
} // namespace detail

inline int scoreWeighted(std::initializer_list<WeightedField> fields, std::string_view query) {
  return detail::scoreFields(fields, query);
}

template <std::ranges::forward_range R>
  requires std::same_as<std::ranges::range_value_t<R>, WeightedField>
inline int scoreWeighted(R &&fields, std::string_view query) {
  return detail::scoreFields(std::forward<R>(fields), query);
}

template <FuzzySearchableType T>
void fuzzyFilter(std::span<const T> items, std::string_view query, std::vector<Scored<int>> &out) {
  out.clear();
  out.reserve(items.size());

  if (query.empty()) {
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
      out.push_back({.data = i, .score = 0});
    }
    return;
  }

  for (int i = 0; i < static_cast<int>(items.size()); ++i) {
    int s = FuzzySearchable<T>::score(items[i], query);
    if (s > 0) { out.push_back({.data = i, .score = s}); }
  }

  std::ranges::stable_sort(out, std::greater{});
}

} // namespace fuzzy

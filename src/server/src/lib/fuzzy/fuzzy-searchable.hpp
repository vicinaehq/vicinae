#pragma once
#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "common/scored.hpp"
#include "lib/fts_fuzzy.hpp"

namespace fuzzy {

/// External trait — specialize for each searchable type.
template <typename T> struct FuzzySearchable;

template <typename T>
concept FuzzySearchableType = requires(const T &item, std::string_view query) {
  { FuzzySearchable<T>::score(item, query) } -> std::convertible_to<int>;
};

struct WeightedField {
  std::string_view text;
  double weight;
};

/// Score a query against multiple weighted fields, returning the best weighted score.
inline int scoreWeighted(std::initializer_list<WeightedField> fields, std::string_view query) {
  int best = 0;
  for (const auto &f : fields) {
    int s = 0;
    if (fts::fuzzy_match(query, f.text, s)) {
      int weighted = static_cast<int>(s * f.weight);
      if (weighted > best) best = weighted;
    }
  }
  return best;
}

/// Filter items by fuzzy score, storing indices into `out`.
/// When query is empty, all indices are included (score 0, original order).
/// When non-empty, only items with score > 0 are included, stable-sorted descending.
/// Reuses `out`'s allocation across calls.
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
    if (s > 0) {
      out.push_back({.data = i, .score = s});
    }
  }

  std::stable_sort(out.begin(), out.end(), std::greater{});
}

} // namespace fuzzy

#pragma once
#include <algorithm>
#include <functional>
#include <ranges>
#include <vector>
#include "fuzzy/scored.hpp"

namespace fuzzy {

template <typename T> class FuzzyScorer {
public:
  using OutType = Scored<T>;
  using ScorableProj = std::function<int(std::string_view query, const T &item)>;

  void score(const std::ranges::sized_range auto &in, std::string_view pattern, const ScorableProj &proj,
             std::vector<OutType> &items) {
    auto withScore = [&](auto &&el) -> OutType { return OutType{.data = el, .score = proj(pattern, el)}; };
    auto filtered = in | std::views::transform(withScore) |
                    std::views::filter([&](auto &&out) { return out.score > 0 || pattern.empty(); });

    items.reserve(in.size());
    items.clear();
    std::ranges::copy(filtered, std::back_inserter(items));
    std::ranges::stable_sort(items, std::greater{});
  }
};

}; // namespace fuzzy

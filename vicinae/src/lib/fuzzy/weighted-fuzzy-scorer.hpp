#pragma once
#include <algorithm>
#include <ranges>
#include <vector>
#include <functional>
#include "common/scored.hpp"
#include "lib/fts_fuzzy.hpp"

namespace fuzzy {
/**
 * Simple utility class that takes a list of strings with an optional weight assigned to them,
 * then fuzzy matches a pattern against them. The best score is returned.
 */
class WeightedScorer {
public:
  void reserve(size_t n) { m_strings.reserve(n); }
  void add(const std::string &str, double weight = 1) { m_strings.emplace_back(std::pair{str, weight}); }

  int score(const std::string &pattern) const {
    auto scores = m_strings | std::views::transform([&](const WeightedString &str) {
                    int score = 0;
                    fts::fuzzy_match(pattern, str.first, score);
                    return static_cast<int>(score * str.second);
                  });

    return *std::ranges::max_element(scores);
  }

private:
  using WeightedString = std::pair<std::string, double>;
  std::vector<WeightedString> m_strings;
};

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

#pragma once
#include <algorithm>
#include <ranges>
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
                    fts::fuzzy_match(pattern.c_str(), str.first, score);
                    return static_cast<int>(score * str.second);
                  });

    return *std::ranges::max_element(scores);
  }

private:
  using WeightedString = std::pair<std::string, double>;
  std::vector<WeightedString> m_strings;
};
}; // namespace fuzzy

#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>
#include "fuzzy/fzf.hpp"
#include "scored.hpp"

namespace fuzzy {

inline constexpr int MIN_QUALITY = 60;
// score points (out of 100) a maximal frecency is worth
inline constexpr double FRECENCY_WEIGHT = 6.0;

// both in [0, 100]: `score` is field-weighted (ranking), `quality` is the unweighted quality of the
// worst matched query word (filtering). See fzf::QueryScore.
struct Match {
  int score = 0;
  int quality = 0;

  bool accepted() const { return quality >= MIN_QUALITY; }
};

// [0, 1]; timestamps are unix seconds
inline double frecency(std::uint32_t visitCount, std::optional<std::uint64_t> lastVisitedAt, std::int64_t now) {
  constexpr double FREQUENCY_SCALE = 5.0;
  constexpr double RECENCY_PEAK = 10.0;
  constexpr double RECENCY_HALF_LIFE_DAYS = 30.0;
  constexpr double CAP = 25.0;
  constexpr double SECONDS_PER_DAY = 86400.0;

  double const frequency = FREQUENCY_SCALE * std::log(1 + visitCount * 0.1);
  double recency = 0.0;

  if (lastVisitedAt) {
    double const daysSince = (now - static_cast<std::int64_t>(*lastVisitedAt)) / SECONDS_PER_DAY;
    recency = RECENCY_PEAK * std::exp(-std::max(0.0, daysSince) / RECENCY_HALF_LIFE_DAYS);
  }

  return std::min(CAP, frequency + recency) / CAP;
}

template <typename T> struct FuzzySearchable;

template <typename T>
concept FuzzySearchableType = requires(const T &item, std::string_view query) {
  { FuzzySearchable<T>::score(item, query) } -> std::convertible_to<Match>;
};

struct WeightedField {
  std::string_view text;
  double weight;
};

struct OwnedWeightedField {
  std::string text;
  double weight;
};

namespace detail {
template <std::ranges::forward_range R>
  requires std::same_as<std::ranges::range_value_t<R>, WeightedField>
inline Match scoreFields(R &&fields, std::string_view query) {
  if (query.empty()) return {};

  auto ws = fields | std::views::transform([](const WeightedField &f) {
              return fzf::WeightedString{f.text, static_cast<float>(f.weight)};
            });

  auto const qs = fzf::threadLocalMatcher().score_query(ws, query);
  return {.score = qs.score, .quality = qs.quality};
}
} // namespace detail

inline Match scoreWeighted(std::initializer_list<WeightedField> fields, std::string_view query) {
  return detail::scoreFields(fields, query);
}

template <std::ranges::forward_range R>
  requires std::same_as<std::ranges::range_value_t<R>, WeightedField>
inline Match scoreWeighted(R &&fields, std::string_view query) {
  return detail::scoreFields(std::forward<R>(fields), query);
}

inline Match scoreWeighted(const std::vector<OwnedWeightedField> &fields, std::string_view query) {
  auto view = fields | std::views::transform(
                           [](const OwnedWeightedField &f) { return WeightedField{f.text, f.weight}; });
  return detail::scoreFields(view, query);
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
    Match const m = FuzzySearchable<T>::score(items[i], query);
    if (m.accepted()) { out.push_back({.data = i, .score = m.score}); }
  }

  std::ranges::stable_sort(out, std::greater{});
}

} // namespace fuzzy

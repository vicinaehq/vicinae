#include "common/enumerate.hpp"
#include "file-indexer/file-indexer-query-engine.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <format>
#include <ranges>

namespace file_indexer::query {

namespace {

constexpr double RANK_LOG_WEIGHT = 10.0;
constexpr int64_t TRUSTED_WORD_MIN_RANK = 3;

} // namespace

std::vector<std::string_view> splitQueryWords(std::string_view query) {
  static constexpr std::string_view WHITESPACE = " \t\n\r\f\v";

  auto trim = [](std::string_view word) -> std::string_view {
    auto const begin = word.find_first_not_of(WHITESPACE);
    if (begin == std::string_view::npos) return {};
    return word.substr(begin, word.find_last_not_of(WHITESPACE) - begin + 1);
  };

  return query | std::views::split(std::string_view{" "}) |
         std::views::transform([&](auto &&v) { return trim(std::string_view{v}); }) |
         std::views::filter([](std::string_view word) { return !word.empty(); }) |
         std::ranges::to<std::vector>();
}

std::string prepareCandidateSearchQuery(std::string_view query) {
  auto words = splitQueryWords(query);
  auto enquote = [](auto &&w) { return std::format("\"{}\"", w); };

  return words | std::views::transform(enquote) | std::views::join_with(std::string_view{" "}) |
         std::ranges::to<std::string>();
}

std::string prepareCorrectionSearchQuery(const CorrectionPlan &plan) {
  std::vector<std::string> parts;

  parts.reserve(plan.choices.size());

  for (const auto &choice : plan.choices) {
    const auto &word = choice.term;

    if (word.size() <= 2) continue;

    parts.emplace_back(std::format("\"{}\"", word));
  }

  return parts | std::views::join_with(std::string_view{" "}) | std::ranges::to<std::string>();
}

double adjustedSuggestionScore(const FileIndexerDatabase::SpellfixSuggestion &suggestion) {
  return suggestion.score - RANK_LOG_WEIGHT * std::log2(1.0 + static_cast<double>(suggestion.rank));
}

double correctionWeight(int distance) {
  double const closeness = 1.0 - 0.5 * (static_cast<double>(distance) / MAX_CORRECTION_DISTANCE);
  return CORRECTION_PENALTY * closeness;
}

std::vector<FileIndexerDatabase::SpellfixSuggestion>
pickCorrections(std::span<const FileIndexerDatabase::SpellfixSuggestion> suggestions,
                std::string_view original, size_t maxCount, bool trustKnownWords) {
  std::string lowered{original};
  std::ranges::transform(lowered, lowered.begin(),
                         [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  auto stem = [](std::string_view word) {
    auto const end = word.find_last_not_of("0123456789");
    if (end == std::string_view::npos) return word;
    return word.substr(0, end + 1);
  };

  auto sameFamily = [&](std::string_view a, std::string_view b) {
    return a.starts_with(b) || b.starts_with(a) || stem(a) == stem(b);
  };

  bool const knownWord = trustKnownWords && std::ranges::any_of(suggestions, [&](const auto &suggestion) {
                           return suggestion.distance == 0 && suggestion.word == lowered &&
                                  suggestion.rank >= TRUSTED_WORD_MIN_RANK;
                         });

  if (knownWord) return {};

  std::vector<FileIndexerDatabase::SpellfixSuggestion> picked;

  picked.reserve(maxCount);

  for (const auto &suggestion : suggestions) {
    if (suggestion.distance > MAX_CORRECTION_DISTANCE) continue;

    const auto &word = suggestion.word;

    if (word == lowered || word.starts_with(lowered)) continue;

    auto family =
        std::ranges::find_if(picked, [&](const auto &existing) { return sameFamily(existing.word, word); });

    if (family != picked.end()) {
      if (family->word.starts_with(word)) { *family = suggestion; }
      continue;
    }

    if (picked.size() < maxCount) { picked.emplace_back(suggestion); }
  }

  return picked;
}

std::vector<CorrectionPlan> buildCorrectionPlans(std::span<const QueryWord> words, size_t maxPlans) {
  std::vector<CorrectionPlan> plans;

  if (words.empty() || maxPlans == 0) return plans;

  const auto makeChoice = [](const QueryWord &word, size_t correctionIndex) {
    if (correctionIndex >= word.corrections.size()) {
      return CorrectionChoice{.original = word.word, .term = word.word};
    }

    const auto &correction = word.corrections[correctionIndex];
    return CorrectionChoice{.original = word.word,
                            .term = correction.word,
                            .weight = correctionWeight(correction.distance),
                            .corrected = true};
  };

  const auto hasCorrection = [](const CorrectionPlan &plan) {
    return std::ranges::any_of(plan.choices, &CorrectionChoice::corrected);
  };

  const auto planKey = [](const CorrectionPlan &plan) {
    return plan.choices | std::views::transform([](const CorrectionChoice &choice) {
             return std::string_view{choice.term};
           }) |
           std::views::join_with(std::string_view{"\n"}) | std::ranges::to<std::string>();
  };

  const auto addPlan = [&](CorrectionPlan plan) {
    if (!hasCorrection(plan)) return;

    const auto key = planKey(plan);
    bool const duplicate =
        std::ranges::any_of(plans, [&](const CorrectionPlan &existing) { return planKey(existing) == key; });

    if (!duplicate && plans.size() < maxPlans) { plans.emplace_back(std::move(plan)); }
  };

  CorrectionPlan bestPlan;

  bestPlan.choices.reserve(words.size());

  for (const auto &word : words) {
    bestPlan.choices.emplace_back(makeChoice(word, 0));
  }

  addPlan(bestPlan);

  for (const auto &[wordIndex, word] : vicinae::enumerate(words)) {
    for (size_t correctionIndex = 1; correctionIndex < word.corrections.size(); ++correctionIndex) {
      if (plans.size() >= maxPlans) return plans;

      CorrectionPlan plan = bestPlan;
      plan.choices[static_cast<size_t>(wordIndex)] = makeChoice(word, correctionIndex);
      addPlan(std::move(plan));
    }
  }

  return plans;
}

} // namespace file_indexer::query

#pragma once
#include <array>
#include <cassert>
#include <cctype>
#include <libqalculate/util.h>
#include <ranges>
#include <string_view>

namespace fuzz {
constexpr const int MAX_STR_SIZE = 255;
static constexpr const int JARO_WINKLER_MAX_PREFIX = 4;

using Data = std::array<char, 255>;

inline float jaroSimilarity(std::string_view s1, std::string_view s2) {
  assert(s1.size() <= MAX_STR_SIZE);
  assert(s2.size() <= MAX_STR_SIZE);

  std::array<char, MAX_STR_SIZE> s1Matches = {};
  std::array<char, MAX_STR_SIZE> s2Matches = {};

  auto traverse = [](std::string_view s1, std::string_view s2, Data &out) -> int {
    int maxDistance = std::max(s1.size(), s2.size()) / 2 - 1;
    int idx = 0;
    for (int i = 0; i != s1.size(); ++i) {
      char c = tolower(s1[i]);
      int maxPos = std::min(i + maxDistance, (int)s2.size() - 1);

      for (int j = std::max(0, i - maxDistance); j <= maxPos; ++j) {
        if (c == tolower(s2[j])) {
          out[idx++] = c;
          break;
        }
      }
    }

    return idx;
  };

  int s1MatchIdx = traverse(s1, s2, s1Matches);
  int s2MatchIdx = traverse(s2, s1, s2Matches);
  int matching = std::min(s1MatchIdx, s2MatchIdx);

  if (!matching) return 0;

  int outOfOrder = 0;

  // std::cout << "s1 idx" << s1MatchIdx << " s2 idx" << s2MatchIdx << std::endl;

  for (int i = 0; i != matching; ++i) {
    if (s1Matches[i] != s2Matches[i]) ++outOfOrder;
  }

  int transpositions = outOfOrder / 2;

  float similarity = 1.0 / 3 *
                     ((float)matching / s1.size() + (float)matching / s2.size() +
                      ((float)matching - transpositions) / matching);

  return similarity;
}

inline float jaroWinklerSimilarity(std::string_view s1, std::string_view s2, float p = 0.1) {
  const float similarity = jaroSimilarity(s1, s2);
  int l = 0;

  while (l < JARO_WINKLER_MAX_PREFIX && l != std::min(s1.size(), s2.size()) &&
         tolower(s1[l]) == tolower(s2[l])) {
    ++l;
  }

  return similarity + (l * p) * (1 - similarity);
}

inline float jaroWinklerLatinText(std::string_view s1, std::string_view s2) {
  using namespace std::views;

  auto recursiveSimilarity = [](std::string_view pattern, std::string_view str) {
    float advancePenalty = 0.05;
    float bestSimilarity = 0;
    for (int i = 0; i != str.size(); ++i) {
      std::string_view part(str.begin() + i, str.end());
      bestSimilarity = std::max(bestSimilarity, jaroWinklerSimilarity(pattern, part) - advancePenalty * i);
    }
    return bestSimilarity;
  };

  auto toView = [](auto &&p) { return std::string_view{p}; };
  auto chunkStr = [&](std::string_view str) {
    auto words = split(str, std::string_view{" "}) | transform([&](auto &&str) {
                   return toView(str) | chunk_by([](char a, char b) {
                            bool caseChanged = std::islower(a) && std::isupper(b);
                            return !caseChanged;
                          }) |
                          transform(toView);
                 }) |
                 join;

    return words;
  };

  auto w1 = chunkStr(s1);
  auto w2 = chunkStr(s2);
  float globalSim = 0;
  int c = 0;

  for (auto word1 : w1) {
    float bestSimilarity = 0;
    for (const auto word2 : w2) {
      bestSimilarity = std::max(bestSimilarity, jaroWinklerSimilarity(word1, word2));
    }
    globalSim += bestSimilarity;
    c++;
  }

  /*
  for (auto word2 : w2) {
    float bestSimilarity =
        std::ranges::max(w1 | transform([&](auto word1) { return jaroWinklerSimilarity(word1, word2); }));
    globalSim += bestSimilarity;
    c++;
  }
  */

  return globalSim / c;
}

inline float jaroWinklerDistance(std::string_view s1, std::string_view s2, float p = 0.1) {
  return 1 - jaroWinklerSimilarity(s1, s2, p);
}

}; // namespace fuzz

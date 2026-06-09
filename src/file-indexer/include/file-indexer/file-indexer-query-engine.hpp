#pragma once
#include "fuzzy/fzf.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include <algorithm>
#include <filesystem>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

struct IndexerFileResult {
  std::filesystem::path path;
  double rank;
};

struct Pagination {
  int offset = 0;
  int limit = 50;
};

class FileIndexerQueryEngine {
public:
  std::vector<IndexerFileResult> query(std::string_view q, const Pagination &pagination) {
    FileIndexerDatabase db;
    std::string const finalQuery = prepareCandidateSearchQuery(q);

    if (finalQuery.empty()) return {};

    auto candidates = db.searchCandidates(finalQuery, candidateLimitFor(pagination));

    return rankCandidates(std::move(candidates), q, pagination);
  }

  static std::string prepareCandidateSearchQuery(std::string_view query) {
    static constexpr std::string_view WHITESPACE = " \t\n\r\f\v";

    auto trim = [](std::string_view word) -> std::string_view {
      auto const begin = word.find_first_not_of(WHITESPACE);
      if (begin == std::string_view::npos) return {};
      return word.substr(begin, word.find_last_not_of(WHITESPACE) - begin + 1);
    };

    std::string result;
    for (const auto word : query | std::views::split(' ')) {
      auto const term = trim(std::string_view(word));
      if (term.empty()) continue;

      if (!result.empty()) result += " OR ";
      result += '"';
      for (char const c : term) {
        if (c == '"') result += '"'; // FTS5 escapes a quote by doubling it
        result += c;
      }
      result += "\"*";
    }

    return result;
  }

  static int candidateLimitFor(const Pagination &pagination) {
    static constexpr int minCandidateLimit = 250;
    static constexpr int candidateLimitMultiplier = 20;

    int const offset = std::max(0, pagination.offset);
    int const limit = std::max(0, pagination.limit);
    int const requestedResultCount = offset + limit;

    if (requestedResultCount == 0) { return 0; }

    return std::max(minCandidateLimit, requestedResultCount * candidateLimitMultiplier);
  }

  static std::vector<IndexerFileResult>
  rankCandidates(std::vector<FileIndexerDatabase::SearchCandidate> candidates, std::string_view query,
                 const Pagination &pagination) {
    struct ScoredCandidate {
      FileIndexerDatabase::SearchCandidate candidate;
      int fuzzyScore = 0;
    };

    std::vector<ScoredCandidate> scored;
    auto &matcher = fzf::threadLocalMatcher();

    scored.reserve(candidates.size());

    for (auto &candidate : candidates) {
      std::string filename = candidate.name.empty() ? candidate.path.filename().string() : candidate.name;
      int const fuzzyScore = matcher.fuzzy_match_v2_score_query(filename, query);

      if (fuzzyScore <= 0) { continue; }

      scored.emplace_back(ScoredCandidate{.candidate = std::move(candidate), .fuzzyScore = fuzzyScore});
    }

    std::ranges::stable_sort(scored, [](const ScoredCandidate &a, const ScoredCandidate &b) {
      if (a.fuzzyScore != b.fuzzyScore) { return a.fuzzyScore > b.fuzzyScore; }
      if (a.candidate.relevancyScore != b.candidate.relevancyScore) {
        return a.candidate.relevancyScore > b.candidate.relevancyScore;
      }
      if (a.candidate.indexRank != b.candidate.indexRank) {
        return a.candidate.indexRank < b.candidate.indexRank;
      }
      return a.candidate.path < b.candidate.path;
    });

    int const offset = std::max(0, pagination.offset);
    int const limit = std::max(0, pagination.limit);
    size_t const start = std::min(static_cast<size_t>(offset), scored.size());
    size_t const end = std::min(start + static_cast<size_t>(limit), scored.size());
    std::vector<IndexerFileResult> results;

    results.reserve(end - start);

    for (size_t i = start; i < end; ++i) {
      const auto &candidate = scored.at(i);

      results.emplace_back(IndexerFileResult{.path = candidate.candidate.path,
                                             .rank = static_cast<double>(candidate.fuzzyScore)});
    }

    return results;
  }
};

#pragma once
#include "fuzzy/fzf.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include <algorithm>
#include <cctype>
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

// FIXME: this fires a new query and creates a database connection on each search, which can
// potentially cause issues when doing a lot of file search. Adding a slight debounce on
// search is recommended.
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
    std::vector<std::string> terms;

    size_t start = 0;
    while (start <= query.size()) {
      size_t const end = query.find(' ', start);
      std::string_view const wordView =
          end == std::string_view::npos ? query.substr(start) : query.substr(start, end - start);

      size_t b = 0;
      size_t e = wordView.size();
      while (b < e && std::isspace(static_cast<unsigned char>(wordView[b])))
        ++b;
      while (e > b && std::isspace(static_cast<unsigned char>(wordView[e - 1])))
        --e;

      if (e > b) {
        std::string term;
        term.reserve(e - b + 4);
        term.push_back('"');
        for (size_t i = b; i < e; ++i) {
          if (wordView[i] == '"') { term.push_back('"'); } // escape by doubling
          term.push_back(wordView[i]);
        }
        term += "\"*";
        terms.emplace_back(std::move(term));
      }

      if (end == std::string_view::npos) break;
      start = end + 1;
    }

    std::string result;
    for (size_t i = 0; i < terms.size(); ++i) {
      if (i != 0) result += " OR ";
      result += terms[i];
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

#pragma once
#include "fuzzy/fzf.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "utils.hpp"
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <ranges>
#include <qstringlist.h>
#include <string>
#include <utility>
#include <vector>

// FIXME: this fires a new query and creates a database connection on each search, which can
// potentially cause issues when doing a lot of file search. Adding a slight debounce on
// search is recommended.
class FileIndexerQueryEngine {
public:
  QFuture<std::vector<IndexerFileResult>> query(std::string_view q,
                                                const AbstractFileIndexer::QueryParams &params) {
    return QtConcurrent::run([params, query = qStringFromStdView(q)]() {
      FileIndexerDatabase db;
      QString finalQuery = prepareCandidateSearchQuery(query);

      if (finalQuery.isEmpty()) return std::vector<IndexerFileResult>();

      auto candidates = db.searchCandidates(finalQuery.toStdString(), candidateLimitFor(params.pagination));

      return rankCandidates(std::move(candidates), query.toStdString(), params.pagination);
    });
  }

  static QString prepareCandidateSearchQuery(QStringView query) {
    QStringList terms;

    for (const auto &word : query.split(' ')) {
      QString term = word.toString().trimmed();

      if (term.isEmpty()) { continue; }

      term.replace('"', "\"\"");
      terms.push_back(QString("\"%1\"*").arg(term));
    }

    return terms.join(QStringLiteral(" OR "));
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

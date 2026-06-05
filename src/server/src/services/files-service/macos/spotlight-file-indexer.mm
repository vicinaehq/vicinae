#include "spotlight-file-indexer.hpp"
#include "fuzzy/fzf.hpp"
#include <CoreServices/CoreServices.h>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <climits>
#include <ranges>
#include <string>
#include <vector>

namespace {

constexpr int MIN_CANDIDATE_LIMIT = 250;
constexpr int CANDIDATE_LIMIT_MULTIPLIER = 20;

int candidateLimitFor(const Pagination &pagination) {
  int const total = std::max(0, pagination.offset) + std::max(0, pagination.limit);

  if (total == 0) { return 0; }

  return std::max(MIN_CANDIDATE_LIMIT, total * CANDIDATE_LIMIT_MULTIPLIER);
}

/**
 * Builds a Spotlight predicate matching each whitespace-separated term as a
 * case/diacritic-insensitive substring of the file name. This purposely mirrors
 * the Linux FTS5 prefix matching: it is only a candidate generator, the real
 * ranking is done afterwards by the fuzzy matcher.
 */
std::string buildPredicate(std::string_view query) {
  std::string predicate;

  for (size_t i = 0; i < query.size();) {
    while (i < query.size() && query[i] == ' ') {
      ++i;
    }

    size_t const start = i;

    while (i < query.size() && query[i] != ' ') {
      ++i;
    }

    if (i == start) { break; }

    std::string_view const word = query.substr(start, i - start);
    std::string escaped;

    escaped.reserve(word.size());

    for (char c : word) {
      if (c == '"' || c == '\\' || c == '*') { escaped.push_back('\\'); }
      escaped.push_back(c);
    }

    if (!predicate.empty()) { predicate += " || "; }
    predicate += "kMDItemFSName == \"*" + escaped + "*\"cd";
  }

  return predicate;
}

std::vector<IndexerFileResult> runQuery(const std::string &query,
                                        const AbstractFileIndexer::QueryParams &params) {
  std::vector<IndexerFileResult> results;
  int const candidateLimit = candidateLimitFor(params.pagination);

  if (candidateLimit == 0) { return results; }

  std::string const predicate = buildPredicate(query);

  if (predicate.empty()) { return results; }

  CFStringRef queryString =
      CFStringCreateWithCString(kCFAllocatorDefault, predicate.c_str(), kCFStringEncodingUTF8);

  if (!queryString) { return results; }

  MDQueryRef mdQuery = MDQueryCreate(kCFAllocatorDefault, queryString, nullptr, nullptr);
  CFRelease(queryString);

  if (!mdQuery) { return results; }

  MDQuerySetMaxCount(mdQuery, candidateLimit);

  if (!MDQueryExecute(mdQuery, kMDQuerySynchronous)) {
    CFRelease(mdQuery);
    return results;
  }

  struct Scored {
    std::filesystem::path path;
    int score = 0;
  };

  CFIndex const count = MDQueryGetResultCount(mdQuery);
  const auto &matcher = fzf::threadLocalMatcher();
  std::vector<Scored> scored;

  scored.reserve(count);

  for (CFIndex i = 0; i < count; ++i) {
    auto item = (MDItemRef)MDQueryGetResultAtIndex(mdQuery, i);

    if (!item) { continue; }

    auto pathRef = (CFStringRef)MDItemCopyAttribute(item, kMDItemPath);

    if (!pathRef) { continue; }

    char buf[PATH_MAX];

    if (CFStringGetCString(pathRef, buf, sizeof(buf), kCFStringEncodingUTF8)) {
      std::filesystem::path path(buf);
      int const fuzzyScore = matcher.fuzzy_match_v2_score_query(path.filename().string(), query);

      if (fuzzyScore > 0) { scored.emplace_back(Scored{.path = std::move(path), .score = fuzzyScore}); }
    }

    CFRelease(pathRef);
  }

  CFRelease(mdQuery);

  std::ranges::stable_sort(scored, [](const Scored &a, const Scored &b) {
    if (a.score != b.score) { return a.score > b.score; }
    return a.path < b.path;
  });

  int const offset = std::max(0, params.pagination.offset);
  int const limit = std::max(0, params.pagination.limit);
  size_t const begin = std::min(static_cast<size_t>(offset), scored.size());
  size_t const end = std::min(begin + static_cast<size_t>(limit), scored.size());

  results.reserve(end - begin);

  for (size_t i = begin; i < end; ++i) {
    results.emplace_back(
        IndexerFileResult{.path = std::move(scored[i].path), .rank = static_cast<double>(scored[i].score)});
  }

  return results;
}

} // namespace

QFuture<std::vector<IndexerFileResult>> SpotlightFileIndexer::queryAsync(std::string_view query,
                                                                         const QueryParams &params) {
  return QtConcurrent::run([params, q = std::string(query)]() {
    @autoreleasepool {
      return runQuery(q, params);
    }
  });
}

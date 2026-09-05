#include <algorithm>
#include <ranges>
#include <common/file-category.hpp>
#include "fuzzy/fuzzy-searchable.hpp"
#include "win-file-candidates.hpp"

namespace {

constexpr int MIN_CANDIDATE_LIMIT = 250;
constexpr int CANDIDATE_LIMIT_MULTIPLIER = 20;

} // namespace

int winFileCandidateLimit(int limit) {
  if (limit <= 0) { return 0; }
  return std::max(MIN_CANDIDATE_LIMIT, limit * CANDIDATE_LIMIT_MULTIPLIER);
}

std::vector<IndexerFileResult> rankWinFileCandidates(std::vector<WinFileCandidate> candidates,
                                                     const std::string &query,
                                                     const IndexerQueryParams &params) {
  struct Scored {
    std::filesystem::path path;
    int score = 0;
    vicinae::FileCategory category = vicinae::FileCategory::Other;
    std::optional<std::string> mimeType;
  };

  std::vector<Scored> scored;
  fuzzy::Query const fuzzyQuery{query};

  scored.reserve(candidates.size());

  for (WinFileCandidate &candidate : candidates) {
    auto category = candidate.isDirectory ? vicinae::FileCategory::Directory
                                          : vicinae::fileCategoryFor(candidate.path, false);

    if (params.category && *params.category != category) { continue; }

    auto const m = fuzzy::scoreWeighted({{candidate.path.filename().string(), 1.0}}, fuzzyQuery);

    if (m.accepted()) {
      scored.emplace_back(Scored{.path = std::move(candidate.path),
                                 .score = m.score,
                                 .category = category,
                                 .mimeType = std::move(candidate.mimeType)});
    }
  }

  std::ranges::stable_sort(scored, [](const Scored &a, const Scored &b) {
    if (a.score != b.score) { return a.score > b.score; }
    return a.path < b.path;
  });

  std::vector<IndexerFileResult> results;
  size_t const end = std::min(static_cast<size_t>(std::max(0, params.limit)), scored.size());

  results.reserve(end);

  for (Scored &item : scored | std::views::take(end)) {
    results.emplace_back(IndexerFileResult{.path = std::move(item.path),
                                           .rank = static_cast<double>(item.score),
                                           .category = item.category,
                                           .mimeType = std::move(item.mimeType)});
  }

  return results;
}

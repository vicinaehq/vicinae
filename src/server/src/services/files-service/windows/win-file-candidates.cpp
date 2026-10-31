#include "win-file-candidates.hpp"

vicinae::FileCategory winFileCategory(const WinFileCandidate &candidate) {
  return candidate.isDirectory ? vicinae::FileCategory::Directory
                               : vicinae::fileCategoryFor(candidate.path, false);
}

std::vector<IndexerFileResult> orderedWinFileResults(std::vector<WinFileCandidate> candidates,
                                                     const IndexerQueryParams &params) {
  std::vector<IndexerFileResult> results;

  results.reserve(candidates.size());

  for (WinFileCandidate &candidate : candidates) {
    auto const category = winFileCategory(candidate);

    if (params.category && *params.category != category) { continue; }

    results.emplace_back(IndexerFileResult{.path = std::move(candidate.path),
                                           .rank = static_cast<double>(candidates.size() - results.size()),
                                           .category = category,
                                           .mimeType = std::move(candidate.mimeType)});
  }

  return results;
}

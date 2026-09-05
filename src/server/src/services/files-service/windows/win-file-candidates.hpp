#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "services/files-service/abstract-file-indexer.hpp"

struct WinFileCandidate {
  std::filesystem::path path;
  std::optional<std::string> mimeType;
  bool isDirectory = false;
};

// Windows backends only generate candidates, the ranking is shared and done by the fuzzy matcher.
int winFileCandidateLimit(int limit);
std::vector<IndexerFileResult> rankWinFileCandidates(std::vector<WinFileCandidate> candidates,
                                                     const std::string &query,
                                                     const IndexerQueryParams &params);

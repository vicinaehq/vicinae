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

int winFileCandidateLimit(int limit);
std::vector<IndexerFileResult> rankWinFileCandidates(std::vector<WinFileCandidate> candidates,
                                                     const std::string &query,
                                                     const IndexerQueryParams &params);
std::vector<IndexerFileResult> orderedWinFileResults(std::vector<WinFileCandidate> candidates,
                                                     const IndexerQueryParams &params);

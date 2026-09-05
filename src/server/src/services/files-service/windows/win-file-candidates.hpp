#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <common/file-category.hpp>
#include "services/files-service/abstract-file-indexer.hpp"

struct WinFileCandidate {
  std::filesystem::path path;
  std::optional<std::string> mimeType;
  bool isDirectory = false;
};

vicinae::FileCategory winFileCategory(const WinFileCandidate &candidate);
std::vector<IndexerFileResult> orderedWinFileResults(std::vector<WinFileCandidate> candidates,
                                                     const IndexerQueryParams &params);

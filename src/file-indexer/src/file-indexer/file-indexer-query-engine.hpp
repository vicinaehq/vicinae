#pragma once
#include "file-indexer/file-indexer-db.hpp"
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

struct IndexerFileResult {
  std::filesystem::path path;
  double rank;
  IndexedFileCategory category = IndexedFileCategory::Other;
  std::optional<std::string> mimeType;
};

namespace file_indexer::query {

struct QueryWord {
  std::string word;
  std::vector<FileIndexerDatabase::SpellfixSuggestion> corrections;
};

struct CorrectionChoice {
  std::string original;
  std::string term;
  double weight = 1.0;
  bool corrected = false;
};

struct CorrectionPlan {
  std::vector<CorrectionChoice> choices;
};

inline constexpr double CORRECTION_PENALTY = 0.85;
inline constexpr int MAX_CORRECTION_DISTANCE = 120;

std::vector<std::string_view> splitQueryWords(std::string_view query);

std::string prepareCandidateSearchQuery(std::string_view query);

std::string prepareCorrectionSearchQuery(const CorrectionPlan &plan);

double adjustedSuggestionScore(const FileIndexerDatabase::SpellfixSuggestion &suggestion);

double correctionWeight(int distance);

std::vector<FileIndexerDatabase::SpellfixSuggestion>
pickCorrections(std::span<const FileIndexerDatabase::SpellfixSuggestion> suggestions,
                std::string_view original, size_t maxCount, bool trustKnownWords = true);

std::vector<CorrectionPlan> buildCorrectionPlans(std::span<const QueryWord> words, size_t maxPlans);

} // namespace file_indexer::query

class FileIndexerQueryEngine {
public:
  using QueryOptions = FileIndexerDatabase::SearchOptions;

  std::vector<IndexerFileResult> query(std::string_view q, int limit, const QueryOptions &options = {});
  bool isAvailable() const { return m_db.isOpen(); }

private:
  FileIndexerDatabase m_db;
};

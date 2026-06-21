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

/**
 * Typos are far more likely to be of a common word than a rare one, and spellfix's own
 * frequency weighting is too weak to surface e.g. 'config' over one-off junk tokens.
 */
double adjustedSuggestionScore(const FileIndexerDatabase::SpellfixSuggestion &suggestion);

/**
 * How much a match through this correction is worth relative to a match through the
 * word the user actually typed. Decays with spelling distance: otherwise a far
 * correction with a longer word wins on raw fzf match length alone (frstwire ->
 * firstperson beating frostwire).
 */
double correctionWeight(int distance);

/**
 * Suggestions come back score-ascending. Three coverage rules keep the few correction
 * slots useful (candidate matching is substring-based, so a word always covers every
 * path covered by its extensions):
 * - extensions of the original word add nothing: those paths already match it
 * - numbered variant families (sercom0, sercom3, ...) collapse into one slot, else
 *   ties between them hog every slot and push real corrections out
 * - a suggestion that is a prefix of an already picked one replaces it (broader)
 */
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

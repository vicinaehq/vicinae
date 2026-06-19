#include "file-indexer/file-indexer-query-engine.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/vocabulary.hpp"
#include "fuzzy/fzf.hpp"
#include "fuzzy/scored.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cctype>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace {

using namespace file_indexer::query;

using SC = FileIndexerDatabase::SearchCandidate;
using Scorer = std::function<int(const SC &)>;

constexpr const auto SCORING_BATCH_SIZE = 500;
constexpr const auto FZF_CUTOFF = 0;
constexpr const auto CANDIDATE_LIMIT = 10000;
constexpr const auto SUGGESTION_FETCH_COUNT = 20;
constexpr const auto MAX_CORRECTIONS_PER_WORD = 3;
constexpr const auto MIN_CORRECTABLE_WORD_LENGTH = 3;

enum class SubstringMatch { None, Inner, TokenStart };

bool isSingleTokenQuery(std::string_view query) {
  return !query.empty() && std::ranges::none_of(query, [](unsigned char c) { return std::isspace(c); });
}

SubstringMatch substringMatch(std::string_view text, std::string_view query) {
  if (query.empty()) return SubstringMatch::None;
  if (query.size() > text.size()) return SubstringMatch::None;

  auto lower = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };
  auto rest = text;
  bool hasInnerMatch = false;

  while (!rest.empty()) {
    auto match = std::ranges::search(rest, query, {}, lower, lower);
    if (match.empty()) break;

    auto const offset = static_cast<size_t>(std::ranges::distance(rest.begin(), match.begin()));
    auto const absoluteOffset = text.size() - rest.size() + offset;

    if (absoluteOffset == 0 || !std::isalnum(static_cast<unsigned char>(text[absoluteOffset - 1]))) {
      return SubstringMatch::TokenStart;
    }

    hasInnerMatch = true;
    rest.remove_prefix(offset + 1);
  }

  return hasInnerMatch ? SubstringMatch::Inner : SubstringMatch::None;
}

double computeSubstringMatchMultiplier(const SC &candidate, std::string_view query) {
  if (!isSingleTokenQuery(query)) return 1.0;

  auto match = std::max(substringMatch(file_indexer::vocab::basenameView(candidate.path.c_str()), query),
                        substringMatch(file_indexer::vocab::dirnameView(candidate.path.c_str()), query));

  switch (match) {
  case SubstringMatch::TokenStart:
    return 1.5;
  case SubstringMatch::Inner:
    return 1.05;
  case SubstringMatch::None:
    return 1.0;
  }

  return 1.0;
}

// for now, we don't try to be too smart about how we prioritize files
// depending on their types, because it is very easy to introduce
// undesirable behavior doing so.
// There are, however, a lot of junk files that really deserve to score lower,
// so we do some amount of work here.
double computeFileRelevanceMultiplier(const SC &candidate) {
  auto pcstr = std::string_view{candidate.path.c_str()};
  auto ext = file_indexer::vocab::fileExtensionView(candidate.path.c_str());

  // FIXME: maybe we want some logic to avoid discriminating against a file extension that
  // is explicitly in the query.

  // emacs junk
  if (pcstr.starts_with("#") && pcstr.ends_with("#")) { return 0.1; }
  if (ext.starts_with("#")) return 0.1;

  // we rarely want object files as a first match
  if (ext == "o") { return 0.5; }

  // swap file stuff, not sure if it can conflict with legitimate extensions so
  // keeping the malus medium.
  if (pcstr.ends_with("~")) return 0.3;
  if (ext == "swp") return 0.5;
  if (ext == "swo") return 0.5;
  if (ext == "swm") return 0.5;

  return 1.0;
}

int scoreCandidate(const SC &candidate, std::string_view query) {
  const auto &ranker = fzf::threadLocalMatcher();
  auto pcstr = candidate.path.c_str();
  auto filename = file_indexer::vocab::basenameView(pcstr);
  auto dirname = file_indexer::vocab::dirnameView(pcstr);
  std::initializer_list<fzf::WeightedString> strs = {{filename, 1}, {dirname, 0.7}};
  int score = ranker.fuzzy_match_v2_score_query(strs, query);

  return score * computeFileRelevanceMultiplier(candidate) *
         computeSubstringMatchMultiplier(candidate, query);
}

/**
 * Same scoring as the strict path, except every query word can match through any of
 * its corrections. Scoring against the original query alone would discard good results:
 * fzf is subsequence-based, so a transposition ('budgte') scores 0 against the file
 * it was corrected to ('budget') and a single zero word nukes the whole candidate.
 */
int scoreCandidate(const SC &candidate, std::span<const QueryWord> words) {
  const auto &ranker = fzf::threadLocalMatcher();
  std::string const filename = candidate.path.filename().string();
  std::string_view const fullPath{candidate.path.c_str()};

  auto wordScore = [&](std::string_view word) {
    int const onFilename = ranker.fuzzy_match_v2(filename, word).score;
    int const onPath = static_cast<int>(ranker.fuzzy_match_v2(fullPath, word).score * 0.7);
    return std::max(onFilename, onPath);
  };

  int total = 0;
  int count = 0;

  for (const auto &queryWord : words) {
    int best = wordScore(queryWord.word);

    for (const auto &correction : queryWord.corrections) {
      best = std::max(best,
                      static_cast<int>(wordScore(correction.word) * correctionWeight(correction.distance)));
    }

    if (best <= 0) return 0;

    total += best;
    ++count;
  }

  return count != 0 ? total / count : 0;
}

void sortCandidates(std::vector<ScoredRef<SC>> &ranked) {
  std::ranges::sort(ranked, [&](auto &&a, auto &&b) {
    if (a.score != b.score) return std::cmp_greater(a.score, b.score);
    const auto psA = a.data->path.native().size();
    const auto psB = b.data->path.native().size();
    if (psA != psB) return std::cmp_less(psA, psB);

    std::error_code ec;
    auto typeA = fs::is_directory(a.data->path, ec) ? 0 : 1;
    auto typeB = fs::is_directory(b.data->path, ec) ? 0 : 1;

    if (typeA != typeB) return std::cmp_greater(typeA, typeB);

    return std::ranges::lexicographical_compare(a.data->path.native(), b.data->path.native());
  });
}

std::vector<ScoredRef<SC>> scoreCandidatesParallel(std::span<const SC> candidates, const Scorer &scorer) {
  size_t const threadCount =
      std::min<size_t>((candidates.size() + SCORING_BATCH_SIZE - 1) / SCORING_BATCH_SIZE,
                       std::max(1u, std::thread::hardware_concurrency()));
  size_t const chunkSize = (candidates.size() + threadCount - 1) / threadCount;
  std::vector<Scored<const SC *>> ranked;

  ranked.resize(candidates.size());

  std::vector<std::thread> threads;
  std::vector<size_t> cutCounts;

  threads.resize(threadCount);
  cutCounts.resize(threadCount);

  flog::debug() << "scoring " << candidates.size() << " candidates using " << threads.size() << " threads\n";

  for (auto [idx, thread] : threads | std::views::enumerate) {
    thread = std::thread{[&, idx]() {
      const auto start = idx * chunkSize;
      const auto end = std::min(static_cast<size_t>(start + chunkSize), ranked.size());
      auto &z = cutCounts[idx];
      z = 0;

      for (auto i = start; i != end; ++i) {
        auto &candidate = candidates[i];
        auto score = scorer(candidate);
        ranked[i] = {&candidate, score};
        if (score <= FZF_CUTOFF) z += 1;
      }
    }};
  }

  for (auto &thread : threads) {
    thread.join();
  }

  auto cutCount = std::ranges::fold_left(cutCounts, 0, std::plus{});

  sortCandidates(ranked);
  ranked.resize(ranked.size() - cutCount); // everything to the right of the offset is below the score
                                           // threshold so we can just resize to nuke it

  return ranked;
}

std::vector<ScoredRef<SC>> scoreCandidates(std::span<const SC> candidates, const Scorer &scorer) {
  // if number of candidates is small, don't bother with multithreading
  if (candidates.size() < SCORING_BATCH_SIZE) {
    std::vector<ScoredRef<SC>> ranked;

    ranked.reserve(candidates.size());

    for (const auto &candidate : candidates) {
      const auto score = scorer(candidate);
      if (score > FZF_CUTOFF) ranked.push_back({&candidate, score});
    }

    sortCandidates(ranked);

    return ranked;
  }

  return scoreCandidatesParallel(candidates, scorer);
}

std::vector<IndexerFileResult> rankCandidates(std::vector<SC> candidates, const Scorer &scorer, int limit) {
  const auto ranked = scoreCandidates(std::span<const SC>{candidates}, scorer);
  std::error_code ec;
  std::vector<IndexerFileResult> results;

  results.reserve(limit);

  for (const auto &scored : ranked) {
    if (fs::exists(scored.data->path, ec)) {
      results.emplace_back(IndexerFileResult{.path = std::move(scored.data->path),
                                             .rank = static_cast<double>(scored.score),
                                             .category = scored.data->category});
      if (results.size() == limit) break;
    }
  }

  return results;
}

std::vector<IndexerFileResult> queryWithCorrections(FileIndexerDatabase &db, std::string_view q, int limit,
                                                    const FileIndexerDatabase::SearchOptions &options,
                                                    bool trustKnownWords) {
  auto words =
      splitQueryWords(q) |
      std::views::transform([](std::string_view word) { return QueryWord{.word = std::string{word}}; }) |
      std::ranges::to<std::vector>();
  bool hasCorrections = false;

  for (auto &queryWord : words) {
    if (queryWord.word.size() < MIN_CORRECTABLE_WORD_LENGTH) continue;

    // prefix and exact mode rank differently (a dropped vowel can push the right
    // word out of the prefix-mode top entirely), so merge both
    auto suggestions = db.spellfixSuggestions(queryWord.word, SUGGESTION_FETCH_COUNT, true);
    auto exact = db.spellfixSuggestions(queryWord.word, SUGGESTION_FETCH_COUNT, false);

    suggestions.insert(suggestions.end(), std::make_move_iterator(exact.begin()),
                       std::make_move_iterator(exact.end()));
    std::ranges::stable_sort(suggestions, std::less{}, adjustedSuggestionScore);

    queryWord.corrections =
        pickCorrections(suggestions, queryWord.word, MAX_CORRECTIONS_PER_WORD, trustKnownWords);
    hasCorrections |= !queryWord.corrections.empty();
  }

  if (!hasCorrections) return {};

  auto relaxedQuery = prepareRelaxedSearchQuery(words);

  if (relaxedQuery.empty()) return {};

  auto candidates = db.searchCandidates(relaxedQuery, CANDIDATE_LIMIT, options);

  flog::debug() << "spellfix fallback: '" << q << "' -> " << relaxedQuery << " (" << candidates.size()
                << " candidates)\n";

  auto scorer = [&](const SC &candidate) { return scoreCandidate(candidate, words); };

  return rankCandidates(std::move(candidates), scorer, limit);
}

} // namespace

std::vector<IndexerFileResult> FileIndexerQueryEngine::query(std::string_view q, int limit,
                                                             const QueryOptions &options) {
  constexpr auto SKELETON_THRESHOLD = 10; // below n, also query skeleton index to increase recall
  FileIndexerDatabase &db = m_db;
  if (!db.isOpen()) return {};

  auto dbQuery = prepareCandidateSearchQuery(q);

  if (dbQuery.empty()) return {};

  flog::debug() << "searching" << std::quoted(dbQuery) << "\n";

  auto candidates = db.searchCandidates(dbQuery, CANDIDATE_LIMIT, options);

  flog::debug() << "got " << candidates.size() << " candidates\n";

  // skeleton matches merge into the candidate set and the reranker arbitrates;
  // skipped when the strict query already saturated the cap
  if (candidates.size() < static_cast<size_t>(SKELETON_THRESHOLD)) {
    auto seen = candidates |
                std::views::transform([](const SC &candidate) { return candidate.path.native(); }) |
                std::ranges::to<std::unordered_set>();

    auto skeletonCandidates = db.searchSkeletonCandidates(dbQuery, CANDIDATE_LIMIT, options);

    flog::debug() << "skeleton merge: '" << q << "' -> " << skeletonCandidates.size() << " candidates\n";

    for (auto &candidate : skeletonCandidates) {
      if (!seen.contains(candidate.path.native())) { candidates.emplace_back(std::move(candidate)); }
    }
  }

  if (!candidates.empty()) {
    auto scorer = [&](const SC &candidate) { return scoreCandidate(candidate, q); };

    // an empty ranking means the candidates were noise: keep falling back
    if (auto results = rankCandidates(std::move(candidates), scorer, limit); !results.empty()) {
      return results;
    }
  }

  // attempt spellfix, trusting vocab words
  if (auto results = queryWithCorrections(db, q, limit, options, true); !results.empty()) { return results; }

  return queryWithCorrections(db, q, limit, options, false);
}

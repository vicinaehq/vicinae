#pragma once
#include "file-indexer/log.hpp"
#include "fuzzy/fzf.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "fuzzy/scored.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <filesystem>
#include <format>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

struct IndexerFileResult {
  std::filesystem::path path;
  double rank;
};

static constexpr const auto BATCH_SIZE = 500;
static constexpr const auto FZF_CUTOFF = 0;
static constexpr const auto CANDIDATE_LIMIT = 10000;

using SC = FileIndexerDatabase::SearchCandidate;

class FileIndexerQueryEngine {
public:
  std::vector<IndexerFileResult> query(std::string_view q, int limit) {
    FileIndexerDatabase db;
    auto dbQuery = prepareCandidateSearchQuery(q);

    if (dbQuery.empty()) return {};

    auto candidates = db.searchCandidates(dbQuery, CANDIDATE_LIMIT);

    auto results = rankCandidates(std::move(candidates), q, limit);

    // TODO: if we did not get enough results, try fallback paths.

    return results;
  }

  static std::string prepareCandidateSearchQuery(std::string_view query) {
    static constexpr std::string_view WHITESPACE = " \t\n\r\f\v";

    auto trim = [](std::string_view word) -> std::string_view {
      auto const begin = word.find_first_not_of(WHITESPACE);
      if (begin == std::string_view::npos) return {};
      return word.substr(begin, word.find_last_not_of(WHITESPACE) - begin + 1);
    };

    // we escape each word in the query by enclosing it in quotes, as expected by FTS5
    auto escapedParts = query | std::views::split(std::string_view{" "}) |
                        std::views::transform([&](auto &&v) { return trim(std::string_view{v}); }) |
                        std::views::filter([](auto &&v) { return v.size() >= 2; }) |
                        std::views::transform([&](auto &&str) {
                          // if it's a bigram, use prefix matching to match all trigrams that start with it
                          if (str.size() == 2) { return std::format("\"{}\"*", str); }
                          return std::format("\"{}\"", str);
                        }) |
                        std::ranges::to<std::vector>();
    auto escapedQuery =
        escapedParts | std::views::join_with(std::string_view{" "}) | std::ranges::to<std::string>();

    return escapedQuery;
  }

  static int scoreCandidate(const FileIndexerDatabase::SearchCandidate &candidate, std::string_view query) {
    const auto &ranker = fzf::threadLocalMatcher();
    std::initializer_list<fzf::WeightedString> strs = {{candidate.path.filename().c_str(), 1},
                                                       {candidate.path.c_str(), 0.7}};
    return ranker.fuzzy_match_v2_score_query(strs, query);
  }

  static std::vector<ScoredRef<SC>> scoreCandidates(std::span<const SC> candidates, std::string_view query) {
    // if number of candidates is small, don't bother with multithreading
    if (candidates.size() < BATCH_SIZE) {
      std::vector<ScoredRef<SC>> ranked;

      ranked.reserve(candidates.size());

      for (const auto &candidate : candidates) {
        const auto score = scoreCandidate(candidate, query);
        if (score > FZF_CUTOFF) ranked.push_back({&candidate, score});
      }

      std::ranges::sort(ranked, std::greater{});

      return ranked;
    }

    return scoreCandidatesParallel(candidates, query);
  }

  static std::vector<ScoredRef<FileIndexerDatabase::SearchCandidate>>
  scoreCandidatesParallel(std::span<const FileIndexerDatabase::SearchCandidate> candidates,
                          std::string_view query) {
    auto batchCount = std::min(static_cast<unsigned int>((candidates.size() + BATCH_SIZE - 1) / BATCH_SIZE),
                               std::thread::hardware_concurrency());
    std::vector<Scored<const FileIndexerDatabase::SearchCandidate *>> ranked;

    ranked.resize(candidates.size());

    std::vector<std::thread> threads;
    std::vector<size_t> cutCounts;

    threads.resize(batchCount);
    cutCounts.resize(batchCount);

    flog::info() << "query " << query << " on " << candidates.size() << " using " << threads.size()
                 << " batches\n";

    for (auto [idx, thread] : threads | std::views::enumerate) {
      flog::info() << "starting up thread " << idx << "\n";
      thread = std::thread{[&, idx]() {
        const auto &ranker = fzf::threadLocalMatcher();
        const auto start = idx * BATCH_SIZE;
        const auto end = std::min(static_cast<size_t>(start + BATCH_SIZE), ranked.size());
        auto &z = cutCounts[idx];
        z = 0;

        for (auto i = start; i != end; ++i) {
          auto &candidate = candidates[i];
          auto score = scoreCandidate(candidate, query);
          ranked[i] = {&candidate, score};
          if (score <= FZF_CUTOFF) z += 1;
        }
      }};
    }

    for (auto &thread : threads) {
      thread.join();
    }

    auto cutCount = std::ranges::fold_left(cutCounts, 0, std::plus{});

    flog::info() << "all threads joined\n";

    std::ranges::sort(ranked, std::greater{});
    ranked.resize(ranked.size() - cutCount); // everything to the right of the offset is below the score
                                             // threshold so we can just resize to nuke it

    return ranked;
  }

  static std::vector<IndexerFileResult>
  rankCandidates(std::vector<FileIndexerDatabase::SearchCandidate> candidates, std::string_view query,
                 int limit) {
    const auto ranked = scoreCandidates(candidates, query);
    std::error_code ec;
    std::vector<IndexerFileResult> results;

    results.reserve(limit);

    for (const auto &scored : ranked) {
      if (std::filesystem::exists(scored.data->path, ec)) {
        results.emplace_back(
            IndexerFileResult{std::move(scored.data->path), static_cast<double>(scored.score)});
        if (results.size() == limit) break;
      }
    }

    return results;
  }
};

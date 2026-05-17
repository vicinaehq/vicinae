#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include "services/files-service/file-indexer/file-indexer-query-engine.hpp"

namespace {

Pagination firstPage(int limit = 10) {
  Pagination pagination;

  pagination.limit = limit;
  return pagination;
}

FileIndexerDatabase::SearchCandidate candidate(std::string name, double relevancyScore = 1.0,
                                               double indexRank = 0.0,
                                               std::string directoryName = "default") {
  return FileIndexerDatabase::SearchCandidate{
      .path = std::filesystem::path("/tmp/vicinae-file-search-tests") / directoryName / name,
      .name = std::move(name),
      .relevancyScore = relevancyScore,
      .indexRank = indexRank,
  };
}

std::string filenameAt(const std::vector<IndexerFileResult> &results, size_t index) {
  REQUIRE(index < results.size());
  return results.at(index).path.filename().string();
}

} // namespace

TEST_CASE("file indexer query makes every word prefix-searchable") {
  REQUIRE(FileIndexerQueryEngine::prepareCandidateSearchQuery(QStringLiteral("fol exa")) ==
          QStringLiteral("\"fol\"* OR \"exa\"*"));
  REQUIRE(FileIndexerQueryEngine::prepareCandidateSearchQuery(QStringLiteral("invoice")) ==
          QStringLiteral("\"invoice\"*"));
  REQUIRE(FileIndexerQueryEngine::prepareCandidateSearchQuery(QStringLiteral("  inv   new  ")) ==
          QStringLiteral("\"inv\"* OR \"new\"*"));
}

TEST_CASE("file indexer fuzzy reranking preserves existing token-prefix matches") {
  std::vector<FileIndexerDatabase::SearchCandidate> candidates{
      candidate("invoice-new-motherboard.pdf"),
      candidate("random archive.zip"),
  };

  auto results = FileIndexerQueryEngine::rankCandidates(candidates, "inv", firstPage());
  REQUIRE(results.size() == 1);
  REQUIRE(filenameAt(results, 0) == "invoice-new-motherboard.pdf");

  results = FileIndexerQueryEngine::rankCandidates(candidates, "mother", firstPage());
  REQUIRE(results.size() == 1);
  REQUIRE(filenameAt(results, 0) == "invoice-new-motherboard.pdf");
}

TEST_CASE("file indexer fuzzy reranking matches split query words in any order") {
  auto results = FileIndexerQueryEngine::rankCandidates(
      {candidate("example folder"), candidate("unrelated notes")}, "fol exa", firstPage());

  REQUIRE(results.size() == 1);
  REQUIRE(filenameAt(results, 0) == "example folder");

  results = FileIndexerQueryEngine::rankCandidates(
      {candidate("example folder"), candidate("unrelated notes")}, "exa fol", firstPage());

  REQUIRE(results.size() == 1);
  REQUIRE(filenameAt(results, 0) == "example folder");
}

TEST_CASE("file indexer fuzzy reranking supports sparse fzf-style matches when candidates are present") {
  auto results = FileIndexerQueryEngine::rankCandidates(
      {candidate("Keyboard Settings"), candidate("System Info Event Log")}, "kbd stg", firstPage());

  REQUIRE(results.size() == 1);
  REQUIRE(filenameAt(results, 0) == "Keyboard Settings");
}

TEST_CASE("file indexer fuzzy reranking filters unrelated candidates") {
  auto results = FileIndexerQueryEngine::rankCandidates(
      {candidate("example folder"), candidate("random archive")}, "zz top", firstPage());

  REQUIRE(results.empty());
}

TEST_CASE("file indexer fuzzy reranking prefers stronger basename matches") {
  auto results = FileIndexerQueryEngine::rankCandidates(
      {candidate("profile editor"), candidate("File Manager")}, "file", firstPage());

  REQUIRE(results.size() == 2);
  REQUIRE(filenameAt(results, 0) == "File Manager");
  REQUIRE(filenameAt(results, 1) == "profile editor");
}

TEST_CASE("file indexer fuzzy reranking uses relevancy as a tiebreaker") {
  auto results = FileIndexerQueryEngine::rankCandidates(
      {candidate("project notes", 0.5, 0.0, "low"), candidate("project notes", 2.0, 0.0, "high")}, "project",
      firstPage());

  REQUIRE(results.size() == 2);
  REQUIRE(results.at(0).path.parent_path().filename().string() == "high");
  REQUIRE(results.at(0).rank == results.at(1).rank);
}

TEST_CASE("file indexer fuzzy reranking applies pagination after scoring") {
  Pagination secondResult;

  secondResult.offset = 1;
  secondResult.limit = 1;

  auto results = FileIndexerQueryEngine::rankCandidates(
      {candidate("profile editor", 1.0), candidate("File Manager", 3.0), candidate("File Mail", 2.0)}, "file",
      secondResult);

  REQUIRE(results.size() == 1);
  REQUIRE(filenameAt(results, 0) == "File Mail");
}

#include <catch2/catch_test_macros.hpp>
#include <QDateTime>
#include <QTemporaryDir>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "services/snippet/snippet-db.hpp"

namespace fs = std::filesystem;

using snippet::SerializedSnippet;
using snippet::SnippetPayload;
using snippet::TextSnippet;

namespace {

QTemporaryDir makeTempDir() {
  QTemporaryDir dir;
  dir.setAutoRemove(true);
  return dir;
}

snippet::SnippetPayload textPayload(const std::string &name) {
  SnippetPayload payload;
  payload.name = name;
  payload.data = TextSnippet{.text = "content of " + name};
  return payload;
}

std::vector<std::string> ids(const std::vector<SerializedSnippet> &snippets) {
  std::vector<std::string> out;
  out.reserve(snippets.size());
  for (const auto &snippet : snippets)
    out.emplace_back(snippet.id);
  return out;
}

std::optional<std::uint64_t> updatedAtOf(const SnippetDatabase &db, std::string_view id) {
  for (const auto &snippet : db.snippets()) {
    if (snippet.id == id) return snippet.updatedAt;
  }
  return std::nullopt;
}

std::string fileContents(const std::filesystem::path &path) {
  std::ifstream in(path, std::ios::binary);
  return std::string{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

void overwriteFileWithDirectory(const std::filesystem::path &path) {
  fs::remove_all(path);
  if (!fs::create_directory(path)) { FAIL("Failed to corrupt snippet storage path"); }
}

} // namespace

TEST_CASE("moveSnippetUp and moveSnippetDown swap adjacent snippets", "[snippet-db]") {
  auto temp = makeTempDir();
  REQUIRE(temp.isValid());
  const auto path = temp.filePath("snippets.json").toStdString();
  SnippetDatabase db(path);

  std::vector<std::string> added;
  added.reserve(3);
  for (const auto &name : {"first", "second", "third"}) {
    auto created = db.addSnippet(textPayload(name));
    REQUIRE(created.has_value());
    added.emplace_back(created->id);
  }

  SECTION("unknown ids fail without changing order") {
    REQUIRE_FALSE(db.moveSnippetUp("does-not-exist").has_value());
    REQUIRE_FALSE(db.moveSnippetDown("does-not-exist").has_value());
    REQUIRE(ids(db.snippets()) == added);
  }

  SECTION("moving the first snippet up is a no-op") {
    REQUIRE(db.moveSnippetUp(added[0]).has_value());
    REQUIRE(ids(db.snippets()) == added);
  }

  SECTION("moving the last snippet down is a no-op") {
    REQUIRE(db.moveSnippetDown(added[2]).has_value());
    REQUIRE(ids(db.snippets()) == added);
  }

  SECTION("moving up swaps a snippet with its predecessor") {
    REQUIRE(db.moveSnippetUp(added[1]).has_value());
    REQUIRE(ids(db.snippets()) == std::vector<std::string>{added[1], added[0], added[2]});
  }

  SECTION("moving down swaps a snippet with its successor") {
    REQUIRE(db.moveSnippetDown(added[1]).has_value());
    REQUIRE(ids(db.snippets()) == std::vector<std::string>{added[0], added[2], added[1]});
  }

  SECTION("moves do not touch timestamps") {
    const auto beforeFirst = updatedAtOf(db, added[0]);
    const auto beforeSecond = updatedAtOf(db, added[1]);
    REQUIRE(db.moveSnippetUp(added[1]).has_value());
    REQUIRE(updatedAtOf(db, added[1]) == beforeSecond);
    REQUIRE(updatedAtOf(db, added[0]) == beforeFirst);
  }

  SECTION("failed persistence rolls the in-memory order back") {
    auto before = ids(db.snippets());
    overwriteFileWithDirectory(path);
    auto attempted = db.moveSnippetUp(added[1]);
    REQUIRE_FALSE(attempted.has_value());
    REQUIRE(ids(db.snippets()) == before);
  }

  SECTION("order survives a reload from disk") {
    REQUIRE(db.moveSnippetUp(added[1]).has_value());
    const auto persisted = fileContents(path);
    SnippetDatabase reloaded(path);
    REQUIRE(ids(reloaded.snippets()) == ids(db.snippets()));
    REQUIRE_FALSE(persisted.empty());
  }

  SECTION("subsequent edits and removals preserve the new order") {
    REQUIRE(db.moveSnippetUp(added[1]).has_value());
    const auto reordered = ids(db.snippets());

    REQUIRE(db.removeSnippet(added[2]).has_value());
    REQUIRE(ids(db.snippets()) == std::vector<std::string>{reordered[0], reordered[1]});

    auto created = db.addSnippet(textPayload("fourth"));
    REQUIRE(created.has_value());
    REQUIRE(ids(db.snippets()).back() == created->id);

    auto updated = db.updateSnippet(added[0], textPayload("renamed"));
    REQUIRE(updated.has_value());
    REQUIRE(ids(db.snippets()) == std::vector<std::string>{reordered[0], reordered[1], created->id});
  }

  SECTION("moves on an empty database fail cleanly") {
    SnippetDatabase emptyDb(temp.filePath("empty.json").toStdString());
    REQUIRE(emptyDb.snippets().empty());
    REQUIRE_FALSE(emptyDb.moveSnippetUp("nope").has_value());
    REQUIRE_FALSE(emptyDb.moveSnippetDown("nope").has_value());
    REQUIRE(emptyDb.snippets().empty());
  }
}

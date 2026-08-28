#include <catch2/catch_test_macros.hpp>
#include <QTemporaryDir>
#include <span>
#include "omni-database.hpp"
#include "services/note/note-db.hpp"

namespace {

/// Every fixture gets its own on-disk database so migrations run for real.
class NoteFixture {
public:
  NoteFixture() {
    REQUIRE(m_dir.isValid());
    m_omniDb = std::make_unique<OmniDatabase>(m_dir.filePath("vicinae.db").toStdString());
    m_db = std::make_unique<NoteDatabase>(*m_omniDb);
  }

  NoteDatabase &db() { return *m_db; }

  note::Note insert(const std::string &title, const std::string &body = {}) {
    auto res = m_db->createNote({.title = title, .body = body});
    REQUIRE(res);
    return *res;
  }

private:
  QTemporaryDir m_dir;
  std::unique_ptr<OmniDatabase> m_omniDb;
  std::unique_ptr<NoteDatabase> m_db;
};

std::vector<std::string> titlesMatching(const NoteDatabase &db, std::string_view query) {
  std::vector<Scored<int>> filtered;
  const auto &notes = db.notes();

  fuzzy::fuzzyFilter<note::Note>(std::span<const note::Note>(notes), query, filtered);

  std::vector<std::string> titles;

  titles.reserve(filtered.size());

  for (const auto &scored : filtered) {
    titles.emplace_back(notes[scored.data].title);
  }

  return titles;
}

} // namespace

TEST_CASE("the migration creates an empty note table") {
  NoteFixture fixture;

  REQUIRE(fixture.db().notes().empty());
}

TEST_CASE("inserted notes are persisted and readable") {
  NoteFixture fixture;

  auto note = fixture.insert("Grocery list", "- milk\n- eggs");

  REQUIRE_FALSE(note.id.empty());
  REQUIRE(note.createdAt > 0);
  REQUIRE(note.updatedAt == note.createdAt);
  REQUIRE_FALSE(note.pinned());

  REQUIRE(fixture.db().notes().size() == 1);

  const auto *stored = fixture.db().findById(note.id);

  REQUIRE(stored);
  REQUIRE(stored->title == "Grocery list");
  REQUIRE(stored->body == "- milk\n- eggs");
}

TEST_CASE("notes without a title are rejected") {
  NoteFixture fixture;

  REQUIRE_FALSE(fixture.db().createNote({.title = "", .body = "orphan"}));
  REQUIRE_FALSE(fixture.db().createNote({.title = std::string(NoteDatabase::MAX_TITLE_LENGTH + 1, 'a')}));
  REQUIRE(fixture.db().notes().empty());
}

TEST_CASE("updating a note rewrites its title and body") {
  NoteFixture fixture;

  auto note = fixture.insert("Draft", "wip");
  auto updated = fixture.db().updateNote(note.id, {.title = "Final", .body = "done"});

  REQUIRE(updated);
  REQUIRE(updated->id == note.id);
  REQUIRE(updated->createdAt == note.createdAt);
  REQUIRE(updated->updatedAt >= note.updatedAt);

  const auto *stored = fixture.db().findById(note.id);

  REQUIRE(stored);
  REQUIRE(stored->title == "Final");
  REQUIRE(stored->body == "done");
}

TEST_CASE("updating an unknown note fails") {
  NoteFixture fixture;

  REQUIRE_FALSE(fixture.db().updateNote("nope", {.title = "Final"}));
  REQUIRE_FALSE(fixture.db().removeNote("nope"));
  REQUIRE_FALSE(fixture.db().setPinned("nope", true));
  REQUIRE_FALSE(fixture.db().registerUse("nope"));
}

TEST_CASE("pinned notes are listed first") {
  NoteFixture fixture;

  fixture.insert("First");
  fixture.insert("Second");
  auto third = fixture.insert("Third");

  REQUIRE(fixture.db().notes().front().title == "Third");

  auto pinned = fixture.db().setPinned(third.id, true);

  REQUIRE(pinned);
  REQUIRE(pinned->pinned());

  auto stillPinned = fixture.db().setPinned(fixture.db().notes().at(1).id, true);

  REQUIRE(stillPinned);

  const auto &notes = fixture.db().notes();

  REQUIRE(notes.size() == 3);
  REQUIRE(notes[0].pinned());
  REQUIRE(notes[1].pinned());
  REQUIRE_FALSE(notes[2].pinned());

  REQUIRE(fixture.db().setPinned(third.id, false));
  REQUIRE_FALSE(fixture.db().findById(third.id)->pinned());
}

TEST_CASE("using a note records the last use timestamp") {
  NoteFixture fixture;

  auto note = fixture.insert("Snippet-ish");

  REQUIRE_FALSE(note.lastUsedAt);
  REQUIRE(fixture.db().registerUse(note.id));
  REQUIRE(fixture.db().findById(note.id)->lastUsedAt);
}

TEST_CASE("fuzzy search matches titles and bodies") {
  NoteFixture fixture;

  fixture.insert("Grocery list", "milk and eggs");
  fixture.insert("Release checklist", "bump the version");
  fixture.insert("Postgres commands", "vacuum analyze");

  REQUIRE(titlesMatching(fixture.db(), "grocery") == std::vector<std::string>{"Grocery list"});
  REQUIRE(titlesMatching(fixture.db(), "vacuum") == std::vector<std::string>{"Postgres commands"});
  REQUIRE(titlesMatching(fixture.db(), "chcklst") == std::vector<std::string>{"Release checklist"});
  REQUIRE(titlesMatching(fixture.db(), "nothing-here").empty());
  REQUIRE(titlesMatching(fixture.db(), "").size() == 3);
}

TEST_CASE("removed notes are gone") {
  NoteFixture fixture;

  auto keep = fixture.insert("Keep");
  auto drop = fixture.insert("Drop");

  auto removed = fixture.db().removeNote(drop.id);

  REQUIRE(removed);
  REQUIRE(removed->title == "Drop");
  REQUIRE_FALSE(fixture.db().findById(drop.id));

  const auto &notes = fixture.db().notes();

  REQUIRE(notes.size() == 1);
  REQUIRE(notes.front().id == keep.id);
}

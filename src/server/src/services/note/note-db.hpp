#pragma once
#include <QCoreApplication>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "fuzzy/fuzzy-searchable.hpp"

class OmniDatabase;

namespace note {

struct Note {
  std::string id;
  std::string title;
  std::string body;
  std::optional<std::uint64_t> pinnedAt;
  std::uint64_t createdAt = 0;
  std::uint64_t updatedAt = 0;
  std::optional<std::uint64_t> lastUsedAt;

  bool pinned() const { return pinnedAt.has_value(); }
};

struct NotePayload {
  std::string title;
  std::string body;
};

} // namespace note

template <> struct fuzzy::FuzzySearchable<note::Note> {
  static fuzzy::Match score(const note::Note &item, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{item.title, 1.0}, {item.body, 0.4}}, query);
  }
};

/**
 * CRUD over the `note` table. Notes are few and small, so the whole table is cached in memory and
 * refreshed from SQL after each mutation, which keeps a single source of truth for the ordering
 * (pinned first, then most recently updated, insertion order breaking same-second ties).
 */
class NoteDatabase {
  Q_DECLARE_TR_FUNCTIONS(NoteDatabase)

public:
  static constexpr std::size_t MAX_NOTES = 10000;
  static constexpr std::size_t MAX_TITLE_LENGTH = 200;

  explicit NoteDatabase(OmniDatabase &db);

  std::expected<note::Note, std::string> createNote(const note::NotePayload &payload);
  std::expected<note::Note, std::string> updateNote(std::string_view id, const note::NotePayload &payload);
  std::expected<note::Note, std::string> removeNote(std::string_view id);
  std::expected<note::Note, std::string> setPinned(std::string_view id, bool pinned);
  std::expected<void, std::string> registerUse(std::string_view id);

  const std::vector<note::Note> &notes() const { return m_notes; }
  const note::Note *findById(std::string_view id) const;

private:
  static std::optional<std::string> validate(const note::NotePayload &payload);

  void reload();

  OmniDatabase &m_db;
  std::vector<note::Note> m_notes;
};

#include <QDateTime>
#include <QUuid>
#include <algorithm>
#include "omni-database.hpp"
#include "services/note/note-db.hpp"

using namespace note;

namespace {
std::uint64_t now() { return static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch()); }
} // namespace

NoteDatabase::NoteDatabase(OmniDatabase &db) : m_db(db) { reload(); }

void NoteDatabase::reload() {
  auto stmt = m_db.db().prepare(R"(
    SELECT id, title, body, pinned_at, created_at, updated_at, last_used_at FROM note
    ORDER BY pinned_at DESC, updated_at DESC, rowid DESC
  )");

  m_notes.clear();

  while (stmt.step()) {
    Note note;

    note.id = stmt.columnText(0);
    note.title = stmt.columnText(1);
    note.body = stmt.columnText(2);
    if (!stmt.isNull(3)) note.pinnedAt = stmt.columnUInt64(3);
    note.createdAt = stmt.columnUInt64(4);
    note.updatedAt = stmt.columnUInt64(5);
    if (!stmt.isNull(6)) note.lastUsedAt = stmt.columnUInt64(6);

    m_notes.emplace_back(std::move(note));
  }
}

const Note *NoteDatabase::findById(std::string_view id) const {
  auto it = std::ranges::find_if(m_notes, [&](auto &&note) { return note.id == id; });

  return it == m_notes.end() ? nullptr : &*it;
}

std::optional<std::string> NoteDatabase::validate(const NotePayload &payload) {
  if (payload.title.empty()) return tr("Title cannot be empty").toStdString();
  if (payload.title.size() > MAX_TITLE_LENGTH) {
    return tr("Title exceeds maximum length of %1").arg(MAX_TITLE_LENGTH).toStdString();
  }

  return std::nullopt;
}

std::expected<Note, std::string> NoteDatabase::createNote(const NotePayload &payload) {
  if (m_notes.size() >= MAX_NOTES) {
    return std::unexpected(tr("Note limit reached (%1)").arg(MAX_NOTES).toStdString());
  }

  if (const auto error = validate(payload)) { return std::unexpected(*error); }

  const auto id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
  const auto timestamp = now();

  auto stmt = m_db.db().prepare(R"(
    INSERT INTO note (id, title, body, created_at, updated_at)
    VALUES (:id, :title, :body, :created_at, :updated_at)
  )");

  stmt.bind(":id", id);
  stmt.bind(":title", payload.title);
  stmt.bind(":body", payload.body);
  stmt.bind(":created_at", timestamp);
  stmt.bind(":updated_at", timestamp);

  if (!stmt.exec()) { return std::unexpected(m_db.db().lastError()); }

  reload();

  return Note{
      .id = id,
      .title = payload.title,
      .body = payload.body,
      .createdAt = timestamp,
      .updatedAt = timestamp,
  };
}

std::expected<Note, std::string> NoteDatabase::updateNote(std::string_view id, const NotePayload &payload) {
  if (const auto error = validate(payload)) { return std::unexpected(*error); }

  const auto *existing = findById(id);

  if (!existing) { return std::unexpected(tr("No note with that ID").toStdString()); }

  Note updated = *existing;

  updated.title = payload.title;
  updated.body = payload.body;
  updated.updatedAt = now();

  auto stmt = m_db.db().prepare(R"(
    UPDATE note SET title = :title, body = :body, updated_at = :updated_at WHERE id = :id
  )");

  stmt.bind(":title", updated.title);
  stmt.bind(":body", updated.body);
  stmt.bind(":updated_at", updated.updatedAt);
  stmt.bind(":id", id);

  if (!stmt.exec()) { return std::unexpected(m_db.db().lastError()); }

  reload();

  return updated;
}

std::expected<Note, std::string> NoteDatabase::removeNote(std::string_view id) {
  const auto *existing = findById(id);

  if (!existing) { return std::unexpected(tr("No note with that ID").toStdString()); }

  Note removed = *existing;
  auto stmt = m_db.db().prepare("DELETE FROM note WHERE id = :id");

  stmt.bind(":id", id);

  if (!stmt.exec()) { return std::unexpected(m_db.db().lastError()); }

  reload();

  return removed;
}

std::expected<Note, std::string> NoteDatabase::setPinned(std::string_view id, bool pinned) {
  const auto *existing = findById(id);

  if (!existing) { return std::unexpected(tr("No note with that ID").toStdString()); }

  Note updated = *existing;

  updated.pinnedAt = pinned ? std::optional{now()} : std::nullopt;

  auto stmt = m_db.db().prepare("UPDATE note SET pinned_at = :pinned_at WHERE id = :id");

  stmt.bind(":pinned_at", updated.pinnedAt);
  stmt.bind(":id", id);

  if (!stmt.exec()) { return std::unexpected(m_db.db().lastError()); }

  reload();

  return updated;
}

std::expected<void, std::string> NoteDatabase::registerUse(std::string_view id) {
  if (!findById(id)) { return std::unexpected(tr("No note with that ID").toStdString()); }

  auto stmt = m_db.db().prepare("UPDATE note SET last_used_at = :last_used_at WHERE id = :id");

  stmt.bind(":last_used_at", now());
  stmt.bind(":id", id);

  if (!stmt.exec()) { return std::unexpected(m_db.db().lastError()); }

  reload();

  return {};
}

#pragma once
#include <qobject.h>
#include <qtmetamacros.h>
#include "services/note/note-db.hpp"

class OmniDatabase;

class NoteService : public QObject {
  Q_OBJECT

signals:
  void notesChanged();

public:
  explicit NoteService(OmniDatabase &db) : m_db(db) {}

  auto createNote(const note::NotePayload &payload) {
    auto res = m_db.createNote(payload);
    if (res) emit notesChanged();
    return res;
  }

  auto updateNote(std::string_view id, const note::NotePayload &payload) {
    auto res = m_db.updateNote(id, payload);
    if (res) emit notesChanged();
    return res;
  }

  auto removeNote(std::string_view id) {
    auto res = m_db.removeNote(id);
    if (res) emit notesChanged();
    return res;
  }

  auto setPinned(std::string_view id, bool pinned) {
    auto res = m_db.setPinned(id, pinned);
    if (res) emit notesChanged();
    return res;
  }

  auto registerUse(std::string_view id) { return m_db.registerUse(id); }

  NoteDatabase *database() { return &m_db; }

private:
  NoteDatabase m_db;
};

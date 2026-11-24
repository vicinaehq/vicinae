#pragma once

#include "fuzzy/fuzzy-searchable.hpp"
#include "services/notes/notes-store.hpp"

template <> struct fuzzy::FuzzySearchable<Notes::Note> {
  static fuzzy::Match score(const Notes::Note &note, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{note.title, 1.0}, {note.markdown, 0.5}}, query);
  }
};

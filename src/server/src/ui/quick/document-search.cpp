#include <QtConcurrentRun>
#include <algorithm>
#include <optional>
#include <tuple>
#include "document-search.hpp"

DocumentSearch::DocumentSearch(QObject *parent) : QObject(parent) {
  m_timer.setSingleShot(true);
  m_timer.setInterval(60);
  connect(&m_timer, &QTimer::timeout, this, &DocumentSearch::search);
}

DocumentSearch::~DocumentSearch() { m_cancellation.request_stop(); }

void DocumentSearch::setModel(DocumentModel *model) {
  if (m_model == model) return;
  if (m_model) disconnect(m_model, nullptr, this, nullptr);
  m_model = model;
  if (model) {
    connect(model, &QAbstractItemModel::modelReset, this, &DocumentSearch::reset);
    connect(model, &QAbstractItemModel::layoutChanged, this, &DocumentSearch::reset);
    connect(model, &QAbstractItemModel::rowsMoved, this, &DocumentSearch::reset);
    connect(model, &QObject::destroyed, this, &DocumentSearch::reset);
    connect(model, &QAbstractItemModel::rowsInserted, this,
            [this](const QModelIndex &, int first, int last) { rowsChanged(first, 0, last - first + 1); });
    connect(model, &QAbstractItemModel::rowsRemoved, this,
            [this](const QModelIndex &, int first, int last) { rowsChanged(first, last - first + 1, 0); });
    connect(
        model, &QAbstractItemModel::dataChanged, this,
        [this](const QModelIndex &first, const QModelIndex &last) { invalidate(first.row(), last.row()); });
  }
  reset();
}

void DocumentSearch::rowsChanged(int first, int removed, int added) {
  if (m_query.isEmpty()) return;
  m_rows.erase(m_rows.begin() + first, m_rows.begin() + first + removed);
  m_rows.insert(m_rows.begin() + first, added, nullptr);
  const int before =
      std::ranges::count_if(std::span(m_matches).first(std::max(0, m_current)), [=](const auto &match) {
        return match.row >= first && match.row < first + removed;
      });
  std::erase_if(m_matches,
                [=](const auto &match) { return match.row >= first && match.row < first + removed; });
  for (auto &match : m_matches)
    if (match.row >= first) match.row += added - removed;
  m_current = m_matches.empty() ? -1 : std::clamp(m_current - before, 0, count() - 1);
  emit resultsChanged();
  emit currentIndexChanged();
  schedule();
}

void DocumentSearch::reset() {
  m_rows.clear();
  m_resultQuery.clear();
  if (m_model && !m_query.isEmpty()) {
    m_rows.reserve(m_model->rowCount());
    m_rows.resize(m_model->rowCount());
  }
  m_matches.clear();
  m_current = -1;
  emit resultsChanged();
  emit currentIndexChanged();
  schedule();
}

void DocumentSearch::invalidate(int first, int last) {
  if (m_query.isEmpty()) return;
  for (int row = first; row <= last && row < int(m_rows.size()); ++row)
    m_rows[row].reset();
  schedule();
}

void DocumentSearch::setQuery(const QString &query) {
  if (m_query == query) return;
  const bool wasEmpty = m_query.isEmpty();
  m_query = query;
  m_reveal = !query.isEmpty();
  emit queryChanged();
  if (wasEmpty || query.isEmpty()) {
    reset();
  } else {
    schedule();
  }
}

void DocumentSearch::setBusy(bool busy) {
  if (m_busy == busy) return;
  m_busy = busy;
  emit busyChanged();
}

void DocumentSearch::schedule() {
  ++m_revision;
  m_cancellation.request_stop();
  if (m_query.isEmpty() || !m_model) {
    m_timer.stop();
    setBusy(false);
    return;
  }
  setBusy(true);
  if (!m_timer.isActive()) m_timer.start();
}

void DocumentSearch::search() {
  if (m_running || m_query.isEmpty() || !m_model) return;
  for (int row = 0; row < int(m_rows.size()); ++row) {
    if (m_rows[row]) continue;
    m_rows[row] = std::make_shared<Row>();
    m_rows[row]->snapshot = m_model->textSnapshot(row);
  }
  m_running = true;
  m_cancellation = {};
  const auto revision = m_revision;
  QtConcurrent::run([rows = m_rows, query = m_query, cancellation = m_cancellation.get_token()] {
    std::vector<Match> matches;
    matches.reserve(64);
    for (int row = 0; row < int(rows.size()); ++row) {
      if (cancellation.stop_requested()) return matches;
      auto &entry = *rows[row];
      if (entry.snapshot) {
        entry.parts = entry.snapshot();
        entry.snapshot = {};
      }
      if (entry.query != query) {
        entry.query.clear();
        entry.matches.clear();
        entry.matches.reserve(8);
        for (int part = 0; part < int(entry.parts.size()); ++part) {
          const auto &text = entry.parts[part];
          if (text.atomic) continue;
          qsizetype offset = 0;
          while ((offset = text.text.indexOf(query, offset, Qt::CaseInsensitive)) >= 0) {
            if (cancellation.stop_requested()) return matches;
            if (entry.matches.size() == entry.matches.capacity())
              entry.matches.reserve(entry.matches.size() * 2);
            entry.matches.emplace_back(Match{0, part, int(offset), int(query.size())});
            offset += query.size();
          }
        }
        entry.query = query;
      }
      if (matches.size() + entry.matches.size() > matches.capacity())
        matches.reserve(std::max(matches.capacity() * 2, matches.size() + entry.matches.size()));
      for (auto match : entry.matches) {
        match.row = row;
        matches.emplace_back(match);
      }
    }
    return matches;
  }).then(this, [this, revision](std::vector<Match> matches) {
    m_running = false;
    if (revision != m_revision) {
      if (!m_timer.isActive()) search();
      return;
    }
    const auto previous = currentMatch() ? std::optional(*currentMatch()) : std::nullopt;
    m_matches = std::move(matches);
    m_resultQuery = m_query;
    m_current = m_matches.empty() ? -1 : 0;
    if (previous) {
      const auto location = [](const Match &match) { return std::tie(match.row, match.part, match.start); };
      const auto found = std::ranges::lower_bound(m_matches, location(*previous), {}, location);
      if (found != m_matches.end()) m_current = int(found - m_matches.begin());
    }
    setBusy(false);
    emit resultsChanged();
    emit currentIndexChanged();
    if (m_reveal && currentMatch()) {
      m_reveal = false;
      const auto match = *currentMatch();
      emit revealRequested(match.row, match.part, match.start, match.length);
    }
  });
}

std::span<const DocumentSearch::Match> DocumentSearch::matches(int row, int part) const {
  const auto key = std::pair(row, part);
  const auto range = std::ranges::equal_range(
      m_matches, key, {}, [](const Match &match) { return std::pair(match.row, match.part); });
  return {range.begin(), range.end()};
}

const DocumentSearch::Match *DocumentSearch::currentMatch() const {
  return m_current >= 0 && m_current < count() ? &m_matches[m_current] : nullptr;
}

void DocumentSearch::move(int direction) {
  if (m_busy || m_matches.empty()) return;
  m_current = (m_current + direction + count()) % count();
  emit currentIndexChanged();
  const auto match = *currentMatch();
  emit revealRequested(match.row, match.part, match.start, match.length);
}

void DocumentSearch::next() { move(1); }
void DocumentSearch::previous() { move(-1); }

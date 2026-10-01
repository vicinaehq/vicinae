#include "services/root-item-manager/search-history.hpp"
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <utility>
#include <qlogging.h>
#include <QDateTime>

namespace fs = std::filesystem;

namespace {
constexpr auto MAX_HISTORY_SIZE = 1000;
}

SearchHistory::SearchHistory(const fs::path &path) : m_path(path) { loadFromDisk(); }

void SearchHistory::add(std::string_view q) {
  if (q.empty()) return;

  std::erase_if(m_data.entries, [&](const Entry &entry) { return entry.q == q; });
  m_data.entries.emplace_front(
      Entry{.q = std::string{q}, .ts = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch())});
  if (m_data.entries.size() > MAX_HISTORY_SIZE) { m_data.entries.resize(MAX_HISTORY_SIZE); }

  saveToDisk();
}

std::optional<SearchHistory::Entry> SearchHistory::at(int offset) const {
  if (offset < 0 || std::cmp_greater_equal(offset, m_data.entries.size())) return std::nullopt;
  return m_data.entries[offset];
}

void SearchHistory::loadFromDisk() {
  if (!fs::exists(m_path)) return;

  if (auto error = glz::read_file_jsonc(m_data, m_path.string(), m_buf)) {
    qWarning() << "Failed to load search history from" << m_path.string().c_str() << glz::format_error(error);
  }
}

void SearchHistory::saveToDisk() {
  if (auto error = glz::write_file_json(m_data, m_path.string(), m_buf)) {
    qWarning() << "Failed to save search history to" << m_path.string().c_str() << glz::format_error(error);
  }
}

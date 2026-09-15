#include "dictation-history.hpp"
#include <algorithm>
#include <utility>
#include <QDateTime>

namespace {
constexpr auto MAX_HISTORY_SIZE = 1000;
} // namespace

DictationHistory::DictationHistory(std::filesystem::path path, QObject *parent)
    : QObject(parent), m_file(std::move(path)) {}

void DictationHistory::add(Entry entry) {
  if (entry.text.empty()) return;

  if (entry.createdAt == 0) entry.createdAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  auto &entries = m_file.data().entries;
  entries.insert(entries.begin(), std::move(entry));
  if (entries.size() > MAX_HISTORY_SIZE) entries.resize(MAX_HISTORY_SIZE);

  m_file.save();
  emit changed();
}

bool DictationHistory::remove(const Entry &entry) {
  const auto removed = std::erase_if(m_file.data().entries, [&](const Entry &e) {
    return e.createdAt == entry.createdAt && e.text == entry.text;
  });
  if (removed == 0) return false;

  m_file.save();
  emit changed();
  return true;
}

void DictationHistory::clear() {
  m_file.data().entries.clear();
  m_file.save();
  emit changed();
}

#include "dictation-history.hpp"
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

#include "dictation-vocabulary.hpp"
#include <algorithm>
#include <ranges>
#include <utility>
#include <QDateTime>
#include <QString>

namespace {

bool sameWord(std::string_view a, std::string_view b) {
  return QString::compare(QString::fromUtf8(a.data(), static_cast<qsizetype>(a.size())),
                          QString::fromUtf8(b.data(), static_cast<qsizetype>(b.size())),
                          Qt::CaseInsensitive) == 0;
}

} // namespace

DictationVocabulary::DictationVocabulary(std::filesystem::path path, QObject *parent)
    : QObject(parent), m_file(std::move(path)) {
  rebuildWords();
}

bool DictationVocabulary::contains(std::string_view word) const {
  return std::ranges::any_of(m_words, [&](std::string_view w) { return sameWord(w, word); });
}

bool DictationVocabulary::add(std::string word) {
  if (word.empty() || contains(word)) return false;

  m_file.data().entries.emplace_back(Entry{
      .word = std::move(word),
      .createdAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch()),
  });
  rebuildWords();
  m_file.save();
  emit changed();
  return true;
}

bool DictationVocabulary::remove(std::string_view word) {
  auto &entries = m_file.data().entries;
  const auto removed = std::erase_if(entries, [&](const Entry &e) { return sameWord(e.word, word); });
  if (removed == 0) return false;

  rebuildWords();
  m_file.save();
  emit changed();
  return true;
}

void DictationVocabulary::clear() {
  m_file.data().entries.clear();
  rebuildWords();
  m_file.save();
  emit changed();
}

void DictationVocabulary::rebuildWords() {
  m_words = entries() | std::views::transform([](const Entry &e) { return std::string_view(e.word); }) |
            std::ranges::to<std::vector>();
}

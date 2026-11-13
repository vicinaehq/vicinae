#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <QObject>
#include "utils/json-file.hpp"

/**
 * Words the user wants dictation to recognize, oldest first, kept in one JSON file.
 * Whisper keeps the tail of its initial prompt when it overflows, so newest entries go last.
 */
class DictationVocabulary : public QObject {
  Q_OBJECT

signals:
  void changed();

public:
  struct Entry {
    std::string word;
    std::uint64_t createdAt = 0;
    // Background for rewriting, such as what a name refers to. Not used for transcription.
    std::optional<std::string> hint;
  };

  explicit DictationVocabulary(std::filesystem::path path, QObject *parent = nullptr);

  bool add(std::string word, std::optional<std::string> hint = std::nullopt);
  bool update(std::string_view word, std::string newWord, std::optional<std::string> hint);
  bool remove(std::string_view word);
  void clear();
  bool contains(std::string_view word) const;
  std::span<const Entry> entries() const { return m_file.data().entries; }

  /**
   * Views into the entries. Valid until the vocabulary changes.
   */
  std::span<const std::string_view> words() const { return m_words; }

private:
  struct Data {
    std::vector<Entry> entries;
  };

  void rebuildWords();

  JsonFile<Data> m_file;
  std::vector<std::string_view> m_words;
};

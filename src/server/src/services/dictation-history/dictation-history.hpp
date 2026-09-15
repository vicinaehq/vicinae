#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include <QObject>
#include "utils/json-file.hpp"

/**
 * Past dictations, newest first, kept in one JSON file. Only text that was actually delivered is recorded.
 */
class DictationHistory : public QObject {
  Q_OBJECT

signals:
  void changed();

public:
  struct Entry {
    std::string text;
    std::uint64_t createdAt = 0;
    std::uint64_t durationMs = 0;
    std::optional<std::string> language;
  };

  explicit DictationHistory(std::filesystem::path path, QObject *parent = nullptr);

  void add(Entry entry);
  std::span<const Entry> entries() const { return m_file.data().entries; }

private:
  struct Data {
    std::vector<Entry> entries;
  };

  JsonFile<Data> m_file;
};
